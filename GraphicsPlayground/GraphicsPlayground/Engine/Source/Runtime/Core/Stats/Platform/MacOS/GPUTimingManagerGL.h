#pragma once
#include "Stats/GPUTimingManager.h"
#include "OpenGLDrv/OpenGL3.h"

class FGPUTimingManagerGL final : public IGPUTimingManager
{
public:
    FGPUTimingManagerGL();
    ~FGPUTimingManagerGL() override;

    bool Initialize(const FGPUTimingInitParams& Params) override;
    bool IsInitialized() const override { return bInitialized; }

    void BeginFrame() override;
    void StartTimestamp(const TStatId& StatId) override;
    void StopTimestamp(const TStatId& StatId) override;
    void EndFrame() override;

    void RetrieveResults() override;
    double GetElapsedTimeMs(const TStatId& StatId) const override;

private:
    struct FTimerPair
    {
        GLuint StartQuery = 0;
        GLuint EndQuery = 0;
        bool   bQueryIssued = false;
        TStatId StatId;
    };

private:
    bool bInitialized = false;

    uint32 NumBufferedFrames = 0;
    uint32 CurrentFrameIndex = 0;
    uint32 ResultsFrameIndex = 0;
    uint32 CurrentQueryIndexInFrame = 0;
    uint64 FrameCounter = 0;

    static constexpr uint32 MAX_QUERIES_PER_FRAME = 32;

    TArray<FTimerPair> FrameQueries;
    TMap<FName, FGPUTimingSample> LatestResults;
};