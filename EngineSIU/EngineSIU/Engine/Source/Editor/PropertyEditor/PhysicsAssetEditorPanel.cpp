#include "PhysicsAssetEditorPanel.h"

#include "Engine/EditorEngine.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "World/PhysicsAssetWorld.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "PhysicalMaterials/Defines.h"

const float	DefaultPrimSize = 15.0f;
const float	DuplicateXOffset = 10.0f;
const ImVec4 InActiveTextColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

FPhysicsAssetEditorPanel::FPhysicsAssetEditorPanel()
{
    SetSupportedWorldTypes(EWorldTypeBitFlag::PhysicsAssetEditor);
}

void FPhysicsAssetEditorPanel::Render()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    if (EditorEngine->ActiveWorld == nullptr || EditorEngine->PhysicsAssetEditorWorld == nullptr)
    {
        return;
    }

    if (BoneIconSRV == nullptr || NonWeightBoneIconSRV == nullptr || BodySetupIconSRV == nullptr || BoxIconSRV == nullptr || SphereIconSRV == nullptr || SphylIconSRV == nullptr || ConstraintIconSRV == nullptr
        || BoxPhATIconSRV == nullptr || SpherePhATIconSRV == nullptr || SphylPhATIconSRV == nullptr)
    {
        LoadBoneIcon();
    }

    /* Pre Setup */
    float TreePanelWidth = (Width) * 0.2f - 6.0f;
    float TreePanelHeight = (Height - 30.f) * 0.5f - 10.f;

    float TreePanelPosX = (Width) * 0.8f + 5.0f;
    float TreePanelPosY = 5.0f;

    ImVec2 TreeMinSize(140, 100);
    ImVec2 TreeMaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(TreeMinSize, TreeMaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(TreePanelPosX, TreePanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(TreePanelWidth, TreePanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    ImGui::Begin("Skeletal Tree", nullptr, PanelFlags); // 창 이름 변경
    RenderAddPrimitiveButton();
    ImGui::SameLine();
    RenderPhysicsAssetFilter();

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::BeginChild("Skeletal Tree Id");
    RenderSkeletonBoneTree();    // Render Bone, Body, Constraint ...
    ImGui::EndChild();
    ImGui::End();

    /* Detail Pre Setup */
    
    float DetailPanelWidth = (Width) * 0.2f - 6.0f;
    float DetailPanelHeight = (Height - 30.f) * 0.5f - 40.f;

    float DetailPanelPosX = (Width) * 0.8f + 5.0f;
    float DetailPanelPosY = (Height - 30.f) * 0.5f;

    ImVec2 DetailMinSize(140, 100);
    ImVec2 DetailMaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(DetailMinSize, DetailMaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(DetailPanelPosX, DetailPanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(DetailPanelWidth, DetailPanelHeight), ImGuiCond_Always);


    ImGui::Begin("Detail", nullptr, PanelFlags); // 창 이름 변경

    RenderDetailPanel();

    ImGui::End();
    
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
    if (ImGui::Button("Exit", ImVec2(ExitPanelWidth, ExitPanelHeight)))
    {
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

void FPhysicsAssetEditorPanel::RenderAddPrimitiveButton()
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
        
        ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
        ImGui::SeparatorText("Shape Type");
        ImGui::PopStyleColor();
        for (const auto& Shape : Shapes)
        {
            if (Shape.ShapeType == EAggCollisionShape::Box)
            {
                ImGui::Image((ImTextureID)BoxPhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            }
            else if (Shape.ShapeType == EAggCollisionShape::Sphere)
            {
                ImGui::Image((ImTextureID)SpherePhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            }
            else if (Shape.ShapeType == EAggCollisionShape::Sphyl)
            {
                ImGui::Image((ImTextureID)SphylPhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            }
            ImGui::SameLine();
            if (ImGui::Selectable(Shape.Label))
            {
                UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
                if (!EditorEngine)
                {
                    continue;
                }
        
                int32 SelectedBoneIndex = EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex;
                int32 SelectedBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
        
                int32 ParentBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex;
                int32 SelectedPrimitiveIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex;
                EAggCollisionShape::Type PrimitiveType = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType;
        
                if (SelectedBoneIndex == -1 && SelectedBodySetupIndex == -1 && (SelectedPrimitiveIndex == -1 || ParentBodySetupIndex == -1))
                {
                    continue;
                }
        
                USkeletalMesh* SkeletalMesh = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
                UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
                
                UBodySetup* SelectedBodySetup = SelectedBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[SelectedBodySetupIndex];
                bool isBoneValid = SelectedBoneIndex == -1 ? false : SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().IsValidRawIndex(SelectedBoneIndex);
                UBodySetup* ParentBodySetup = ParentBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[ParentBodySetupIndex];
                
                bool isPrimitiveExist = false;
                if (PrimitiveType == EAggCollisionShape::Box)
                {
                    if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.BoxElems.Num())
                    {
                        isPrimitiveExist = true;
                    }
                }
                else if (PrimitiveType == EAggCollisionShape::Sphere)
                {
                    if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphereElems.Num())
                    {
                        isPrimitiveExist = true;
                    }
                }
                else if (PrimitiveType == EAggCollisionShape::Sphyl)
                {
                    if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphylElems.Num())
                    {
                        isPrimitiveExist = true;
                    }
                }

                if (!isPrimitiveExist)
                {
                    ParentBodySetup = nullptr;
                }
                
                if (isBoneValid == false && SelectedBodySetup == nullptr && ParentBodySetup == nullptr)
                {
                    continue;
                }

                UBodySetup* TargetBodySetup = nullptr;

                if (isBoneValid)
                {
                    FName BoneName = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetBoneName(SelectedBoneIndex);
                    int32 BodyIndex = PhysicsAsset->FindBodyIndex(BoneName);
                    if (BodyIndex == -1)
                    {
                        TargetBodySetup = FObjectFactory::ConstructObject<UBodySetup>(PhysicsAsset);

                        TargetBodySetup->BoneName = BoneName;

                        TargetBodySetup->CollisionResponse = EBodyCollisionResponse::Type::BodyCollision_Enabled;
                        TargetBodySetup->DefaultInstance.ExternalCollisionProfileBodySetup = TargetBodySetup;
                        TargetBodySetup->DefaultInstance.InstanceBodyIndex = BodyIndex;
                        TargetBodySetup->DefaultInstance.InstanceBoneIndex = SelectedBoneIndex;

                        FPhysicsMaterial* PhysicsMaterial = new FPhysicsMaterial();
                        PhysicsMaterial->Density = 1000.f; // 임시 밀도 값

                        // !TODO : 디폴트 머티리얼을 하나 만들어두고 그걸 사용하도록 해야 함
                        UPhysicalMaterial* PhysMaterial = FObjectFactory::ConstructObject<UPhysicalMaterial>(nullptr);
                        PhysMaterial->Material = PhysicsMaterial;

                        TargetBodySetup->PhysMaterial = PhysMaterial;
                        PhysicsAsset->BodySetup.Add(TargetBodySetup);
                        PhysicsAsset->UpdateBodySetupIndexMap();
                    }
                    else
                    {
                        TargetBodySetup = PhysicsAsset->BodySetup[BodyIndex];
                    }
                }
                else if (SelectedBodySetup != nullptr)
                {
                    TargetBodySetup = SelectedBodySetup;
                }
                else if (ParentBodySetup != nullptr)
                {
                    TargetBodySetup = ParentBodySetup;
                }
                if (TargetBodySetup != nullptr || SelectedBoneIndex != -1)
                {
                    AddShape(PhysicsAsset, TargetBodySetup, SelectedBoneIndex, Shape.ShapeType);
                }
            }
        }
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::RenderPhysicsAssetFilter()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 ButtonSize = ImVec2(32, 32);
    
    ImGui::PushFont(IconFont);
    if (ImGui::Button("\ue9c4", ButtonSize))
    {
        ImGui::OpenPopup("Physics Asset Filter");
    }
    ImGui::PopFont();
    
    if (ImGui::BeginPopup("Physics Asset Filter"))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
        ImGui::SeparatorText("Bone Option");
        ImGui::PopStyleColor();
        bool ShowBones = EditorEngine->PhysicsAssetEditorWorld->bShowBones;
        if (ImGui::Checkbox("Show Bone", &ShowBones))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowBones = ShowBones;
        }

        bool ShowBoneIndices = EditorEngine->PhysicsAssetEditorWorld->bShowBoneIndices;
        if (ImGui::Checkbox("Show Bone Index", &ShowBoneIndices))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowBoneIndices = ShowBoneIndices;
        }
        ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
        ImGui::SeparatorText("Physics Asset Filter");
        ImGui::PopStyleColor();
        bool ShowBodies = EditorEngine->PhysicsAssetEditorWorld->bShowBodies;
        if (ImGui::Checkbox("Show Body", &ShowBodies))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowBodies = ShowBodies;
        }
        bool ShowConstraints = EditorEngine->PhysicsAssetEditorWorld->bShowConstraints;
        if (ImGui::Checkbox("Show Constraint", &ShowConstraints))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowConstraints = ShowConstraints;
        }
        bool ShowPrimitives = EditorEngine->PhysicsAssetEditorWorld->bShowPrimitives;
        if (ImGui::Checkbox("Show Primitive", &ShowPrimitives))
        {
            EditorEngine->PhysicsAssetEditorWorld->bShowPrimitives = ShowPrimitives;
        }
        
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::RenderSkeletonBoneTree()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    USkeletalMeshComponent* SelectedSkeletalMeshComponent = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent();
    USkeletalMesh* SelectedSkeletalMesh = SelectedSkeletalMeshComponent->GetSkeletalMeshAsset();
    UPhysicsAsset* SelectedPhysicsAsset = SelectedSkeletalMesh->GetPhysicsAsset();

    int32 BoneIndex = 0;
    RenderTreeRecursive(SelectedSkeletalMesh, SelectedPhysicsAsset, BoneIndex, EditorEngine->PhysicsAssetEditorWorld->bShowBones, EditorEngine->PhysicsAssetEditorWorld->bShowBoneIndices, EditorEngine->PhysicsAssetEditorWorld->bShowBodies, EditorEngine->PhysicsAssetEditorWorld->bShowConstraints, EditorEngine->PhysicsAssetEditorWorld->bShowPrimitives);
}

void FPhysicsAssetEditorPanel::RenderDetailPanel()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    int32 SelectedBoneIndex = EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex;
    int32 SelectedBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
    int32 SelectedConstraintIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedConstraintIndex;

    int32 ParentBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex;
    int32 SelectedPrimitiveIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex;
    EAggCollisionShape::Type PrimitiveType = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType;

    if (SelectedBoneIndex == -1 && SelectedBodySetupIndex == -1 && (SelectedPrimitiveIndex == -1 || ParentBodySetupIndex == -1) && SelectedConstraintIndex == -1)
    {
        return;
    }

    USkeletalMesh* SkeletalMesh = EditorEngine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
    
    UBodySetup* SelectedBodySetup = SelectedBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[SelectedBodySetupIndex];
    bool isBoneValid = SelectedBoneIndex == -1 ? false : SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().IsValidRawIndex(SelectedBoneIndex);

    UBodySetup* ParentBodySetup = ParentBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[ParentBodySetupIndex];
    UPhysicsConstraintTemplate* PhysicsConstraintTemplate = SelectedConstraintIndex == -1 ? nullptr : PhysicsAsset->ConstraintSetup[SelectedConstraintIndex];
    
    bool isPrimitiveExist = false;
    
    if (PrimitiveType == EAggCollisionShape::Box)
    {
        if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.BoxElems.Num())
        {
            isPrimitiveExist = true;
        }
    }
    else if (PrimitiveType == EAggCollisionShape::Sphere)
    {
        if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphereElems.Num())
        {
            isPrimitiveExist = true;
        }
    }
    else if (PrimitiveType == EAggCollisionShape::Sphyl)
    {
        if (SelectedPrimitiveIndex >= 0 && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphylElems.Num())
        {
            isPrimitiveExist = true;
        }
    }
    
    if (!isPrimitiveExist)
    {
        ParentBodySetup = nullptr;
    }
    
    
    if (isBoneValid == false && SelectedBodySetup == nullptr && ParentBodySetup == nullptr && PhysicsConstraintTemplate == nullptr)
    {
        return;
    }

    UBodySetup* TargetBodySetup = nullptr;
    if (ParentBodySetup)
    {
        TargetBodySetup = ParentBodySetup;
    }
    else if (SelectedBodySetup)
    {
        TargetBodySetup = SelectedBodySetup;
    }

    if (isBoneValid)
    {
        const FReferenceSkeleton& ReferenceSkeleton = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
        if (SelectedBoneIndex != -1)
        {
            FName BoneName = ReferenceSkeleton.GetBoneName(SelectedBoneIndex);
            ImGui::Text("%s", GetData(BoneName.ToString()));
        }
    }
    else if (TargetBodySetup)
    {        
        ImGui::Separator();
        const UClass* Class = TargetBodySetup->GetClass();

        for (; Class; Class = Class->GetSuperClass())
        {
            const TArray<FProperty*>& Properties = Class->GetProperties();
            if (!Properties.IsEmpty())
            {
                ImGui::SeparatorText(*Class->GetName());
            }

            for (const FProperty* Prop : Properties)
            {
                Prop->DisplayInImGui(TargetBodySetup);
            }
        }        
    }
    else if (PhysicsConstraintTemplate)
    {
        ImGui::Separator();
        const UClass* Class = PhysicsConstraintTemplate->GetClass();

        for (; Class; Class = Class->GetSuperClass())
        {
            const TArray<FProperty*>& Properties = Class->GetProperties();
            if (!Properties.IsEmpty())
            {
                ImGui::SeparatorText(*Class->GetName());
            }

            for (const FProperty* Prop : Properties)
            {
                Prop->DisplayInImGui(PhysicsConstraintTemplate);
            }
        }   
    }
}

void FPhysicsAssetEditorPanel::RenderTreeRecursive(USkeletalMesh* InSkeletalMesh, UPhysicsAsset* InPhysicsAsset, int32 InBoneIndex, uint8 bShowBones, uint8 bShowBoneIndices, uint8 bShowBodies,
    uint8 bShowConstraints, uint8 bShowPrimitives)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    const FReferenceSkeleton* ReferenceSkeleton = InSkeletalMesh->GetRefSkeleton();
    const FMeshBoneInfo& BoneInfo = ReferenceSkeleton->RawRefBoneInfo[InBoneIndex];

    const FString& ShortBoneName = GetCleanBoneName(BoneInfo, InBoneIndex, bShowBoneIndices);

    bool bBoneNodeOpen = false;

    ImGui::PushID(InBoneIndex);

    if (bShowBones)
    {
        ImGui::Image((ImTextureID)BoneIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
        ImGui::SameLine();

        ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

        bool bBoneHasChildren = false;
        for (int32 i = 0; i < ReferenceSkeleton->RawRefBoneInfo.Num(); ++i)
        {
            if (ReferenceSkeleton->RawRefBoneInfo[i].ParentIndex == InBoneIndex)
            {
                bBoneHasChildren = true;
                break;
            }
        }

        int32 BodyIndex = InPhysicsAsset->FindBodyIndex(BoneInfo.Name);
        
        if (BodyIndex >= 0 && InPhysicsAsset->BodySetup.Num() > BodyIndex)
        {
            if (bShowBodies)
            {
                bBoneHasChildren = true;
            }
            UBodySetup* BodySetup = InPhysicsAsset->BodySetup[BodyIndex];
            if (bShowPrimitives && (BodySetup->AggGeom.BoxElems.Num() > 0 || BodySetup->AggGeom.SphereElems.Num() || BodySetup->AggGeom.SphylElems.Num()))
            {
                bBoneHasChildren = true;
            }
        }

        if (bShowConstraints && InPhysicsAsset->ConstraintSetup.Num() > 0)
        {
            bBoneHasChildren = true;
        }
        
        if (!bBoneHasChildren)
        {
            NodeFlags |= ImGuiTreeNodeFlags_Leaf; // 자식 없으면 리프 노드
            NodeFlags &= ~ImGuiTreeNodeFlags_OpenOnArrow; // 리프 노드는 화살표로 열 필요 없음
        }

        bBoneNodeOpen = ImGui::TreeNodeEx(*ShortBoneName, NodeFlags);

        if (bBoneNodeOpen)
        {
            UBodySetup* BodySetup = nullptr;
            if (BodyIndex >= 0 && InPhysicsAsset->BodySetup.Num() > BodyIndex)
            {
                BodySetup = InPhysicsAsset->BodySetup[BodyIndex];
            }
            
            DrawPopupBone(InPhysicsAsset, BodySetup, InBoneIndex);
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
        {
            // 엔진에 선택된 본 인덱스 설정 (가상의 함수 호출)
            EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
            EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex = (InBoneIndex);
        }
    }

    
    // TODO 코드가 복잡하다. 
    if ((bShowBones && bBoneNodeOpen) || !bShowBones) // 노드가 열려있다면
    {
        ImGui::PushID("Body");
        bool bDrawChildren = true;
        bool bBodySetupNodeOpen = false;

        // Push & Draw BodySetup
        if (bShowBodies)
        {
            bDrawChildren = false;
            UBodySetup* TargetBodySetup = nullptr;
            int32 TargetBodyIndex = InPhysicsAsset->FindBodyIndex(BoneInfo.Name);
            if (TargetBodyIndex >= 0 && InPhysicsAsset->BodySetup.Num() > TargetBodyIndex)
            {
                TargetBodySetup = InPhysicsAsset->BodySetup[TargetBodyIndex];
            }

            if (TargetBodySetup != nullptr)
            {
                ImGui::Image((ImTextureID)BodySetupIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
                ImGui::SameLine();
            
                bool bBodySetupHasChildren = false;

                if ((TargetBodySetup->AggGeom.BoxElems.Num() > 0 || TargetBodySetup->AggGeom.SphereElems.Num() || TargetBodySetup->AggGeom.SphylElems.Num()))
                {
                    bBodySetupHasChildren = true;
                }

                TArray<int32> Constraints;
                InPhysicsAsset->BodyFindConstraints(TargetBodyIndex, Constraints);
                // Constraint가 있는 경우 
                if (Constraints.Num() > 0)
                {
                    bBodySetupHasChildren = true;
                }
            
                ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
                if (!bBodySetupHasChildren)
                {
                    NodeFlags |= ImGuiTreeNodeFlags_Leaf; // 자식 없으면 리프 노드
                    NodeFlags &= ~ImGuiTreeNodeFlags_OpenOnArrow; // 리프 노드는 화살표로 열 필요 없음
                }

                bBodySetupNodeOpen = ImGui::TreeNodeEx(GetData(GetCleanBoneName(TargetBodySetup->BoneName.ToString())), NodeFlags);
                bool bIsDelete = false;
                if (bBodySetupNodeOpen)
                {
                    DrawPopupBodySetup(InPhysicsAsset, TargetBodySetup, InBoneIndex, bIsDelete);
                    if (!bIsDelete)
                    {
                        bDrawChildren = true;
                    }
                }
                if (!bIsDelete && ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
                {
                    EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                    EditorEngine->PhysicsAssetEditorWorld->SelectedBodySetupIndex = (TargetBodyIndex);
                }   
            }
        }

        // Draw Child
        if (bDrawChildren)
        {
            if (bShowPrimitives)
            {
                int32 TargetBodyIndex = InPhysicsAsset->FindBodyIndex(BoneInfo.Name);
                if (TargetBodyIndex != -1 && InPhysicsAsset->BodySetup.Num() > TargetBodyIndex)
                {
                    UBodySetup* TargetBodySetup = InPhysicsAsset->BodySetup[TargetBodyIndex];
                    for (uint32 PrimitiveIndex = 0; PrimitiveIndex < TargetBodySetup->AggGeom.SphereElems.Num(); PrimitiveIndex++)
                    {
                        EAggCollisionShape::Type PrimitiveType = EAggCollisionShape::Sphere;
                        FKSphereElem& ShapeElem = TargetBodySetup->AggGeom.SphereElems[PrimitiveIndex];
                        ImGui::PushID(GetData(FString::FromInt(static_cast<int32>(PrimitiveType)) + FString::FromInt(PrimitiveIndex)));
                        ImGui::Image((ImTextureID)SphereIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
                        ImGui::SameLine();
        
                        ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;

                        if (ImGui::TreeNodeEx(GetData(GetCleanBoneName(ShapeElem.Name.ToString())), NodeFlags))
                        {
                            bool bIsDelete = false; 
                            DrawPopupPrimitive(InPhysicsAsset, TargetBodySetup, PrimitiveType, PrimitiveIndex, bIsDelete);

                            if (!bIsDelete && ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
                            {
                                EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType = PrimitiveType;
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex = PrimitiveIndex;
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex = (TargetBodyIndex);
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    for (uint32 PrimitiveIndex = 0; PrimitiveIndex < TargetBodySetup->AggGeom.BoxElems.Num(); PrimitiveIndex++)
                    {
                        EAggCollisionShape::Type PrimitiveType = EAggCollisionShape::Box;
                        FKBoxElem& ShapeElem = TargetBodySetup->AggGeom.BoxElems[PrimitiveIndex];
                        ImGui::PushID(GetData(FString::FromInt(static_cast<int32>(PrimitiveType)) + FString::FromInt(PrimitiveIndex)));
                        ImGui::Image((ImTextureID)BoxIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
                        ImGui::SameLine();
        
                        ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;

                        if (ImGui::TreeNodeEx(GetData(GetCleanBoneName(ShapeElem.Name.ToString())), NodeFlags))
                        {
                            bool bIsDelete = false;
                            DrawPopupPrimitive(InPhysicsAsset, TargetBodySetup, PrimitiveType, PrimitiveIndex, bIsDelete);
                            
                            if (!bIsDelete && ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
                            {
                                EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType = PrimitiveType;
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex = PrimitiveIndex;
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex = (TargetBodyIndex);
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    for (uint32 PrimitiveIndex = 0; PrimitiveIndex < TargetBodySetup->AggGeom.SphylElems.Num(); PrimitiveIndex++)
                    {
                        EAggCollisionShape::Type PrimitiveType = EAggCollisionShape::Sphyl;
                        FKSphylElem& ShapeElem = TargetBodySetup->AggGeom.SphylElems[PrimitiveIndex];
                        ImGui::PushID(GetData(FString::FromInt(static_cast<int32>(PrimitiveType)) + FString::FromInt(PrimitiveIndex)));
                        ImGui::Image((ImTextureID)SphylIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
                        ImGui::SameLine();
        
                        ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;

                        if (ImGui::TreeNodeEx(GetData(GetCleanBoneName(ShapeElem.Name.ToString())), NodeFlags))
                        {
                            bool bIsDelete;
                            DrawPopupPrimitive(InPhysicsAsset, TargetBodySetup, PrimitiveType, PrimitiveIndex, bIsDelete);
                            
                            if (!bIsDelete && ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
                            {
                                EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType = PrimitiveType;
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex = PrimitiveIndex;
                                EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex = (TargetBodyIndex);
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                }
            }

            if (bShowConstraints)
            {
                int32 TargetBodyIndex = InPhysicsAsset->FindBodyIndex(BoneInfo.Name);
                if (TargetBodyIndex != -1 && InPhysicsAsset->BodySetup.Num() > TargetBodyIndex)
                {
                    // 동일한 이름의 Constraint가 들어올 수 있음. 유의
                    TArray<int32> ConstraintIndices;
                    InPhysicsAsset->BodyFindConstraints(TargetBodyIndex, ConstraintIndices);
                    for (int32 ConstraintIndex : ConstraintIndices)
                    {
                        ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;
                        UPhysicsConstraintTemplate* ConstraintTemplate = InPhysicsAsset->ConstraintSetup[ConstraintIndex];
                        ImGui::PushID(ConstraintIndex);
                        ImGui::Image((ImTextureID)ConstraintIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
                        ImGui::SameLine();

                        if (ImGui::TreeNodeEx(GetData(FString::Printf("[%s->%s] Constraint", GetData(GetCleanBoneName(ConstraintTemplate->DefaultInstance.ConstraintBone1.ToString())), GetData(GetCleanBoneName(ConstraintTemplate->DefaultInstance.ConstraintBone2.ToString())))), NodeFlags))
                        {
                            bool bIsDelete = false;
                            DrawPopupConstraint(InPhysicsAsset, ConstraintIndex, bIsDelete);
                            
                            if (!bIsDelete && ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
                            {
                                EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                                EditorEngine->PhysicsAssetEditorWorld->SelectedConstraintIndex = ConstraintIndex;
                            }
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                }
            }
        }

        // Pop BodySetup
        if (bShowBones)
        {
            if (bBodySetupNodeOpen)
            {
                ImGui::TreePop(); // 트리 노드 닫기
            }
            ImGui::PopID();
        }

        if (bBoneNodeOpen || (!bShowBones && bBodySetupNodeOpen))
        {
            // 자식 본들 재귀적으로 처리
            for (int32 i = 0; i < ReferenceSkeleton->RawRefBoneInfo.Num(); ++i)
            {
                if (ReferenceSkeleton->RawRefBoneInfo[i].ParentIndex == InBoneIndex)
                {
                    RenderTreeRecursive(InSkeletalMesh, InPhysicsAsset, i, bShowBones, bShowBoneIndices, bShowBodies, bShowConstraints, bShowPrimitives);
                }
            }
        }

        // Pop BodySetup
        if (!bShowBones)
        {
            if (bBodySetupNodeOpen)
            {
                ImGui::TreePop(); // 트리 노드 닫기
            }
            ImGui::PopID();
        }
    }
    
    if (bBoneNodeOpen)
    {
        ImGui::TreePop();
    }
    
    ImGui::PopID(); // ID 스택 복원
}

void FPhysicsAssetEditorPanel::DrawPopupBodySetup(UPhysicsAsset* PhysicsAsset, UBodySetup* BodySetup, int32 InBoneIndex, bool& bIsDelete)
{
    bIsDelete = false;
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    if (ImGui::BeginPopupContextItem("TreeNodeRightClick"))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
        ImGui::SeparatorText("Body");
        ImGui::PopStyleColor();
        if (ImGui::MenuItem("Body Add/Replace (Not Implemented)"))
        {
            // TODO
        }
        if (ImGui::BeginMenu("Add Shape"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
            ImGui::SeparatorText("Shape Type");
            ImGui::Text("(Create Body Setup Automatically)");
            ImGui::PopStyleColor();

            
            ImGui::Image((ImTextureID)BoxPhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            ImGui::SameLine();
            if (ImGui::MenuItem("Add Box"))
            {
                AddShape(PhysicsAsset, BodySetup, InBoneIndex, EAggCollisionShape::Box);
            }
            ImGui::Image((ImTextureID)SpherePhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            ImGui::SameLine();
            if (ImGui::MenuItem("Add Sphere"))
            {
                AddShape(PhysicsAsset, BodySetup, InBoneIndex, EAggCollisionShape::Sphere);
            }
            ImGui::Image((ImTextureID)SphylPhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            ImGui::SameLine();
            if (ImGui::MenuItem("Add Capsule"))
            {
                AddShape(PhysicsAsset, BodySetup, InBoneIndex, EAggCollisionShape::Sphyl);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Constraint"))
        {
            ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Always);

            ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
            ImGui::SeparatorText("Create New Constraint");
            ImGui::PopStyleColor();
            if (ImGui::BeginChild("Create New Constraint"))
            {
                for (UBodySetup* TempBodySetup : PhysicsAsset->BodySetup)
                {
                    if (TempBodySetup == BodySetup)
                    {
                        continue;
                    }

                    ImGui::Image((ImTextureID)ConstraintIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
                    ImGui::SameLine();
                    if (ImGui::MenuItem(GetData(GetCleanBoneName(TempBodySetup->BoneName.ToString()))))
                    {
                        const FMeshBoneInfo& MeshBoneInfo = PhysicsAsset->PreviewSkeletalMesh->GetRefSkeleton()->GetRawRefBoneInfo()[InBoneIndex];
                        int32 OtherBoneIndex = PhysicsAsset->PreviewSkeletalMesh->GetRefSkeleton()->FindBoneIndex(TempBodySetup->BoneName);
                        const FMeshBoneInfo& OtherMeshBoneInfo = PhysicsAsset->PreviewSkeletalMesh->GetRefSkeleton()->GetRawRefBoneInfo()[OtherBoneIndex];
                        UPhysicsConstraintTemplate* NewPhysicsConstraintTemplate = FObjectFactory::ConstructObject<UPhysicsConstraintTemplate>(PhysicsAsset);

                        PhysicsAsset->ConstraintSetup.Add(NewPhysicsConstraintTemplate);
                        int32 NewConstraintIndex = PhysicsAsset->ConstraintSetup.Num() - 1;
                        
                        NewPhysicsConstraintTemplate->DefaultInstance.JointName = MeshBoneInfo.Name;
                        // TODO Check
                        // NewPhysicsConstraintTemplate->DefaultInstance.SetAngularSwing1Motion(Params.AngularConstraintMode);
                        // NewPhysicsConstraintTemplate->DefaultInstance.SetAngularSwing2Motion(Params.AngularConstraintMode);
                        // NewPhysicsConstraintTemplate->DefaultInstance.SetAngularTwistMotion(Params.AngularConstraintMode);
                        NewPhysicsConstraintTemplate->DefaultInstance.ConstraintBone1 = MeshBoneInfo.Name;
                        NewPhysicsConstraintTemplate->DefaultInstance.ConstraintBone2 = OtherMeshBoneInfo.Name;
                        // NewPhysicsConstraintTemplate->DefaultInstance.SnapTransformsToDefault(EConstraintTransformComponentFlags::All, PhysicsAsset);
                        // NewPhysicsConstraintTemplate->SetDefaultProfile(NewPhysicsConstraintTemplate->DefaultInstance);
                        NewPhysicsConstraintTemplate->DefaultInstance.ConstraintIndex = NewConstraintIndex;
                        

                        if (NewConstraintIndex != -1)
                        {
                            EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                            EditorEngine->PhysicsAssetEditorWorld->SelectedConstraintIndex = NewConstraintIndex;
                        }
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Delete"))
        {
            bIsDelete = true;

            BodySetup->AggGeom.BoxElems.Empty();
            BodySetup->AggGeom.SphereElems.Empty();
            BodySetup->AggGeom.SphylElems.Empty();

            for (UPhysicsConstraintTemplate* ConstraintSetup : PhysicsAsset->ConstraintSetup)
            {
                GUObjectArray.MarkRemoveObject(ConstraintSetup);
            }

            PhysicsAsset->ConstraintSetup.Empty();
            
            PhysicsAsset->BodySetup.Remove(BodySetup);
            PhysicsAsset->UpdateBodySetupIndexMap();
            EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
            GUObjectArray.MarkRemoveObject(BodySetup);
        }
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::DrawPopupBone(UPhysicsAsset* PhysicsAsset, UBodySetup* BodySetup, int32 InBoneIndex) const
{
    if (ImGui::BeginPopupContextItem("TreeNodeRightClick"))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
        ImGui::SeparatorText("Body");
        ImGui::PopStyleColor();
        if (ImGui::MenuItem("Body Add/Replace (Not Implemented)"))
        {
            // TODO
        }
        if (ImGui::BeginMenu("Add Shape"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, InActiveTextColor);
            ImGui::SeparatorText("Shape Type");
            ImGui::Text("Create BodySetup Automatically");
            ImGui::PopStyleColor();
            ImGui::Image((ImTextureID)BoxPhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            ImGui::SameLine();
            if (ImGui::MenuItem("Add Box"))
            {
                AddShape(PhysicsAsset, BodySetup, InBoneIndex, EAggCollisionShape::Box);
            }
            ImGui::Image((ImTextureID)SpherePhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            ImGui::SameLine();
            if (ImGui::MenuItem("Add Sphere"))
            {
                AddShape(PhysicsAsset, BodySetup, InBoneIndex, EAggCollisionShape::Sphere);
            }
            ImGui::Image((ImTextureID)SphylPhATIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
            ImGui::SameLine();
            if (ImGui::MenuItem("Add Capsule"))
            {
                AddShape(PhysicsAsset, BodySetup, InBoneIndex, EAggCollisionShape::Sphyl);
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::DrawPopupPrimitive(UPhysicsAsset* InPhysicsAsset, UBodySetup* InBodySetup, EAggCollisionShape::Type PrimitiveType, uint32 PrimitiveIndex, bool& bIsDelete)
{
    bIsDelete = false;
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    if (ImGui::BeginPopupContextItem("TreeNodeRightClick"))
    {
        if (ImGui::MenuItem("Delete"))
        {
            bIsDelete = true;
            switch (PrimitiveType)
            {
            case EAggCollisionShape::Sphere:
            {
                InBodySetup->AggGeom.SphereElems.RemoveAt(PrimitiveIndex);
                break;
            }
            case EAggCollisionShape::Box:
            {
                InBodySetup->AggGeom.BoxElems.RemoveAt(PrimitiveIndex);
                break;
            }
            case EAggCollisionShape::Sphyl:
            {
                InBodySetup->AggGeom.SphylElems.RemoveAt(PrimitiveIndex);
                break;
            }
            case EAggCollisionShape::Convex:
            case EAggCollisionShape::TaperedCapsule:
            case EAggCollisionShape::LevelSet:
            case EAggCollisionShape::SkinnedLevelSet:
            case EAggCollisionShape::Unknown:
            default:
                break;
            }

            if (InBodySetup->AggGeom.BoxElems.Num() == 0 && InBodySetup->AggGeom.SphereElems.Num() == 0 && InBodySetup->AggGeom.SphylElems.Num() == 0)
            {
                for (UPhysicsConstraintTemplate* ConstraintSetup : InPhysicsAsset->ConstraintSetup)
                {
                    GUObjectArray.MarkRemoveObject(ConstraintSetup);
                }

                InPhysicsAsset->ConstraintSetup.Empty();
                
                InPhysicsAsset->BodySetup.Remove(InBodySetup);
                InPhysicsAsset->UpdateBodySetupIndexMap();
                EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
                GUObjectArray.MarkRemoveObject(InBodySetup);
            }
        }
        ImGui::EndPopup();
    }
}

void FPhysicsAssetEditorPanel::DrawPopupConstraint(UPhysicsAsset* PhysicsAsset, int32 TargetConstraintIndex, bool& bIsDelete) const
{
    bIsDelete = false;
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }
    
    if (ImGui::BeginPopupContextItem("TreeNodeRightClick"))
    {
        if (ImGui::MenuItem("Delete"))
        {
            bIsDelete = true;
            UPhysicsConstraintTemplate*  TargetPhysicsConstraintTemplate = PhysicsAsset->ConstraintSetup[TargetConstraintIndex];
            PhysicsAsset->ConstraintSetup.RemoveAt(TargetConstraintIndex);
            GUObjectArray.MarkRemoveObject(TargetPhysicsConstraintTemplate);
            EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
            
            for (int32 ConstraintIndex = 0; ConstraintIndex < PhysicsAsset->ConstraintSetup.Num(); ConstraintIndex++)
            {
                UPhysicsConstraintTemplate* Constraint = PhysicsAsset->ConstraintSetup[ConstraintIndex];
                Constraint->DefaultInstance.ConstraintIndex = ConstraintIndex;
            }
        }
        ImGui::EndPopup();
    }
}

FString FPhysicsAssetEditorPanel::GetCleanBoneName(const FMeshBoneInfo& BoneInfo, int32 BoneIndex, uint8 bShowBoneIndices) const
{
    FString InFullName = BoneInfo.Name.ToString();

    FString name = GetCleanBoneName(InFullName);

    if (bShowBoneIndices)
    {
        name += FString::Printf(" <%d>", BoneIndex);
    }
    
    return name;
}

FString FPhysicsAssetEditorPanel::GetCleanBoneName(const FString& InFullName) const
{
    // 1) 계층 구분자 '|' 뒤 이름만 취하기
    int32 barIdx = InFullName.FindChar(TEXT('|'),
        /*case*/ ESearchCase::CaseSensitive,
        /*dir*/  ESearchDir::FromEnd);
    FString name = (barIdx != INDEX_NONE)
        ? InFullName.RightChop(barIdx + 1)
        : InFullName;

    // 2) 네임스페이스 구분자 ':' 뒤 이름만 취하기
    int32 colonIdx = name.FindChar(TEXT(':'),
        /*case*/ ESearchCase::CaseSensitive,
        /*dir*/  ESearchDir::FromEnd);
    if (colonIdx != INDEX_NONE)
    {
        name = name.RightChop(colonIdx + 1);
    }
    
    return name;
}

void FPhysicsAssetEditorPanel::LoadBoneIcon()
{
    BoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Bone_16x.png")->TextureSRV;
    NonWeightBoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/BoneNonWeighted_16x.png")->TextureSRV;
    BodySetupIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Body_16x.png")->TextureSRV;
    BoxIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/box_16px.png")->TextureSRV;
    SphereIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Sphere_16px.png")->TextureSRV;
    SphylIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Sphyl_16x.png")->TextureSRV;
    ConstraintIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Constraint_16x.png")->TextureSRV;

    SpherePhATIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/icon_PhAT_Sphere_40x.png")->TextureSRV;
    BoxPhATIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/icon_PhAT_Box_40x.png")->TextureSRV;
    SphylPhATIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/icon_PhAT_Sphyl_40x.png")->TextureSRV;
}

void FPhysicsAssetEditorPanel::AddShape(UPhysicsAsset* InPhysicsAsset, UBodySetup* TargetBodySetup, int32 BoneIndex, EAggCollisionShape::Type InShapeType) const
{
    if (TargetBodySetup == nullptr && BoneIndex == -1)
    {
        return;
    }
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    if (TargetBodySetup == nullptr)
    {
        FName BoneName = InPhysicsAsset->PreviewSkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetBoneName(BoneIndex);
        TargetBodySetup = FObjectFactory::ConstructObject<UBodySetup>(InPhysicsAsset);
        TargetBodySetup->BoneName = BoneName;

        int32 BodyIndex = InPhysicsAsset->FindBodyIndex(BoneName);

        TargetBodySetup->CollisionResponse = EBodyCollisionResponse::Type::BodyCollision_Enabled;
        TargetBodySetup->DefaultInstance.ExternalCollisionProfileBodySetup = TargetBodySetup;
        TargetBodySetup->DefaultInstance.InstanceBodyIndex = BodyIndex;
        TargetBodySetup->DefaultInstance.InstanceBoneIndex = BoneIndex;

        FPhysicsMaterial* PhysicsMaterial = new FPhysicsMaterial();
        PhysicsMaterial->Density = 1000.f; // 임시 밀도 값

        // !TODO : 디폴트 머티리얼을 하나 만들어두고 그걸 사용하도록 해야 함
        UPhysicalMaterial* PhysMaterial = FObjectFactory::ConstructObject<UPhysicalMaterial>(nullptr);
        PhysMaterial->Material = PhysicsMaterial;

        TargetBodySetup->PhysMaterial = PhysMaterial;
        InPhysicsAsset->BodySetup.Add(TargetBodySetup);
        InPhysicsAsset->UpdateBodySetupIndexMap();
    }

    int32 NewPrimitiveIndex = -1;
    switch (InShapeType)
    {
    case EAggCollisionShape::Sphere:
    {
        FKSphereElem SphereElem = FKSphereElem();
        SphereElem.Name = TargetBodySetup->BoneName.ToString() + "_sphere";
        TargetBodySetup->AggGeom.SphereElems.Add(SphereElem);
        SphereElem.Center = FVector::ZeroVector;
        SphereElem.Radius = DefaultPrimSize;
        NewPrimitiveIndex = TargetBodySetup->AggGeom.SphereElems.Num() - 1;
        break;
    }
    case EAggCollisionShape::Box:
    {
        FKBoxElem BoxElem = FKBoxElem();
        BoxElem.Name = TargetBodySetup->BoneName.ToString() + "_box";
        TargetBodySetup->AggGeom.BoxElems.Add(BoxElem);
        BoxElem.SetTransform( FTransform::Identity );

        BoxElem.X = 0.5f * DefaultPrimSize;
        BoxElem.Y = 0.5f * DefaultPrimSize;
        BoxElem.Z = 0.5f * DefaultPrimSize;
        NewPrimitiveIndex = TargetBodySetup->AggGeom.SphereElems.Num() - 1;
        break;
    }
    case EAggCollisionShape::Sphyl:
    {
        FKSphylElem SphylElem = FKSphylElem();
        SphylElem.Name = TargetBodySetup->BoneName.ToString() + "_capsule";
        TargetBodySetup->AggGeom.SphylElems.Add(SphylElem);

        SphylElem.SetTransform( FTransform::Identity );

        SphylElem.Length = DefaultPrimSize;
        SphylElem.Radius = DefaultPrimSize;
        NewPrimitiveIndex = TargetBodySetup->AggGeom.SphereElems.Num() - 1;
        
        break;
    }
    case EAggCollisionShape::Convex:
    case EAggCollisionShape::TaperedCapsule:
    case EAggCollisionShape::LevelSet:
    case EAggCollisionShape::SkinnedLevelSet:
    case EAggCollisionShape::Unknown:
    default:
        break;
    }

    if (NewPrimitiveIndex != -1)
    {
        EditorEngine->PhysicsAssetEditorWorld->ClearSelected();
        EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType = InShapeType;
        EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex = NewPrimitiveIndex;
        EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex = InPhysicsAsset->FindBodyIndex(TargetBodySetup->BoneName);
    }
}
