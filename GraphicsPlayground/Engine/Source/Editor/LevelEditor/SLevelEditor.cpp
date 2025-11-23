#include "SLevelEditor.h"
#include <fstream>
#include <ostream>
#include <sstream>
#include "Launch/EngineLoop.h"
#include "Engine/UnrealClient.h"
#include "Windows/WindowsCursor.h"
#include "InteractiveToolsFramework/BaseGizmos/GizmoBaseComponent.h"
#include "Classes/Engine/EditorEngine.h"
#include "Slate/Widgets/Layout/SSplitter.h"
#include "SlateCore/Widgets/SWindow.h"
#include "Editor/UnrealEd/EditorViewportClient.h"
#include "ApplicationCore/Generic/GenericSlateAppMessageHandler.h"
#include "ApplicationCore/Generic/GenericWindow.h"


extern FEngineLoop GEngineLoop;


SLevelEditor::SLevelEditor()
    : RootSplitter(nullptr)
    , LeftSplitter(nullptr)
    , RightSplitter(nullptr)
    , bMultiViewportMode(false)
{
}

void SLevelEditor::Initialize(uint32 InEditorWidth, uint32 InEditorHeight)
{
    FIntRect InRect(0, 0, InEditorWidth, InEditorHeight);

    SingleWindow = new SWindow(InRect);
    RootSplitter = new SSplitterH(InRect);
    LeftSplitter = new SSplitterV(FIntRect());
    RightSplitter = new SSplitterV(FIntRect());

    MultiViewportsCached[0] = LeftSplitter->SideLT;
    MultiViewportsCached[1] = RightSplitter->SideLT;
    MultiViewportsCached[2] = LeftSplitter->SideRB;
    MultiViewportsCached[3] = RightSplitter->SideRB;

    RootSplitter->SideLT = LeftSplitter;
    RootSplitter->SideRB = RightSplitter;

    RootSplitter->UpdateChilds();
    LeftSplitter->UpdateChilds();
    RightSplitter->UpdateChilds();


    for (size_t i = 0; i < 4; i++)
    {
        FIntRect ViewportRect = MultiViewportsCached[i]->GetRect();

        constexpr FVector InitialPosition = FVector(10.f, 10.f, 10.f);
        FRotator InitialRotation = FRotator::MakeLookAtRotation(
            InitialPosition,
            FVector::ZeroVector
        );
        FViewportCamera* ViewportCamera = new FViewportPerspectiveCamera(
            InitialPosition,
            InitialRotation,
            static_cast<float>(ViewportRect.GetWidth()) / ViewportRect.GetHeight()
        );

        ViewportClients[i] = std::make_shared<FEditorViewportClient>();

        ViewportClients[i]->Initialize(ViewportRect, ViewportCamera);
    }

    ActiveViewportClient = ViewportClients[0];
    if (!bMultiViewportMode)
    {
        FIntRect SingleViewportRect = SingleWindow->GetRect();

        ActiveViewportClient->Resize(SingleViewportRect);
    }
    
    LoadConfig();

    FGenericSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();

    Handler->OnPIEModeStartDelegate.AddLambda([this]()
        {
            this->RegisterPIEInputDelegates();
        });

    Handler->OnPIEModeEndDelegate.AddLambda([this]()
        {
            this->RegisterEditorInputDelegates();
        });

    // Register Editor input when first initialization. 
    RegisterEditorInputDelegates();
}

void SLevelEditor::Tick(float DeltaTime)
{
    for (std::shared_ptr<FEditorViewportClient> Viewport : ViewportClients)
    {
        Viewport->Tick(DeltaTime);
    }
}

void SLevelEditor::Release()
{
    delete SingleWindow;
    delete RootSplitter;
    delete LeftSplitter;
    delete RightSplitter;
}

void SLevelEditor::ResizeEditor(uint32 InEditorWidth, uint32 InEditorHeight)
{
    if (InEditorWidth == EditorWidth && InEditorHeight == EditorHeight)
    {
        return;
    }

    SingleWindow->Resize(FIntRect(0, 0, InEditorWidth, InEditorHeight));
    RootSplitter->Resize(FIntRect(0, 0, InEditorWidth, InEditorHeight));

    RootSplitter->UpdateChilds();
    LeftSplitter->UpdateChilds();
    RightSplitter->UpdateChilds();

    ResizeViewports();
}

void SLevelEditor::SelectViewport(const FIntPoint& Point)
{
    for (int i = 0; i < 4; i++)
    {
        if (ViewportClients[i]->GetViewportRect().Contains(Point))
        {
            SetActiveViewportClient(i);
            return;
        }
    }
}

void SLevelEditor::ResizeViewports()
{
    if (bMultiViewportMode)
    {
        for (int i = 0; i < 4; ++i)
        {
            assert(ViewportClients[i]);
            ViewportClients[i]->Resize(MultiViewportsCached[i]->GetRect());
        }
    }
    else
    {
        ActiveViewportClient->Resize(SingleWindow->GetRect());
    }
}

void SLevelEditor::SetEnableMultiViewport(bool bIsEnable)
{
    bMultiViewportMode = bIsEnable;
    ResizeViewports();
}

bool SLevelEditor::IsMultiViewport() const
{
    return bMultiViewportMode;
}

void SLevelEditor::LoadConfig()
{
    // auto Config = ReadIniFile(IniFilePath);

    // int32 WindowX = FMath::Max(GetValueFromConfig(Config, "WindowX", 0), 0);
    // int32 WindowY = FMath::Max(GetValueFromConfig(Config, "WindowY", 0), 0);
    // int32 WindowWidth = GetValueFromConfig(Config, "WindowWidth", EditorWidth);
    // int32 WindowHeight = GetValueFromConfig(Config, "WindowHeight", EditorHeight);
    // if (WindowWidth > 100 && WindowHeight > 100)
    // {
    //     MoveWindow(GEngineLoop.AppWnd, WindowX, WindowY, WindowWidth, WindowHeight, true);
    // }
    // bool Zoomed = GetValueFromConfig(Config, "Zoomed", false);
    // if (Zoomed)
    // {
    //     ShowWindow(GEngineLoop.AppWnd, SW_MAXIMIZE);
    // }
    
    // FEditorViewportClient::Pivot.X = GetValueFromConfig(Config, "OrthoPivotX", 0.0f);
    // FEditorViewportClient::Pivot.Y = GetValueFromConfig(Config, "OrthoPivotY", 0.0f);
    // FEditorViewportClient::Pivot.Z = GetValueFromConfig(Config, "OrthoPivotZ", 0.0f);
    // FEditorViewportClient::OrthoSize = GetValueFromConfig(Config, "OrthoZoomSize", 10.0f);

    // SetActiveViewportClient(GetValueFromConfig(Config, "ActiveViewportIndex", 0));
    // bMultiViewportMode = GetValueFromConfig(Config, "bMultiView", false);
    // if (bMultiViewportMode)
    // {
    //     SetEnableMultiViewport(true);
    // }
    // else
    // {
    //     SetEnableMultiViewport(false);
    // }
    
    // for (size_t i = 0; i < 4; i++)
    // {
    //     ViewportClients[i]->LoadConfig(Config);
    // }
    
    // if (HSplitter)
    // {
    //     HSplitter->LoadConfig(Config);
    // }
    // if (VSplitter)
    // {
    //     VSplitter->LoadConfig(Config);
    // }

    // ResizeViewports();
}

void SLevelEditor::SaveConfig()
{
    // TMap<FString, FString> config;
    // if (HSplitter)
    // {
    //     HSplitter->SaveConfig(config);
    // }
    // if (VSplitter)
    // {
    //     VSplitter->SaveConfig(config);
    // }
    // for (size_t i = 0; i < 4; i++)
    // {
    //     ViewportClients[i]->SaveConfig(config);
    // }
    // ActiveViewportClient->SaveConfig(config);

    // RECT WndRect = {};
    // GetWindowRect(GEngineLoop.AppWnd, &WndRect);
    // config["WindowX"] = std::to_string(WndRect.left);
    // config["WindowY"] = std::to_string(WndRect.top);
    // config["WindowWidth"] = std::to_string(WndRect.right - WndRect.left);
    // config["WindowHeight"] = std::to_string(WndRect.bottom - WndRect.top);
    // config["Zoomed"] = std::to_string(IsZoomed(GEngineLoop.AppWnd));
    
    // config["bMultiView"] = std::to_string(bMultiViewportMode);
    // config["ActiveViewportIndex"] = std::to_string(ActiveViewportClient->ViewportIndex);
    // config["ScreenWidth"] = std::to_string(EditorWidth);
    // config["ScreenHeight"] = std::to_string(EditorHeight);
    // config["OrthoPivotX"] = std::to_string(ActiveViewportClient->Pivot.X);
    // config["OrthoPivotY"] = std::to_string(ActiveViewportClient->Pivot.Y);
    // config["OrthoPivotZ"] = std::to_string(ActiveViewportClient->Pivot.Z);
    // config["OrthoZoomSize"] = std::to_string(ActiveViewportClient->OrthoSize);
    // WriteIniFile(IniFilePath, config);
}

TMap<FString, FString> SLevelEditor::ReadIniFile(const FString& FilePath)
{
    TMap<FString, FString> config;
    std::ifstream file(FilePath.ToUTF8String());
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '[' || line[0] == ';')
        {
            continue;
        }
        std::istringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value))
        {
            config[key] = value;
        }
    }
    return config;
}

void SLevelEditor::WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config)
{
    std::ofstream file(FilePath.ToUTF8String());
    for (const auto& pair : Config)
    {
        file << pair.Key.ToUTF8String() << "=" << pair.Value.ToUTF8String() << "\n";
    }
}

void SLevelEditor::RegisterEditorInputDelegates() 
{
    FGenericSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();
    
    // Clear current delegate functions
    for (const FDelegateHandle& Handle : InputDelegatesHandles)
    {
        Handler->OnKeyCharDelegate.Remove(Handle);
        Handler->OnKeyDownDelegate.Remove(Handle);
        Handler->OnKeyUpDelegate.Remove(Handle);
        Handler->OnMouseDownDelegate.Remove(Handle);
        Handler->OnMouseUpDelegate.Remove(Handle);
        Handler->OnMouseDoubleClickDelegate.Remove(Handle);
        Handler->OnMouseWheelDelegate.Remove(Handle);
        Handler->OnMouseMoveDelegate.Remove(Handle);
        Handler->OnRawMouseInputDelegate.Remove(Handle);
        Handler->OnRawKeyboardInputDelegate.Remove(Handle);
    }

    InputDelegatesHandles.Add(Handler->OnMouseDownDelegate.AddLambda([this](const FPointerEvent& InMouseEvent)
        {
            if (ImGui::GetIO().WantCaptureMouse) return;

            switch (InMouseEvent.GetEffectingButton())  // NOLINT(clang-diagnostic-switch-enum)
            {
            case EKeys::LeftMouseButton:
            {
                if (const UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine))
                {
                    if (const AActor* SelectedActor = EdEngine->GetSelectedActor())
                    {
                        USceneComponent* TargetComponent = nullptr;
                        if (USceneComponent* SelectedComponent = EdEngine->GetSelectedComponent())
                        {
                            TargetComponent = SelectedComponent;
                        }
                        else if (AActor* SelectedActor = EdEngine->GetSelectedActor())
                        {
                            TargetComponent = SelectedActor->GetRootComponent();
                        }
                        else
                        {
                            return;
                        }

                        // 초기 Actor와 Cursor의 거리차를 저장
                        FVector RayOrigin, RayDir;
                        ActiveViewportClient->DeprojectScreenToWorld(FWindowsCursor::GetClientPosition(), RayOrigin, RayDir);

                        const FVector TargetLocation = TargetComponent->GetComponentLocation();
                        const float TargetDist = FVector::Distance(ActiveViewportClient->GetViewportCamera()->ViewLocation, TargetLocation);
                        const FVector TargetRayEnd = RayOrigin + RayDir * TargetDist;
                        TargetDiff = TargetLocation - TargetRayEnd;
                    }
                }
                break;
            }
            case EKeys::RightMouseButton:
            {
                if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
                {
                    FWindowsCursor::SetShowMouseCursor(false);
                    MousePinPosition = InMouseEvent.GetScreenSpacePosition();
                }
                break;
            }
            default:
                break;
            }

            // 마우스 이벤트가 일어난 위치의 뷰포트를 선택
            //if (bMultiViewportMode)
            //{
            //    // POINT Point;
            //    // GetCursorPos(&Point);
            //    // ScreenToClient(GEngineLoop.AppWnd, &Point);
            //    // FVector2D ClientPos = FVector2D{ static_cast<float>(Point.x), static_cast<float>(Point.y) };
            //    FIntPoint ClientPos = InMouseEvent.GetScreenSpacePosition();
            //    SelectViewport(ClientPos);
            //    VSplitter->OnPress({ ClientPos.X, ClientPos.Y });
            //    HSplitter->OnPress({ ClientPos.X, ClientPos.Y });
            //}
        }));

    InputDelegatesHandles.Add(Handler->OnMouseMoveDelegate.AddLambda([this](const FPointerEvent& InMouseEvent)
        {
            if (ImGui::GetIO().WantCaptureMouse) return;

            // Splitter 움직임 로직
            if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
            {
                const auto& [DeltaX, DeltaY] = InMouseEvent.GetCursorDelta();

                bool bSplitterDragging = false;
                //if (VSplitter->IsSplitterPressed())
                //{
                //    VSplitter->OnDrag(FPoint(DeltaX, DeltaY));
                //    bSplitterDragging = true;
                //}
                //if (HSplitter->IsSplitterPressed())
                //{
                //    HSplitter->OnDrag(FPoint(DeltaX, DeltaY));
                //    bSplitterDragging = true;
                //}

                if (bSplitterDragging)
                {
                    ResizeViewports();
                }
            }

            // 멀티 뷰포트일 때, 커서 변경 로직
            if (
                bMultiViewportMode
                && !InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)
                && !InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)
                )
            {
                // TODO: 나중에 커서가 Viewport 위에 있을때만 ECursorType::Crosshair로 바꾸게끔 하기
                // ECursorType CursorType = ECursorType::Crosshair;
                ECursorType CursorType = ECursorType::Arrow;
                
                // POINT Point;
                // GetCursorPos(&Point);
                // ScreenToClient(GEngineLoop.AppWnd, &Point);
                FIntPoint MousePos = InMouseEvent.GetScreenSpacePosition();
                
                const bool bIsVerticalHovered = RootSplitter->IsInMargin(MousePos);
                const bool bIsHorizontalHovered = RightSplitter->IsInMargin(MousePos) || LeftSplitter->IsInMargin(MousePos);

                if (bIsHorizontalHovered && bIsVerticalHovered)
                {
                    CursorType = ECursorType::ResizeAll;
                }
                else if (bIsHorizontalHovered)
                {
                    CursorType = ECursorType::ResizeLeftRight;
                }
                else if (bIsVerticalHovered)
                {
                    CursorType = ECursorType::ResizeUpDown;
                }
                FWindowsCursor::SetMouseCursor(CursorType);
            }
        }));

    InputDelegatesHandles.Add(Handler->OnMouseUpDelegate.AddLambda([this](const FPointerEvent& InMouseEvent)
        {
            switch (InMouseEvent.GetEffectingButton())  // NOLINT(clang-diagnostic-switch-enum)
            {
            case EKeys::RightMouseButton:
            {
                FWindowsCursor::SetShowMouseCursor(true);
                FWindowsCursor::SetPosition(
                    MousePinPosition.X,
                    MousePinPosition.Y
                );
                return;
            }

            // Viewport 선택 로직
            case EKeys::LeftMouseButton:
            {
                //VSplitter->OnRelease();
                //HSplitter->OnRelease();
                return;
            }

            default:
                return;
            }
        }));

    InputDelegatesHandles.Add(Handler->OnRawMouseInputDelegate.AddLambda([this](const FPointerEvent& InMouseEvent)
        {
			// if(GEngineLoop.GetAppMessageHandler()->IsWindowFocused(GEngineLoop.AppWnd)) return;
            if (GEngineLoop.MainWindow->IsActive())
            {
                return;
            }
            // Mouse Move 이벤트 일때만 실행
            if (
                InMouseEvent.GetInputEvent() == IE_Axis
                && InMouseEvent.GetEffectingButton() == EKeys::Invalid
                )
            {
                // 에디터 카메라 이동 로직
                if (
                    !InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)
                    && InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)
                    )
                {
                    ActiveViewportClient->MouseMove(InMouseEvent);
                }

                else if (
                    !InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)
                    && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)
                    )
                {
                    // Gizmo control
                    if (const UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine))
                    {
                        if (EdEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
                        {
                            return;
                        }
                        const UGizmoBaseComponent* Gizmo = Cast<UGizmoBaseComponent>(ActiveViewportClient->GetPickedGizmoComponent());
                        if (!Gizmo)
                        {
                            return;
                        }

                        USceneComponent* TargetComponent = EdEngine->GetSelectedComponent();
                        if (!TargetComponent)
                        {
                            if (AActor* SelectedActor = EdEngine->GetSelectedActor())
                            {
                                TargetComponent = SelectedActor->GetRootComponent();
                            }
                            else
                            {
                                return;
                            }
                        }

                        FVector RayOrigin, RayDir;
                        ActiveViewportClient->DeprojectScreenToWorld(FWindowsCursor::GetClientPosition(), RayOrigin, RayDir);

                        const float TargetDist = FVector::Distance(ActiveViewportClient->GetViewportCamera()->ViewLocation, TargetComponent->GetComponentLocation());
                        const FVector TargetRayEnd = RayOrigin + RayDir * TargetDist;
                        const FVector Result = TargetRayEnd + TargetDiff;

                        FVector NewLocation = TargetComponent->GetComponentLocation();
                        if (EdEngine->GetEditorPlayer()->GetCoordMode() == CDM_WORLD)
                        {
                            // 월드 좌표계에서 카메라 방향을 고려한 이동
                            if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowX)
                            {
                                // 카메라의 오른쪽 방향을 X축 이동에 사용
                                NewLocation.X = Result.X;
                            }
                            else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowY)
                            {
                                // 카메라의 오른쪽 방향을 Y축 이동에 사용
                                NewLocation.Y = Result.Y;
                            }
                            else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
                            {
                                // 카메라의 위쪽 방향을 Z축 이동에 사용
                                NewLocation.Z = Result.Z;
                            }
                        }
                        else
                        {
                            // Result에서 현재 액터 위치를 빼서 이동 벡터를 구함
                            const FVector Delta = Result - TargetComponent->GetComponentLocation();
                            // 각 축에 대해 Local 방향 벡터에 투영하여 이동량 계산
                            if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowX)
                            {
                                const float MoveAmount = FVector::DotProduct(Delta, TargetComponent->GetForwardVector());
                                NewLocation += TargetComponent->GetForwardVector() * MoveAmount;
                            }
                            else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowY)
                            {
                                const float MoveAmount = FVector::DotProduct(Delta, TargetComponent->GetRightVector());
                                NewLocation += TargetComponent->GetRightVector() * MoveAmount;
                                TargetComponent->SetWorldLocation(NewLocation);
                            }
                            else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
                            {
                                const float MoveAmount = FVector::DotProduct(Delta, TargetComponent->GetUpVector());
                                NewLocation += TargetComponent->GetUpVector() * MoveAmount;
                            }
                        }
                        TargetComponent->SetWorldLocation(NewLocation);
                    }
                }
            }

            // 마우스 휠 이벤트
            else if (InMouseEvent.GetEffectingButton() == EKeys::MouseWheelAxis)
            {
                // 카메라 속도 조절
                if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && ActiveViewportClient->IsPerspective())
                {
                    const float CurrentSpeed = ActiveViewportClient->CameraMovementSpeed;
                    const float Adjustment = FMath::Sign(InMouseEvent.GetWheelDelta()) * FMath::Loge(CurrentSpeed + 1.0f) * 0.5f;

                    ActiveViewportClient->CameraMovementSpeed = CurrentSpeed + Adjustment;
                }
            }
        }));

    InputDelegatesHandles.Add(Handler->OnMouseWheelDelegate.AddLambda([this](const FPointerEvent& InMouseEvent)
        {
            if (ImGui::GetIO().WantCaptureMouse) return;

            // 뷰포트에서 앞뒤 방향으로 화면 이동
            if (ActiveViewportClient->IsPerspective())
            {
                if (!InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
                {
                    const FVector CameraLoc = ActiveViewportClient->GetViewportCamera()->ViewLocation;
                    const FVector CameraForward = ActiveViewportClient->GetViewportCamera()->GetForwardVector();
                    ActiveViewportClient->GetViewportCamera()->ViewLocation =
                        CameraLoc + CameraForward * InMouseEvent.GetWheelDelta() * 50.0f;
                }
            }
            else
            {
                // @todo 
                //FEditorViewportClient::SetOthoSize(-InMouseEvent.GetWheelDelta());
            }
        }));

    InputDelegatesHandles.Add(Handler->OnKeyDownDelegate.AddLambda([this](const FKeyEvent& InKeyEvent)
        {
            ActiveViewportClient->InputKey(InKeyEvent);
        }));

    InputDelegatesHandles.Add(Handler->OnKeyUpDelegate.AddLambda([this](const FKeyEvent& InKeyEvent)
        {
            ActiveViewportClient->InputKey(InKeyEvent);
        }));
}

void SLevelEditor::RegisterPIEInputDelegates()
{
    FGenericSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();

    // Clear current delegate functions
    for (const FDelegateHandle& Handle : InputDelegatesHandles)
    {
        Handler->OnKeyCharDelegate.Remove(Handle);
        Handler->OnKeyDownDelegate.Remove(Handle);
        Handler->OnKeyUpDelegate.Remove(Handle);
        Handler->OnMouseDownDelegate.Remove(Handle);
        Handler->OnMouseUpDelegate.Remove(Handle);
        Handler->OnMouseDoubleClickDelegate.Remove(Handle);
        Handler->OnMouseWheelDelegate.Remove(Handle);
        Handler->OnMouseMoveDelegate.Remove(Handle);
        Handler->OnRawMouseInputDelegate.Remove(Handle);
        Handler->OnRawKeyboardInputDelegate.Remove(Handle);
    }
    // Add Delegate functions in PIE mode
}
