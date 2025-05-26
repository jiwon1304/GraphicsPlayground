#include "PhysicsAssetEditorPanel.h"

#include "Engine/EditorEngine.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "World/PhysicsAssetWorld.h"

const float	DefaultPrimSize = 15.0f;
const float	DuplicateXOffset = 10.0f;

FPhysicsAssetEditorPanel::FPhysicsAssetEditorPanel()
{
    SetSupportedWorldTypes(EWorldTypeBitFlag::PhysicsAssetEditor);
}

void FPhysicsAssetEditorPanel::Render()
{
    // TODO UISOO Implement (늦어도 ㄱㅊ)
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    if (EditorEngine->ActiveWorld == nullptr || EditorEngine->PhysicsAssetEditorWorld == nullptr)
    {
        return;
    }

    USkeleton* Skeleton = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset()->GetSkeleton();
    if (BoneIconSRV == nullptr || NonWeightBoneIconSRV == nullptr) {
        LoadBoneIcon();
    }

    /* Pre Setup */
    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.7f;

    float PanelPosX = (Width) * 0.8f+5.0f;
    float PanelPosY = 5.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    // TODO UISOO Implement First

    // Draw

    RenderAddShapeButton();
    ImGui::SameLine();
    RenderPhysicsAssetFilter();

    // Delete Key -> Delete
    // Internal -> Add/Remove - BobySetup
    //          -> Add/Remove - Shape
    //          -> Add/Remove - Constraint
    //          -> Select - Bone, BodySetup(Shape), Constraint
    //          -> Delete Key
    
    // RenderSkeletonBoneTree();    // Render Bone, Body, Constraint ...
    
    float ExitPanelWidth = (Width) * 0.2f - 6.0f;
    float ExitPanelHeight = 30.0f;

    float ExitPanelPosX = Width - ExitPanelWidth;
    float ExitPanelPosY = Height - ExitPanelHeight - 10;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::SetNextWindowSize(ImVec2(ExitPanelWidth, ExitPanelHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(ExitPanelPosX, ExitPanelPosY), ImGuiCond_Always);

    constexpr ImGuiWindowFlags ExitPanelFlags =
        ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoScrollbar;
        
    ImGui::Begin("Exit Physics Asset Editor", nullptr, ExitPanelFlags);
    if (ImGui::Button("Exit", ImVec2(ExitPanelWidth, ExitPanelHeight))) {
        // TODO UISOO Check
        // ClearRefSkeletalMeshComponent();
        EditorEngine->EndPhysicsAssetEditor();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void FPhysicsAssetEditorPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    if (hWnd && GetClientRect(hWnd, &clientRect))
    {
        Width = static_cast<float>(clientRect.right - clientRect.left);
        Height = static_cast<float>(clientRect.bottom - clientRect.top);
    }
}

void FPhysicsAssetEditorPanel::RenderAddShapeButton()
{
    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 ButtonSize = ImVec2(32, 32);
    
    ImGui::PushFont(IconFont);
    if (ImGui::Button("\ue9c8", ButtonSize))
    {
        ImGui::OpenPopup("Add Primitive");
    }
    ImGui::PopFont();

    if (ImGui::BeginPopup("Add Primitive"))
    {
        struct FShape
        {
            const char* Label;
            EAggCollisionShape::Type ShapeType;
        };

        static constexpr FShape Shapes[] =
        {
            {.Label = "Box",      .ShapeType = EAggCollisionShape::Box },
            {.Label = "Sphere",    .ShapeType = EAggCollisionShape::Sphere },
            {.Label = "Capsule", .ShapeType = EAggCollisionShape::Sphyl },
        };

        // for (const auto& Shape : Shapes)
        // {
        //     if (ImGui::Selectable(Shape.Label))
        //     {
        //         UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
        //         if (!EditorEngine)
        //         {
        //             return;
        //         }
        //
        //         int32 SelectedBoneIndex = EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex;
        //         int32 SelectedBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
        //
        //         int32 ParentBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedShape.ParentBodySetupIndex;
        //         int32 SelectedShapeIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedShape.SelectedShapeIndex;
        //         EAggCollisionShape::Type ShapeType = EditorEngine->PhysicsAssetEditorWorld->SelectedShape.ShapeType;
        //
        //         if (SelectedBoneIndex == -1 && SelectedBodySetupIndex == -1 && (SelectedShapeIndex == -1 || ParentBodySetupIndex == -1))
        //         {
        //             return;
        //         }
        //
        //         USkeletalMesh* SkeletalMesh = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
        //         UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
        //         UBodySetup* SelectedBodySetup = PhysicsAsset->BodySetup[SelectedBodySetupIndex];
        //         const FMeshBoneInfo& SelectedBoneInfo = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetRawRefBoneInfo()[SelectedBoneIndex];
        //         const FTransform& SelectedBonePose = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetRawRefBonePose()[SelectedBoneIndex];
        //
        //         UBodySetup* ParentBodySetup = PhysicsAsset->BodySetup[SelectedBodySetupIndex];
        //         FKShapeElem ShapeElem;
        //         if (ShapeType == EAggCollisionShape::Box)
        //         {
        //             ShapeElem = ParentBodySetup->AggGeom.BoxElems[SelectedShapeIndex];
        //         }
        //         else if (ShapeType == EAggCollisionShape::Sphere)
        //         {
        //             ShapeElem = ParentBodySetup->AggGeom.SphereElems[SelectedShapeIndex];
        //
        //         }
        //         
        //         if (SelectedBone == nullptr && SelectedBodySetup == nullptr && SelectedShape == nullptr)
        //         {
        //             continue;
        //         }
        //         UWorld* World = GEngine->ActiveWorld;
        //         AActor* SpawnedActor = nullptr;
        //         switch (Shape.ShapeType)
        //         {
        //         case EAggCollisionShape::Sphere:
        //         {
        //             FKSphereElem SphereElem = FKSphereElem();
        //             SelectedBodySetup->AggGeom.SphereElems.Add(SphereElem);
        //             SphereElem.Center = FVector::ZeroVector;
        //             SphereElem.Radius = DefaultPrimSize;
        //             break;
        //         }
        //         case EAggCollisionShape::Box:
        //         {
        //             FKBoxElem BoxElem = FKBoxElem();
        //             SelectedBodySetup->AggGeom.BoxElems.Add(BoxElem);
        //             BoxElem.SetTransform( FTransform::Identity );
        //
        //             BoxElem.X = 0.5f * DefaultPrimSize;
        //             BoxElem.Y = 0.5f * DefaultPrimSize;
        //             BoxElem.Z = 0.5f * DefaultPrimSize;
        //             break;
        //         }
        //         case EAggCollisionShape::Sphyl:
        //         {
        //             FKSphylElem SphylElem = FKSphylElem();
        //             SelectedBodySetup->AggGeom.SphylElems.Add(SphylElem);
        //
        //             SphylElem.SetTransform( FTransform::Identity );
        //
        //             SphylElem.Length = DefaultPrimSize;
        //             SphylElem.Radius = DefaultPrimSize;
        //             
        //             break;
        //         }
        //         default:
        //             break;
        //         }
        //
        //         ClearSelectedBody();
        //     }
        // }
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::RenderPhysicsAssetFilter()
{
}

void FPhysicsAssetEditorPanel::RenderSkeletonBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex)
{
}

void FPhysicsAssetEditorPanel::LoadBoneIcon()
{
    BoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Bone_16x.PNG")->TextureSRV;
    NonWeightBoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/BoneNonWeighted_16x.PNG")->TextureSRV;
}
