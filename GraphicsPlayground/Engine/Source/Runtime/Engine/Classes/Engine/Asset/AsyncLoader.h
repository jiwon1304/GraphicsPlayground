#pragma once

#include <functional>
#include <future>
#include <atomic>

#include "HAL/PlatformType.h"
#include "Container/Array.h"
#include "Container/String.h"
#include "Container/Queue.h"
#include "AssetInfo.h"

class UAssetManager;

struct FAsyncAssetLoader
{
public:
    FAsyncAssetLoader(UAssetManager* InAssetManager);
    ~FAsyncAssetLoader();

    /**
     * Starts worker thread
     */
    void Initialize();

    /**
     * Stops worker thread
     */
    void Shutdown();

    /**
     * Process works that should be handled in the main thread.
     * Flush the loaded assets to UAssetManager
     * and checks for any new assets to load.
     */
    void Tick();

    /**
     * Add an asset to the async load queue.
     */
    void LoadAssetAsync(const FString& AssetPath);

    /**
     * Waits until all queued assets are loaded.
     */
    void WaitUntilAllAssetsLoaded();

    /**
     * Get the list of loaded assets.
     * Returned info will be cleared from this loader after calling this function.
     */
    TArray<Asset::FAssetInfo> GetLoadedAssets();

protected:
    /**
     * Actual worker function that processes the load queue.
     * The worker thread pops asset paths from the queue
     * and calls this function on them.
     * Should be implemented in the derived class.
     * This is independent of threading logic.
     * @param AssetPath The path of the asset to load.
     * @return The loaded asset info.
     */
    virtual Asset::FAssetInfo Process(const FString& AssetPath) = 0;

    UAssetManager* AssetManager;
    
    // Use this mutex when updating any array or queue
    std::mutex UpdateMutex;
    TQueue<FString> LoadQueue;
    TArray<Asset::FAssetInfo> LoadedAssets;

    std::thread WorkerThread;
    std::atomic<bool> bStopWorker;
    std::atomic<bool> bEmptyQueue;
};