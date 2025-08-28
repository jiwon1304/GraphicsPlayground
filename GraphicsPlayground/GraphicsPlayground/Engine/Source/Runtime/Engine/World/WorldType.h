#pragma once

enum class EWorldType
{
    None,
    Game,
    Editor,
    PIE,
    EditorPreview,
    GamePreview,
    GameRPC,
    SkeletalViewer,
    Inactive,
    PhysicsAssetEditor, // TODO - SkeletalViewer 빼는 Cost 때문에 그냥 박아버림.
};

// TODO : EWolrldType과 EWorldTypeBitFlag 혼용 중. EWorldTypeBitFlag으로 통일하기 
// -> EWorldType은 World의 Type을 나타내는 Enum이며, EWorldTypeBitFlag는 오직 UI를 위해 사용하고 있다. 통일하면 안됨.
enum class EWorldTypeBitFlag
{
    None = 0,
    Game = 1 << 0,
    Editor = 1 << 1,
    PIE = 1 << 2,
    EditorPreview = 1 << 3,
    GamePreview = 1 << 4,
    GameRPC = 1 << 5,
    SkeletalViewer = 1 << 6,
    Inactive = 1 << 7,
    PhysicsAssetEditor = 1 << 8,
};

inline EWorldTypeBitFlag operator|(EWorldTypeBitFlag A, EWorldTypeBitFlag B)
{
    return static_cast<EWorldTypeBitFlag>(
        static_cast<uint32>(A) | static_cast<uint32>(B)
    );
}

inline EWorldTypeBitFlag operator&(EWorldTypeBitFlag A, EWorldTypeBitFlag B)
{
    return static_cast<EWorldTypeBitFlag>(
        static_cast<uint32>(A) & static_cast<uint32>(B)
    );
}

inline bool HasFlag(EWorldTypeBitFlag Mask, EWorldTypeBitFlag Flag)
{
    return (static_cast<uint32>(Mask) & static_cast<uint32>(Flag)) != 0;
}
