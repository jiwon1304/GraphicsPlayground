#include "GPUTimingManagerD3D11.h"
#include <assert.h>

using Microsoft::WRL::ComPtr;

FGPUTimingManagerD3D11::FGPUTimingManagerD3D11() = default;

bool FGPUTimingManagerD3D11::Initialize(const FGPUTimingInitParams& Params)
{
    if (bInitialized) return true;
    if (!Params.D3DDevice || !Params.D3DContext || Params.NumBufferedFrames < 2) return false;

    Device = static_cast<ID3D11Device*>(Params.D3DDevice);
    Context = static_cast<ID3D11DeviceContext*>(Params.D3DContext);
    NumBufferedFrames = Params.NumBufferedFrames;

    FrameQueries.Reserve(NumBufferedFrames * MAX_QUERIES_PER_FRAME);
    for (uint32 i = 0; i < NumBufferedFrames * MAX_QUERIES_PER_FRAME; ++i)
    {
        FrameQueries.Add(FFrameQueries());
    }

    D3D11_QUERY_DESC QueryDesc = {};
    QueryDesc.MiscFlags = 0;

    for (uint32 i = 0; i < NumBufferedFrames; ++i)
    {
        QueryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        HRESULT hr = Device->CreateQuery(&QueryDesc, FrameQueries[i * MAX_QUERIES_PER_FRAME].DisjointQuery.GetAddressOf());
        if (FAILED(hr)) return false;

        QueryDesc.Query = D3D11_QUERY_TIMESTAMP;
        for (uint32 j = 0; j < MAX_QUERIES_PER_FRAME; ++j)
        {
            const uint32 QueryIndex = i * MAX_QUERIES_PER_FRAME + j;

            if (j > 0)
                FrameQueries[QueryIndex].DisjointQuery = FrameQueries[i * MAX_QUERIES_PER_FRAME].DisjointQuery;

            hr = Device->CreateQuery(&QueryDesc, FrameQueries[QueryIndex].StartQuery.GetAddressOf());
            if (FAILED(hr)) return false;

            hr = Device->CreateQuery(&QueryDesc, FrameQueries[QueryIndex].EndQuery.GetAddressOf());
            if (FAILED(hr)) return false;

            FrameQueries[QueryIndex].bQueryIssued = false;
            FrameQueries[QueryIndex].StatId = TStatId();
        }
    }

    CurrentFrameIndex = 0;
    ResultsFrameIndex = (CurrentFrameIndex + 1) % NumBufferedFrames;
    bInitialized = true;
    return true;
}

void FGPUTimingManagerD3D11::BeginFrame()
{
    if (!bInitialized) return;

    LatestResults.Empty();
    RetrieveResults();

    CurrentQueryIndexInFrame = 0;

    const uint32 BaseQueryIndex = CurrentFrameIndex * MAX_QUERIES_PER_FRAME;
    Context->Begin(FrameQueries[BaseQueryIndex].DisjointQuery.Get());

    FrameCounter++;
}

void FGPUTimingManagerD3D11::StartTimestamp(const TStatId& StatId)
{
    if (!bInitialized || CurrentQueryIndexInFrame >= MAX_QUERIES_PER_FRAME)
        return;

    const uint32 QueryIndex = CurrentFrameIndex * MAX_QUERIES_PER_FRAME + CurrentQueryIndexInFrame;
    Context->End(FrameQueries[QueryIndex].StartQuery.Get());
    FrameQueries[QueryIndex].StatId = StatId;
    FrameQueries[QueryIndex].bQueryIssued = true;
}

void FGPUTimingManagerD3D11::StopTimestamp(const TStatId& StatId)
{
    if (!bInitialized || CurrentQueryIndexInFrame >= MAX_QUERIES_PER_FRAME)
        return;

    const uint32 QueryIndex = CurrentFrameIndex * MAX_QUERIES_PER_FRAME + CurrentQueryIndexInFrame;

    if (FrameQueries[QueryIndex].StatId != StatId || !FrameQueries[QueryIndex].bQueryIssued)
        return;

    Context->End(FrameQueries[QueryIndex].EndQuery.Get());
    CurrentQueryIndexInFrame++;
}

void FGPUTimingManagerD3D11::EndFrame()
{
    if (!bInitialized) return;

    const uint32 BaseQueryIndex = CurrentFrameIndex * MAX_QUERIES_PER_FRAME;
    Context->End(FrameQueries[BaseQueryIndex].DisjointQuery.Get());

    CurrentFrameIndex = (CurrentFrameIndex + 1) % NumBufferedFrames;
    ResultsFrameIndex = (CurrentFrameIndex + 1) % NumBufferedFrames;
}

void FGPUTimingManagerD3D11::RetrieveResults()
{
    if (!bInitialized) return;

    const uint32 BaseResultIndex = ResultsFrameIndex * MAX_QUERIES_PER_FRAME;

    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT DisjointData;
    HRESULT hr = Context->GetData(FrameQueries[BaseResultIndex].DisjointQuery.Get(), &DisjointData, sizeof(DisjointData), D3D11_ASYNC_GETDATA_DONOTFLUSH);

    if (hr == S_OK)
    {
        for (uint32 i = 0; i < MAX_QUERIES_PER_FRAME; ++i)
        {
            const uint32 QueryIndex = BaseResultIndex + i;
            if (!FrameQueries[QueryIndex].bQueryIssued) continue;

            double ResultMS = -2.0;

            if (DisjointData.Disjoint)
            {
                ResultMS = -1.0;
            }
            else
            {
                UINT64 StartTime = 0;
                HRESULT hrStart = Context->GetData(FrameQueries[QueryIndex].StartQuery.Get(), &StartTime, sizeof(StartTime), D3D11_ASYNC_GETDATA_DONOTFLUSH);

                UINT64 EndTime = 0;
                HRESULT hrEnd = Context->GetData(FrameQueries[QueryIndex].EndQuery.Get(), &EndTime, sizeof(EndTime), D3D11_ASYNC_GETDATA_DONOTFLUSH);

                if (hrStart == S_OK && hrEnd == S_OK)
                {
                    if (EndTime >= StartTime)
                    {
                        const UINT64 Delta = EndTime - StartTime;
                        ResultMS = (static_cast<double>(Delta) / static_cast<double>(DisjointData.Frequency)) * 1000.0;
                    }
                    else
                    {
                        ResultMS = -4.0;
                    }
                }
            }

            const TStatId& statId = FrameQueries[QueryIndex].StatId;
            if (statId.GetName() != NAME_None)
            {
                LatestResults[statId.GetName()] = { ResultMS, FrameCounter };
            }

            FrameQueries[QueryIndex].bQueryIssued = false;
            FrameQueries[QueryIndex].StatId = TStatId();
        }
    }
}

double FGPUTimingManagerD3D11::GetElapsedTimeMs(const TStatId& StatId) const
{
    if (!bInitialized) return -3.0;
    if (const FGPUTimingSample* it = LatestResults.Find(StatId.GetName()))
        return it->ElapsedTimeMs;
    return -3.0;
}