#include "GPUTimingManagerGL.h"
#include <assert.h>

FGPUTimingManagerGL::FGPUTimingManagerGL() = default;

FGPUTimingManagerGL::~FGPUTimingManagerGL()
{
    if (!bInitialized) return;
    for (FTimerPair& q : FrameQueries)
    {
        if (q.StartQuery) glDeleteQueries(1, &q.StartQuery);
        if (q.EndQuery)   glDeleteQueries(1, &q.EndQuery);
        q.StartQuery = q.EndQuery = 0;
    }
    bInitialized = false;
}

bool FGPUTimingManagerGL::Initialize(const FGPUTimingInitParams& Params)
{
    if (bInitialized) return true;
    if (Params.NumBufferedFrames < 2) return false;

    NumBufferedFrames = Params.NumBufferedFrames;

    FrameQueries.Reserve(NumBufferedFrames * MAX_QUERIES_PER_FRAME);
    for (uint32 i = 0; i < NumBufferedFrames * MAX_QUERIES_PER_FRAME; ++i)
    {
        FrameQueries.Add(FTimerPair{});
        glGenQueries(1, &FrameQueries[i].StartQuery);
        glGenQueries(1, &FrameQueries[i].EndQuery);
        FrameQueries[i].bQueryIssued = false;
        FrameQueries[i].StatId = TStatId();
    }

    CurrentFrameIndex = 0;
    ResultsFrameIndex = (CurrentFrameIndex + 1) % NumBufferedFrames;
    bInitialized = true;
    return true;
}

void FGPUTimingManagerGL::BeginFrame()
{
    if (!bInitialized) return;

    LatestResults.Empty();
    RetrieveResults();
    CurrentQueryIndexInFrame = 0;
    FrameCounter++;
}

void FGPUTimingManagerGL::StartTimestamp(const TStatId& StatId)
{
    if (!bInitialized || CurrentQueryIndexInFrame >= MAX_QUERIES_PER_FRAME)
        return;

    const uint32 QueryIndex = CurrentFrameIndex * MAX_QUERIES_PER_FRAME + CurrentQueryIndexInFrame;
    glQueryCounter(FrameQueries[QueryIndex].StartQuery, GL_TIMESTAMP);
    FrameQueries[QueryIndex].StatId = StatId;
    FrameQueries[QueryIndex].bQueryIssued = true;
}

void FGPUTimingManagerGL::StopTimestamp(const TStatId& StatId)
{
    if (!bInitialized || CurrentQueryIndexInFrame >= MAX_QUERIES_PER_FRAME)
        return;

    const uint32 QueryIndex = CurrentFrameIndex * MAX_QUERIES_PER_FRAME + CurrentQueryIndexInFrame;
    if (FrameQueries[QueryIndex].StatId != StatId || !FrameQueries[QueryIndex].bQueryIssued)
        return;

    glQueryCounter(FrameQueries[QueryIndex].EndQuery, GL_TIMESTAMP);
    CurrentQueryIndexInFrame++;
}

void FGPUTimingManagerGL::EndFrame()
{
    if (!bInitialized) return;
    CurrentFrameIndex = (CurrentFrameIndex + 1) % NumBufferedFrames;
    ResultsFrameIndex = (CurrentFrameIndex + 1) % NumBufferedFrames;
}

void FGPUTimingManagerGL::RetrieveResults()
{
    if (!bInitialized) return;

    const uint32 BaseResultIndex = ResultsFrameIndex * MAX_QUERIES_PER_FRAME;

    for (uint32 i = 0; i < MAX_QUERIES_PER_FRAME; ++i)
    {
        const uint32 QueryIndex = BaseResultIndex + i;
        FTimerPair& Pair = FrameQueries[QueryIndex];
        if (!Pair.bQueryIssued) continue;

        GLint startAvailable = 0, endAvailable = 0;
        glGetQueryObjectiv(Pair.StartQuery, GL_QUERY_RESULT_AVAILABLE, &startAvailable);
        glGetQueryObjectiv(Pair.EndQuery,   GL_QUERY_RESULT_AVAILABLE, &endAvailable);

        if (startAvailable && endAvailable)
        {
            GLuint64 startNs = 0, endNs = 0;
            glGetQueryObjectui64v(Pair.StartQuery, GL_QUERY_RESULT, &startNs);
            glGetQueryObjectui64v(Pair.EndQuery,   GL_QUERY_RESULT, &endNs);

            double resultMs = -4.0;
            if (endNs >= startNs)
            {
                const double deltaNs = static_cast<double>(endNs - startNs);
                resultMs = deltaNs / 1.0e6;
            }

            const TStatId& statId = Pair.StatId;
            if (statId.GetName() != NAME_None)
            {
                LatestResults[statId.GetName()] = { resultMs, FrameCounter };
            }

            Pair.bQueryIssued = false;
            Pair.StatId = TStatId();
        }
    }
}

double FGPUTimingManagerGL::GetElapsedTimeMs(const TStatId& StatId) const
{
    if (!bInitialized) return -3.0;
    if (const FGPUTimingSample* found = LatestResults.Find(StatId.GetName()))
        return found->ElapsedTimeMs;
    return -3.0;
}