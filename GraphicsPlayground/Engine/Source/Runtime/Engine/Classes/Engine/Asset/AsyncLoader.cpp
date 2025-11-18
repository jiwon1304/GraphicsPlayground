#include "AsyncLoader.h"

FAsyncAssetLoader::FAsyncAssetLoader(UAssetManager *InAssetManager)
{
}

FAsyncAssetLoader::~FAsyncAssetLoader()
{
}

void FAsyncAssetLoader::Initialize()
{
    bStopWorker.store(false);
    bEmptyQueue.store(true);

    WorkerThread = std::thread([this]()
    {
        // loops until shutdown is called
        while (!bStopWorker.load(std::memory_order_acquire))
        {
            // wait until there is work to do
            // be sure the queue is not empty
            while (LoadQueue.IsEmpty())
            {
                // This will be notified in every tick if the queue is not empty
                bEmptyQueue.wait(false, std::memory_order_acquire);
            }

            FString AssetPath;
            {
                std::unique_lock<std::mutex> Lock(UpdateMutex);
                LoadQueue.Dequeue(AssetPath);
            }
            Asset::FAssetInfo LoadedAsset = Process(AssetPath);
            {
                std::lock_guard<std::mutex> Lock(UpdateMutex);
                LoadedAssets.Add(MoveTemp(LoadedAsset));
            }
        }
    });
}

void FAsyncAssetLoader::Shutdown()
{
    bStopWorker.store(true, std::memory_order_release);
    bStopWorker.notify_all();

    assert(WorkerThread.joinable());
    if (WorkerThread.joinable())
    {
        WorkerThread.join();
    }
    
}

void FAsyncAssetLoader::Tick()
{
    if ( LoadQueue.IsEmpty() )
    {
        return;
    }
    else
    {
        bEmptyQueue.store(false, std::memory_order_release);
        bEmptyQueue.notify_all();
    }
}

void FAsyncAssetLoader::LoadAssetAsync(const FString &AssetPath)
{
    {
        std::lock_guard<std::mutex> Lock(UpdateMutex);
        LoadQueue.Enqueue(AssetPath);
    }
    bEmptyQueue.store(false, std::memory_order_release);
    bEmptyQueue.notify_all();
}

void FAsyncAssetLoader::WaitUntilAllAssetsLoaded()
{
    bEmptyQueue.wait(false, std::memory_order_acquire);
}

TArray<Asset::FAssetInfo> FAsyncAssetLoader::GetLoadedAssets()
{
    TArray<Asset::FAssetInfo> Result;
    {
        std::lock_guard<std::mutex> Lock(UpdateMutex);
        Result = MoveTemp(LoadedAssets); // move and clear LoadedAssets
    }
    return Result;
}
