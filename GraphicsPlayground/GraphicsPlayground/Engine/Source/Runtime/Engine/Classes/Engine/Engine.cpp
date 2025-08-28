#include "Engine.h"

#include "EditorEngine.h"
#include "WindowsPlatformTime.h"
#include "UnrealEd/SceneManager.h"
#include "UObject/Casts.h"
#include "World/World.h"
#include "PhysicsCore/PhysxSolversModule.h"

UEngine* GEngine = nullptr;

void UEngine::Init()
{
    // Random Seed 초기화
    {
        const uint32 Seed1 = FPlatformTime::Cycles();
        const uint32 Seed2 = FPlatformTime::Cycles();

        FMath::RandInit(static_cast<int32>(Seed1));
        FMath::SRandInit(static_cast<int32>(Seed2));

        UE_LOG(ELogLevel::Display, TEXT("RandInit(%d) SRandInit(%d)."), Seed1, Seed2);
    }

    // PhysX 초기화
    FPhysxSolversModule::GetModule();

    // 컴파일 타임에 확정되지 못한 타입을 런타임에 검사
    UStruct::ResolvePendingProperties();
}

FWorldContext* UEngine::GetWorldContextFromWorld(const UWorld* InWorld)
{
    for (FWorldContext* WorldContext : WorldList)
    {
        if (WorldContext->World() == InWorld)
        {
            return WorldContext;
        }
    }
    return nullptr;
}

FWorldContext& UEngine::CreateNewWorldContext(EWorldType InWorldType)
{
    FWorldContext* NewWorldContext = new FWorldContext();
    WorldList.Add(NewWorldContext);
    NewWorldContext->WorldType = InWorldType;
    NewWorldContext->ContextHandle = FName(*FString::Printf(TEXT("WorldContext_%d"), NextWorldContextHandle++));

    return *NewWorldContext;
}


void UEngine::LoadLevel(const FString& FileName) const
{
    SceneManager::LoadSceneFromJsonFile(*FileName, *ActiveWorld);
}

void UEngine::SaveLevel(const FString& FileName) const
{
    SceneManager::SaveSceneToJsonFile(*FileName, *ActiveWorld);
}
