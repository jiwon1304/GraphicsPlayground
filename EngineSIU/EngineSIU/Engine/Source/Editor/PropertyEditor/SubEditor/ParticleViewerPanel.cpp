#include "ParticleViewerPanel.h"
#include "Engine/UnrealClient.h"
#include "UnrealEd/EditorViewportClient.h"

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

void ParticleViewerPanel::RenderViewportPanel()
{
    ImGui::Text("Viewport");
}

void ParticleViewerPanel::RenderEmitterPanel()
{
    ImVec2 viewportSize = ImVec2(Width * 0.6f, Height * 0.5f);
    float BlockWidth = ItemWidth + WidthPad;
    float BlockHeight = viewportSize.y - HeightPad;
    float headerHeight = ImGui::GetTextLineHeightWithSpacing();

    InputEmitterPanel(); // Delete 키 입력 처리

    ImGui::Text("Emitter Editor");

    // Emitter 목록 출력
    for (int i = 0; i < EmitterList.Num(); ++i) {
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
        ImGui::Text("%s", EmitterList[i].Name.ToAnsiString().c_str());

        ImGui::Spacing();
        ImGui::Separator();

        for (auto& Module : EmitterList[i].Modules) {
            ImGui::Checkbox(Module.Name.ToAnsiString().c_str(), &Module.Enabled);
        }


        ImGui::EndChild();
        ImGui::PopID();

        // 다음 아이템을 같은 줄에 붙이기
        if (i + 1 < EmitterList.Num())
            ImGui::SameLine(0, WidthPad);
    }
}

void ParticleViewerPanel::RenderDetailPanel()
{
    ImGui::Text("Detail");
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
    }
    if (ImGui::MenuItem("속도")) {
    }
}

void ParticleViewerPanel::RenderEmitterCreatePopup()
{
    if (ImGui::MenuItem("새 파티클 스프라이트 이미터")) {
        EmitterList.Add({
            "Particle Emitter",
            {
                {"가속", true, ImVec4(0.8f, 0.3f, 0.3f, 1.0f)},
                {"수명", true, ImVec4(0.3f, 0.8f, 0.3f, 1.0f)},
                {"속도", true, ImVec4(0.3f, 0.3f, 0.8f, 1.0f)}
            }
            });
    }
}

void ParticleViewerPanel::InputEmitterPanel()
{
    //Delete 키로 Emitter 삭제
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        if (SelectedEmitterIndex >= 0 && SelectedEmitterIndex < EmitterList.Num())
        {
            EmitterList.RemoveAt(SelectedEmitterIndex);
            SelectedEmitterIndex = -1;
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
            SelectedEmitterIndex = -1;          // 빈 공간 좌클릭 → 선택 해제

        if (clickR)
            ImGui::OpenPopup("EmitterEmptyContextMenu"); // 빈 공간 우클릭 → 팝업 오픈
    }

    if (ImGui::BeginPopup("EmitterEmptyContextMenu"))
    {
        RenderEmitterCreatePopup();
        ImGui::EndPopup();
    }
}

