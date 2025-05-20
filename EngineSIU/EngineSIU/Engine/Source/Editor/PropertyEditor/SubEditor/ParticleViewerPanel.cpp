#include "ParticleViewerPanel.h"
#include "Engine/UnrealClient.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/ObjectFactory.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModules/ParticleModuleRequired.h"
#include "Particles/ParticleModules/ParticleModuleSpawn.h"
#include "Particles/ParticleLODLevel.h"
#include "Engine/AssetManager.h"
#include "Components/ParticleSystemComponent.h"
#include "Particles/ParticleModules/ParticleModuleLifetime.h"
#include "Particles/ParticleSpriteEmitter.h"

const char* ModuleTypeToString(EModuleType ModuleType)
{
    switch (ModuleType) {
    case EPMT_General:    return "General";
    case EPMT_TypeData:   return "TypeData";
    case EPMT_Beam:       return "Beam";
    case EPMT_Trail:      return "Trail";
    case EPMT_Spawn:      return "Spawn";
    case EPMT_Required:   return "Required";
    case EPMT_Event:      return "Event";
    case EPMT_Light:      return "Light";
    case EPMT_SubUV:      return "SubUV";
    default:              return "Unknown";
    }
}

void ParticleViewerPanel::PrepareRender(FEditorViewportClient* ViewportClient)
{
    const EViewModeIndex ViewMode = ViewportClient->GetViewMode();
    FViewportResource* ViewportResource = ViewportClient->GetViewportResource();
    RenderTargetRHI = ViewportResource->GetRenderTarget(EResourceType::ERT_Scene);
    DepthStencilRHI = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);
}

void ParticleViewerPanel::Render()
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(Width, Height));

    ImGuiWindowFlags mainCanvasFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

    if (ImGui::Begin("TestWindow", nullptr, mainCanvasFlags))
    {
        RenderPanelLayout();
        ImGui::End();
    }
}

void ParticleViewerPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    if (hWnd && GetClientRect(hWnd, &clientRect))
    {
        Width = static_cast<float>(clientRect.right - clientRect.left);
        Height = static_cast<float>(clientRect.bottom - clientRect.top);
    }
}

void ParticleViewerPanel::RenderPanelLayout()
{
    RenderFilePanel();

    // 전체 남은 영역 크기 구하기
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float leftW = avail.x * 0.5f;
    float rightW = avail.x - leftW;
    float halfH = avail.y * 0.5f;

    // 왼쪽 절반 Viewport
    ImGui::BeginChild("LeftViewport", ImVec2(leftW, avail.y), false);
    RenderViewportPanel();
    ImGui::EndChild();

    ImGui::SameLine(0, 0);

    // 오른쪽 그리기
    ImGui::BeginGroup();

    //오른쪽 상단 Emitter
    ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("RightTopEmitter", ImVec2(rightW, halfH), true, flags);
    RenderEmitterPanel();
    ImGui::EndChild();

    //오른쪽 하단 Detail
    ImGui::BeginChild("RightBottomDetail", ImVec2(rightW, avail.y - halfH), true);
    RenderDetailPanel();
    ImGui::EndChild();

    ImGui::EndGroup();
}

void ParticleViewerPanel::RenderFilePanel()
{

    RenderParticleSystemList();

    ImGui::SameLine();

    if (ImGui::Button("새 파티클 생성하기")) {
        ImGui::OpenPopup("Create New Particle System");
    }

    RenderCreateParticlePopup();
}

void ParticleViewerPanel::RenderViewportPanel()
{
    ImGui::Text("Viewport");
}

void ParticleViewerPanel::RenderEmitterPanel()
{
    if (ParticleSystem == nullptr)
        return;
    ImVec2 viewportSize = ImVec2(Width * 0.6f, Height * 0.5f);
    float BlockWidth = ItemWidth + WidthPad;
    float BlockHeight = viewportSize.y - HeightPad;
    float headerHeight = ImGui::GetTextLineHeightWithSpacing();

    InputEmitterPanel(); // Delete 키 입력 처리

    ImGui::Text("Emitter Editor");

    // Emitter 목록 출력
    for (int i = 0; i < ParticleSystem->Emitters.Num(); ++i) {
        UParticleEmitter* Emitter = ParticleSystem->Emitters[i];
        ImGui::PushID(i);

        ImVec2 blockMin = ImGui::GetCursorScreenPos();
        ImVec2 blockMax = ImVec2(blockMin.x + BlockWidth, blockMin.y + BlockHeight);

        // 블록 전체를 감싸는 InvisibleButton: 헤더/모듈 위에서도 클릭 감지
        ImGui::PushStyleColor(ImGuiCol_ChildBg, EmitterBGColor);
        ImGui::BeginChild("EmitterBlock", ImVec2(ItemWidth, BlockHeight), false);
        ImGui::PopStyleColor();

        // 클릭 감지: 마우스가 이 Rect 안에 있고, 클릭됐으면 선택
        if (ImGui::IsMouseHoveringRect(blockMin, blockMax) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            SelectedEmitterIndex = i;
            SelectedModuleIndex = -1; // Emitter 클릭 시 Module 선택 해제
        }

        // 2) 우클릭 팝업 트리거
        if (ImGui::IsMouseHoveringRect(blockMin, blockMax) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("EmitterContextMenu");
        }

        // 3) 팝업 렌더링
        if (ImGui::BeginPopup("EmitterContextMenu"))
        {
            RenderEmitterModulePopup(i);
            ImGui::EndPopup();
        }

        // 헤더 영역에만 색을 그려주기
        ImDrawList* dl = ImGui::GetWindowDrawList();
        // 헤더 배경 채우기
        dl->AddRectFilled(
            blockMin,
            ImVec2(blockMin.x + ItemWidth, blockMin.y + headerHeight),
            ImGui::GetColorU32((SelectedEmitterIndex == i) ? SelectedEmitterTextColor : EmitterTextColor)
        );

        // 헤더 텍스트
        ImGui::SetCursorScreenPos(blockMin);
        ImGui::Text("%s",
            Emitter->EmitterName.ToString().ToAnsiString().c_str());

        ImGui::Spacing();
        ImGui::Separator();

        int ModuleIndex = 0;
        for (auto& Module : Emitter->LODLevels[0]->Modules) {
            ImVec2 moduleMin = ImGui::GetCursorScreenPos();
            ImVec2 moduleMax = ImVec2(moduleMin.x + ItemWidth, moduleMin.y + headerHeight);

            // 클릭 처리
            if (ImGui::IsMouseHoveringRect(moduleMin, moduleMax) &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                SelectedEmitterIndex = i;
                SelectedModuleIndex = ModuleIndex;
            }

            // 색상 처리
            bool isModuleSelected = (SelectedEmitterIndex == i) && (SelectedModuleIndex == ModuleIndex);
            dl->AddRectFilled(
                moduleMin,
                moduleMax,
                ImGui::GetColorU32(isModuleSelected ? SelectedEmitterTextColor : EmitterTextColor)
            );

            ImGui::SetCursorScreenPos(moduleMin);
            ImGui::Text("%s", ModuleTypeToString(Module->GetModuleType()));

            ImGui::Spacing();
            ModuleIndex++;
        }


        ImGui::EndChild();
        ImGui::PopID();

        // 다음 아이템을 같은 줄에 붙이기
        if (i + 1 < ParticleSystem->Emitters.Num())
            ImGui::SameLine(0, WidthPad);
    }
}

void ParticleViewerPanel::RenderDetailPanel()
{
    ImGui::Text("Detail");

    if (ParticleSystem == nullptr||SelectedEmitterIndex==-1||SelectedModuleIndex==-1||CurrentParticleSystemIndex==-1)
        return;
    UParticleModule* Module = ParticleSystem->Emitters[SelectedEmitterIndex]->LODLevels[0]->Modules[SelectedModuleIndex];
    const UClass* Class = Module->GetClass();

    for (; Class; Class = Class->GetSuperClass())
    {
        const TArray<FProperty*>& Properties = Class->GetProperties();
        if (!Properties.IsEmpty())
        {
            ImGui::SeparatorText(*Class->GetName());
        }

        for (const FProperty* Prop : Properties)
        {
            Prop->DisplayInImGui(Module);
        }
    }
}

void ParticleViewerPanel::RenderCurveEditorPanel()
{
    ImGui::Text("CurveEditor");
}


void ParticleViewerPanel::RenderEmitterModulePopup(int EmitterIndex)
{
    if (ImGui::MenuItem("이미터")) {
    }
    if (ImGui::MenuItem("파티클 시스템")) {
    }
    if (ImGui::MenuItem("타입 데이터")) {
    }
    if (ImGui::MenuItem("가속")) {
    }
    if (ImGui::MenuItem("컬러")) {
    }
    if (ImGui::MenuItem("킬")) {
    }
    if (ImGui::MenuItem("수명")) {
    }
    if (ImGui::MenuItem("회전")) {
    }
    if (ImGui::MenuItem("스폰")) {
        UParticleEmitter* Emitter = ParticleSystem->Emitters[EmitterIndex];
        UParticleModuleSpawn* SpawnModule = FObjectFactory::ConstructObject<UParticleModuleSpawn>(Emitter->LODLevels[0]);
        Emitter->LODLevels[0]->Modules.Add(SpawnModule);
        ParticleSystem->PostEditChangeProperty();
    }
    if (ImGui::MenuItem("속도")) {
    }
}

void ParticleViewerPanel::RenderEmitterCreatePopup()
{
    if (ImGui::MenuItem("새 파티클 스프라이트 이미터")) {
        UParticleEmitter* NewEmitter = CreateDefaultParticleEmitter();
        ParticleSystem->Emitters.Add(NewEmitter);
    }
}

void ParticleViewerPanel::InputEmitterPanel()
{
    //Delete 키로 Emitter 삭제
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        if (SelectedEmitterIndex >= 0 && SelectedEmitterIndex < ParticleSystem->Emitters.Num())
        {
            UParticleEmitter* SelectedEmitter = ParticleSystem->Emitters[SelectedEmitterIndex];

            // Module이 선택된 경우 Module 삭제
            if (SelectedModuleIndex >= 0 && SelectedModuleIndex < SelectedEmitter->LODLevels[0]->Modules.Num()) {
                SelectedEmitter->LODLevels[0]->Modules.RemoveAt(SelectedModuleIndex);

                // 선택 해제
                SelectedModuleIndex = -1;
            }
            else {
                // Module이 선택되지 않았으면 Emitter 삭제
                ParticleSystem->Emitters.RemoveAt(SelectedEmitterIndex);
                ParticleSystem->PostEditChangeProperty();
                // 선택 해제
                SelectedEmitterIndex = -1;
                SelectedModuleIndex = -1;
            }
        }
    }

    // 빈 공간 클릭 시 선택 해제 및 팝업 오픈
    bool windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    bool anyItemHovered = ImGui::IsAnyItemHovered();
    bool clickL = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    bool clickR = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

    if (windowHovered && !anyItemHovered)
    {
        if (clickL)
        {
            SelectedEmitterIndex = -1;          // 빈 공간 좌클릭 → 선택 해제
            SelectedModuleIndex = -1;          // Module 선택 해제
        }

        if (clickR)
            ImGui::OpenPopup("EmitterEmptyContextMenu"); // 빈 공간 우클릭 → 팝업 오픈
    }

    if (ImGui::BeginPopup("EmitterEmptyContextMenu"))
    {
        RenderEmitterCreatePopup();
        ImGui::EndPopup();
    }
}

void ParticleViewerPanel::CreateNewParticleSystem(const FString& Name)
{
    auto& ParticleSystemMap = UAssetManager::Get().GetParticleSystemMap();

    ParticleSystem = FObjectFactory::ConstructObject<UParticleSystem>(ParticleSystemComponent);
    UParticleEmitter* NewEmitter = CreateDefaultParticleEmitter();
    ParticleSystem->Emitters.Add(NewEmitter);   
    ParticleSystem->PostEditChangeProperty();
    ParticleSystemMap.Add(FName(*Name), ParticleSystem);

    // ParticleNames 목록을 새로 업데이트
    ParticleNames.Empty();
    int Index = 0;
    for (auto& Pair : ParticleSystemMap) {
        ParticleNames.Add(Pair.Key);
        if (Pair.Key == FName(*Name)) {
            CurrentParticleSystemIndex = Index;
        }
        ++Index;
    }
}

void ParticleViewerPanel::RenderParticleSystemList()
{
    auto& ParticleSystemMap = UAssetManager::Get().GetParticleSystemMap();


    if (ParticleSystemMap.Num() == 0) {
        ImGui::Text("No Particle Systems Found.");
        ParticleSystem = nullptr; // 비어있으면 초기화
        CurrentParticleSystemIndex = -1;
        return;
    }

    // ParticleSystemMap의 이름 목록을 가져오기
    ParticleNames.Empty();
    for (auto& Pair : ParticleSystemMap) {
        ParticleNames.Add(Pair.Key);
    }

    // 이름 배열로 ComboBox 표시
    if (ImGui::BeginCombo("Particle Systems",
        CurrentParticleSystemIndex >=0 && CurrentParticleSystemIndex < ParticleNames.Num() ? 
        ParticleNames[CurrentParticleSystemIndex].ToString().ToAnsiString().c_str() : "Select Particle System"))
    {
        for (int i = 0; i < ParticleNames.Num(); ++i) {
            const bool isSelected = (CurrentParticleSystemIndex == i);
            if (ImGui::Selectable(ParticleNames[i].ToString().ToAnsiString().c_str(), isSelected)) {
                CurrentParticleSystemIndex = i;
                // 선택된 파티클 시스템을 가져오기
                ParticleSystem = ParticleSystemMap[ParticleNames[i]];
                ParticleSystem->PostEditChangeProperty();
                ParticleSystemComponent->SetTemplate(ParticleSystem);
                SelectedEmitterIndex = -1; // Emitter 선택 해제
                SelectedModuleIndex = -1; // Module 선택 해제
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Remove 버튼
    if (ImGui::Button("현재 파티클 삭제")) {
        if (CurrentParticleSystemIndex < ParticleNames.Num()) {
            FName SelectedName = ParticleNames[CurrentParticleSystemIndex];
            ParticleSystemMap.Remove(SelectedName);
            ParticleSystem = nullptr; // 선택된 파티클 시스템 초기화
            CurrentParticleSystemIndex = -1;  // 인덱스 초기화
            SelectedEmitterIndex = -1; // Emitter 선택 해제
            SelectedModuleIndex = -1; // Module 선택 해제
        }
    }
}

void ParticleViewerPanel::RenderCreateParticlePopup()
{
    static char NewParticleName[128] = "";

    auto& ParticleSystemMap = UAssetManager::Get().GetParticleSystemMap();
    // 팝업 열기
    if (ImGui::BeginPopupModal("Create New Particle System", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("파티클 시스템의 이름을 입력하세요:");
        ImGui::InputText("##NewParticleName", NewParticleName, IM_ARRAYSIZE(NewParticleName));

        // 생성 버튼
        if (ImGui::Button("생성하기", ImVec2(120, 0)))
        {
            FString ParticleNameStr(NewParticleName);
            if (!ParticleNameStr.IsEmpty() && !ParticleSystemMap.Contains(FName(*ParticleNameStr)))
            {
                CreateNewParticleSystem(ParticleNameStr);
                ParticleSystemComponent->SetTemplate(ParticleSystem);
                ImGui::CloseCurrentPopup();
            }
            else
            {
                ImGui::Text("유효하지 않거나 중복된 이름입니다.");
            }
        }

        ImGui::SameLine();

        // 취소 버튼
        if (ImGui::Button("취소", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ParticleViewerPanel::RemoveParticleSystem(const FName& AssetName)
{
}

UParticleEmitter* ParticleViewerPanel::CreateDefaultParticleEmitter()
{
    UParticleSpriteEmitter* NewEmitter = FObjectFactory::ConstructObject<UParticleSpriteEmitter>(ParticleSystem);
    NewEmitter->EmitterName = FName("Particle Emitter");

    UParticleLODLevel* LODLevel = FObjectFactory::ConstructObject<UParticleLODLevel>(NewEmitter);
    LODLevel->Initialize();
    NewEmitter->LODLevels.Add(LODLevel);
    NewEmitter->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleRequired>(NewEmitter));
    NewEmitter->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSpawn>(NewEmitter));
    NewEmitter->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLifetime>(NewEmitter));
    LODLevel->UpdateModuleLists();
    return NewEmitter;
}

void ParticleViewerPanel::SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent)
{
    ParticleSystemComponent = InParticleSystemComponent;
}



