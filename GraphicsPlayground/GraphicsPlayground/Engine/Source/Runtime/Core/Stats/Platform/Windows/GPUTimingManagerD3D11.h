#pragma once
#include "Stats/GPUTimingManager.h"
#include <d3d11.h>
#include <wrl/client.h>

class FGPUTimingManagerD3D11 final : public IGPUTimingManager
{
public:
    FGPUTimingManagerD3D11();
    ~FGPUTimingManagerD3D11() override = default;

    bool Initialize(const FGPUTimingInitParams& Params) override;

    void BeginFrame() override;
    void StartTimestamp(const TStatId& StatId) override;
    void StopTimestamp(const TStatId& StatId) override;
    void EndFrame() override;

    void RetrieveResults() override;
    double GetElapsedTimeMs(const TStatId& StatId) const override;

private:
    struct FFrameQueries
    {
        Microsoft::WRL::ComPtr<ID3D11Query> StartQuery;
        Microsoft::WRL::ComPtr<ID3D11Query> EndQuery;
        Microsoft::WRL::ComPtr<ID3D11Query> DisjointQuery;
        bool bQueryIssued = false;
        TStatId StatId;
    };

private:
    bool bInitialized = false;

    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* Context = nullptr;

    uint32 NumBufferedFrames = 0;
    uint32 CurrentFrameIndex = 0;
    uint32 ResultsFrameIndex = 0;
    uint32 CurrentQueryIndexInFrame = 0;
    uint64 FrameCounter = 0;

    static constexpr uint32 MAX_QUERIES_PER_FRAME = 32;

    TArray<FFrameQueries> FrameQueries;
    TMap<FName, FGPUTimingSample> LatestResults;
};