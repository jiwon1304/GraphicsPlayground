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
    RenderViewportPanel();
    ImGui::SameLine(0, 0);
    RenderEmitterPanel();
    RenderDetailPanel();
    ImGui::SameLine(0, 0);
    RenderCurveEditorPanel();
}

void ParticleViewerPanel::RenderViewportPanel()
{
    ImVec2 viewportSize = ImVec2(Width * 0.4f, Height * 0.5f);
    ImGui::BeginChild("Viewport", viewportSize, true);

    ImGui::Text("Viewport");
    ImGui::EndChild();
}

void ParticleViewerPanel::RenderEmitterPanel()
{
    ImVec2 viewportSize = ImVec2(Width * 0.6f, Height * 0.5f);
    float BlockWidth = ItemWidth + WidthPad;
    float BlockHeight = viewportSize.y - HeightPad;
    float headerHeight = ImGui::GetTextLineHeightWithSpacing();

    ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("Emitter", viewportSize, true, flags);
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

    ImGui::EndChild();
}

void ParticleViewerPanel::RenderDetailPanel()
{
    ImVec2 viewportSize = ImVec2(Width * 0.4f, Height * 0.5f);
    ImGui::BeginChild("Detail", viewportSize, true);

    ImGui::Text("Detail");
    ImGui::EndChild();
}

void ParticleViewerPanel::RenderCurveEditorPanel()
{
    ImVec2 viewportSize = ImVec2(Width * 0.6f, Height * 0.5f);
    ImGui::BeginChild("CurveEditor", viewportSize, true);

    ImGui::Text("CurveEditor");
    ImGui::EndChild();
}


void ParticleViewerPanel::RenderEmitterModulePopup(int EmitterIndex)
{
    if (ImGui::MenuItem("이미터 이름변경")) {
        // TODO: 이름 변경 로직
    }
    if (ImGui::MenuItem("이미터 복제")) {
        if (EmitterIndex != -1) {
            EmitterList.Add(EmitterList[EmitterIndex]);
        }
    }
    if (ImGui::MenuItem("이미터 삭제")) {
        if (EmitterIndex != -1) {
            EmitterList.RemoveAt(EmitterIndex);
            if (SelectedEmitterIndex == EmitterIndex)
                SelectedEmitterIndex = -1;
        }
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

