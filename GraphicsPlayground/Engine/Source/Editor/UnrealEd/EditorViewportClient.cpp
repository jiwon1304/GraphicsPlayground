#include "EditorViewportClient.h"
#include "fstream"
#include "sstream"
#include "ostream"
#include "Math/JungleMath.h"
#include "Math/Rect.h"
#include "Engine/UnrealClient.h"
#include "Windows/WindowsCursor.h"
#include "World/World.h"
#include "Classes/GameFramework/Actor.h"
#include "Classes/Engine/EditorEngine.h"

#include "CoreUObject/UObject/ObjectFactory.h"
#include "InteractiveToolsFramework/BaseGizmos/TransformGizmo.h"
#include "Classes/Camera/CameraComponent.h"
#include "Editor/LevelEditor/SLevelEditor.h"
#include "SlateCore/Input/Events.h"
#include "World/PhysicsAssetWorld.h"
#include "Classes/Camera/PlayerCameraManager.h"
// #include "Windows/D3D11RHI/GraphicDevice.h"

FVector FEditorViewportClient::Pivot = FVector(0.0f, 0.0f, 0.0f);
float FEditorViewportClient::OrthoSize = 10.0f;

FEditorViewportClient::FEditorViewportClient()
    : ViewportType(LVT_Perspective)
    , ShowFlag(2047)
    , ViewMode(EViewModeIndex::VMI_Lit_BlinnPhong)
{
}

FEditorViewportClient::~FEditorViewportClient()
{
}

void FEditorViewportClient::Initialize(const FIntRect& InRect, FViewportCamera* InCamera)
{
    FViewportClient::Initialize(InRect, InCamera);

    GizmoActor = FObjectFactory::ConstructObject<ATransformGizmo>(GEngine); // TODO : EditorEngine 외의 다른 Engine 형태가 추가되면 GEngine 대신 다른 방식으로 넣어주어야 함.
    GizmoActor->Initialize(this);
}

void FEditorViewportClient::Tick(const float DeltaTime)
{
    if (GEngine->ActiveWorld->WorldType == EWorldType::Editor
        || GEngine->ActiveWorld->WorldType == EWorldType::SkeletalViewer
        || GEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
    {
        UpdateCamera(DeltaTime);
    }
    //UpdateViewMatrix();
    //UpdateProjectionMatrix();
    GizmoActor->Tick(DeltaTime);
}

void FEditorViewportClient::UpdateCamera(const float DeltaTime)
{
    if (PressedKeys.Contains(EKeys::A))
    {
        ViewportCamera->MoveRight(-CameraMovementSpeed * DeltaTime);
    }

    if (PressedKeys.Contains(EKeys::D))
    {
        ViewportCamera->MoveRight(CameraMovementSpeed * DeltaTime);
    }

    if (PressedKeys.Contains(EKeys::W))
    {
        ViewportCamera->MoveForward(CameraMovementSpeed * DeltaTime);
    }

    if (PressedKeys.Contains(EKeys::S))
    {
        ViewportCamera->MoveForward(-CameraMovementSpeed * DeltaTime);
    }

    if (PressedKeys.Contains(EKeys::E))
    {
        ViewportCamera->MoveUp(CameraMovementSpeed * DeltaTime);
    }

    if (PressedKeys.Contains(EKeys::Q))
    {
        ViewportCamera->MoveUp(-CameraMovementSpeed * DeltaTime);
    }
}

void FEditorViewportClient::InputKey(const FKeyEvent& InKeyEvent)
{
    // if (GetKeyState(VK_RBUTTON) & 0x8000)
    if (true) // TODO: 우클릭 상태인지 확인하는 부분 수정 필요
    {
        switch (InKeyEvent.GetCharacter())
        {
        case 'A':
        {
            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                PressedKeys.Add(EKeys::A);
            }
            else if (InKeyEvent.GetInputEvent() == IE_Released)
            {
                PressedKeys.Remove(EKeys::A);
            }
            break;
        }
        case 'D':
        {
            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                PressedKeys.Add(EKeys::D);
            }
            else if (InKeyEvent.GetInputEvent() == IE_Released)
            {
                PressedKeys.Remove(EKeys::D);
            }
            break;
        }
        case 'W':
        {
            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                PressedKeys.Add(EKeys::W);
            }
            else if (InKeyEvent.GetInputEvent() == IE_Released)
            {
                PressedKeys.Remove(EKeys::W);
            }
            break;
        }
        case 'S':
        {
            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                PressedKeys.Add(EKeys::S);
            }
            else if (InKeyEvent.GetInputEvent() == IE_Released)
            {
                PressedKeys.Remove(EKeys::S);
            }
            break;
        }
        case 'E':
        {
            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                PressedKeys.Add(EKeys::E);
            }
            else if (InKeyEvent.GetInputEvent() == IE_Released)
            {
                PressedKeys.Remove(EKeys::E);
            }
            break;
        }
        case 'Q':
        {
            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                PressedKeys.Add(EKeys::Q);
            }
            else if (InKeyEvent.GetInputEvent() == IE_Released)
            {
                PressedKeys.Remove(EKeys::Q);
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        AEditorPlayer* EdPlayer = CastChecked<UEditorEngine>(GEngine)->GetEditorPlayer();
        switch (InKeyEvent.GetCharacter())
        {
        case 'W':
        {
            EdPlayer->SetMode(CM_TRANSLATION);
            break;
        }
        case 'E':
        {
            EdPlayer->SetMode(CM_ROTATION);
            break;
        }
        case 'R':
        {
            EdPlayer->SetMode(CM_SCALE);
            break;
        }
        default:
            break;
        }
        PressedKeys.Empty();
    }


    // 일반적인 단일 키 이벤트
    if (InKeyEvent.GetInputEvent() == IE_Pressed)
    {
        switch (InKeyEvent.GetCharacter())
        {
        case 'F':
        {
            UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
            USceneComponent* SelectedComponent = Engine->GetSelectedComponent();
            AActor* SelectedActor = Engine->GetSelectedActor();

            USceneComponent* TargetComponent = nullptr;

            if (SelectedComponent != nullptr)
            {
                TargetComponent = SelectedComponent;
            }
            else if (SelectedActor != nullptr)
            {
                TargetComponent = SelectedActor->GetRootComponent();
            }
            if (TargetComponent != nullptr)
            {
                //if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(TargetComponent))
                //{
                //    FViewportCamera& ViewTransform = PerspectiveCamera;
                //    float FOV = ViewFOV;

                //    // 로컬 바운딩 박스
                //    FBoundingBox Box = Primitive->GetBoundingBox();
                //    FVector LocalCenter = (Box.MinLocation + Box.MaxLocation) * 0.5f;
                //    FVector LocalExtents = (Box.MaxLocation - Box.MinLocation) * 0.5f;
                //    float Radius = LocalExtents.Length();

                //    FVector WorldCenter;
                //    
                //    if (Engine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
                //    {
                //        FMatrix ComponentToWorld = Engine->PhysicsAssetEditorWorld->GetSelectedTransform().ToMatrixWithScale();
                //        WorldCenter = ComponentToWorld.GetTranslationVector();
                //        Radius = 10;
                //    }
                //    else
                //    {
                //        FMatrix ComponentToWorld = Primitive->GetWorldMatrix();
                //        WorldCenter = ComponentToWorld.TransformPosition(LocalCenter);
                //    }

                //    // FOV 기반 거리 계산
                //    float VerticalFOV = FMath::DegreesToRadians(FOV);
                //    float Distance = FMath::Max(Radius / FMath::Tan(VerticalFOV * 0.5f), 10.0f);

                //    // 카메라 위치 설정
                //    ViewTransform.SetLocation(WorldCenter - ViewTransform.GetForwardVector() * Distance);
                //}

                // else
                //{
                //    FViewportCamera& ViewTransform = PerspectiveCamera;
                //    ViewTransform.SetLocation(
                //        // TODO: 10.0f 대신, 정점의 min, max의 거리를 구해서 하면 좋을 듯
                //        TargetComponent->GetComponentLocation() - (ViewTransform.GetForwardVector() * 10.0f)
                //    );
                //}
            }
            break;
        }
        case 'M':
        {
            SLevelEditor* LevelEd = GEngineLoop.GetLevelEditor();
            LevelEd->SetEnableMultiViewport(!LevelEd->IsMultiViewport());
            break;
        }
        default:
            break;
        }

        // Virtual Key
        UEditorEngine* EdEngine = CastChecked<UEditorEngine>(GEngine);
        switch (InKeyEvent.GetKey())
        {
        case EKeys::Delete:
        {
            if (GEngine->ActiveWorld->WorldType == EWorldType::SkeletalViewer || GEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
                return;
            UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
            if (Engine)
            {
                USceneComponent* SelectedComponent = Engine->GetSelectedComponent();
                AActor* SelectedActor = Engine->GetSelectedActor();

                if (SelectedComponent)
                {
                    AActor* Owner = SelectedComponent->GetOwner();

                    if (Owner && Owner->GetRootComponent() != SelectedComponent)
                    {
                        UE_LOG(ELogLevel::Display, "Delete Component - %s", *SelectedComponent->GetName());
                        Engine->DeselectComponent(SelectedComponent);
                        SelectedComponent->DestroyComponent();
                    }
                    else if (SelectedActor)
                    {
                        UE_LOG(ELogLevel::Display, "Delete Component - %s", *SelectedActor->GetName());
                        Engine->DeselectActor(SelectedActor);
                        Engine->DeselectComponent(SelectedComponent);
                        Engine->ActiveWorld->DestroyActor(SelectedActor);
                    }
                }
                else if (SelectedActor)
                {
                    UE_LOG(ELogLevel::Display, "Delete Component - %s", *SelectedActor->GetName());
                    Engine->DeselectActor(SelectedActor);
                    Engine->DeselectComponent(SelectedComponent);
                    Engine->ActiveWorld->DestroyActor(SelectedActor);
                }
            }
            break;
        }
        case EKeys::SpaceBar:
        {
            EdEngine->GetEditorPlayer()->AddControlMode();
            break;
        }
        default:
            break;
        }
    }
    return;
}

void FEditorViewportClient::MouseMove(const FPointerEvent& InMouseEvent)
{
    const auto& [DeltaX, DeltaY] = InMouseEvent.GetCursorDelta();

    // Yaw(좌우 회전) 및 Pitch(상하 회전) 값 변경
    if (IsPerspective())
    {
        ViewportCamera->RotateYaw(DeltaX * CameraRotationSensitivity);
        ViewportCamera->RotatePitch(DeltaY * CameraRotationSensitivity);
    }
    //else
    //{
    //    PivotMoveRight(DeltaX);
    //    PivotMoveUp(DeltaY);
    //}
}

void FEditorViewportClient::DeprojectScreenToWorld(const FIntPoint& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDir) const
{
    const int32 TopLeftX = ViewportRect.Min.X;
    const int32 TopLeftY = ViewportRect.Min.Y;
    const int32 Width = ViewportRect.GetWidth();
    const int32 Height = ViewportRect.GetHeight();

    // 뷰포트의 NDC 위치
    const FVector2D NDCPos = {
        (static_cast<float>(ScreenPos.X - TopLeftX) / Width * 2.0f) - 1.0f,
        1.0f - (static_cast<float>(ScreenPos.Y - TopLeftY) / Height * 2.0f)
    };

    ViewportCamera->DeprojectNDCToWorld(NDCPos, OutWorldOrigin, OutWorldDir);
}

void FEditorViewportClient::DeprojectScreenToView(const FIntPoint& ScreenPos, FVector& OutViewOrigin, FVector& OutViewDir) const
{
    const int32 TopLeftX = ViewportRect.Min.X;
    const int32 TopLeftY = ViewportRect.Min.Y;
    const int32 Width = ViewportRect.GetWidth();
    const int32 Height = ViewportRect.GetHeight();

    const FVector2D NDCPos = {
        (static_cast<float>(ScreenPos.X - TopLeftX) / Width * 2.0f) - 1.0f,
        1.0f - (static_cast<float>(ScreenPos.Y - TopLeftY) / Height * 2.0f)
    };

    ViewportCamera->DeprojectNDCToView(NDCPos, OutViewOrigin, OutViewDir);
}

void FEditorViewportClient::GetViewInfo(FMinimalViewInfo& OutViewInfo) const
{
    bool bGotViewInfo = false;
        
    OutViewInfo = FMinimalViewInfo();
    if (APlayerController* PC = GEngine->ActiveWorld->GetPlayerController())
    {
        if (APlayerCameraManager* PCM = PC->PlayerCameraManager)
        {
            if (PCM->PendingViewTarget.Target != nullptr)
            {
                OutViewInfo = PCM->LastFrameViewTarget.POV;
            }else
            {
                OutViewInfo = PCM->ViewTarget.POV;
            }
            bGotViewInfo = true;
        }
    }

    if (!bGotViewInfo)
    {
        OutViewInfo = FMinimalViewInfo();
    }
}

//void FEditorViewportClient::CameraMoveForward(const float InValue)
//{
//    if (IsPerspective())
//    {
//        FVector CurCameraLoc = PerspectiveCamera.GetLocation();
//        CurCameraLoc = CurCameraLoc + PerspectiveCamera.GetForwardVector() * GetCameraSpeedScalar() * InValue;
//        PerspectiveCamera.SetLocation(CurCameraLoc);
//    }
//    else
//    {
//        Pivot.X += InValue * 0.1f;
//    }
//}
//
//void FEditorViewportClient::CameraMoveRight(const float InValue)
//{
//    if (IsPerspective())
//    {
//        FVector CurCameraLoc = PerspectiveCamera.GetLocation();
//        CurCameraLoc = CurCameraLoc + PerspectiveCamera.GetRightVector() * GetCameraSpeedScalar() * InValue;
//        PerspectiveCamera.SetLocation(CurCameraLoc);
//    }
//    else
//    {
//        Pivot.Y += InValue * 0.1f;
//    }
//}
//
//void FEditorViewportClient::CameraMoveUp(const float InValue)
//{
//    if (IsPerspective())
//    {
//        FVector CurCameraLoc = PerspectiveCamera.GetLocation();
//        CurCameraLoc.Z = CurCameraLoc.Z + GetCameraSpeedScalar() * InValue;
//        PerspectiveCamera.SetLocation(CurCameraLoc);
//    }
//    else
//    {
//        Pivot.Z += InValue * 0.1f;
//    }
//}
//
//void FEditorViewportClient::CameraRotateYaw(const float InValue)
//{
//    FVector CurCameraRot = PerspectiveCamera.GetRotation();
//    CurCameraRot.Z += InValue ;
//    PerspectiveCamera.SetRotation(CurCameraRot);
//}
//
//void FEditorViewportClient::CameraRotatePitch(const float InValue)
//{
//    FVector CurCameraRot = PerspectiveCamera.GetRotation();
//    CurCameraRot.Y = FMath::Clamp(CurCameraRot.Y + InValue, -89.f, 89.f);
//    PerspectiveCamera.SetRotation(CurCameraRot);
//}

//void FEditorViewportClient::PivotMoveRight(const float InValue) const
//{
//    Pivot = Pivot + OrthogonalCamera.GetRightVector() * InValue * -0.05f;
//}
//
//void FEditorViewportClient::PivotMoveUp(const float InValue) const
//{
//    Pivot = Pivot + OrthogonalCamera.GetUpVector() * InValue * 0.05f;
//}

//void FEditorViewportClient::UpdateViewMatrix()
//{
//    if (GEngine && GEngine->ActiveWorld->WorldType == EWorldType::PIE && Engine==GEngine)
//    {
//        FMinimalViewInfo ViewInfo;
//        GetViewInfo(ViewInfo);
//
//        FMatrix RotationMatrix = ViewInfo.Rotation.ToMatrix();
//        FVector FinalUp = FMatrix::TransformVector(FVector::UpVector, RotationMatrix);
//        
//        View = JungleMath::CreateViewMatrix(
//            ViewInfo.Location,
//            ViewInfo.Location + ViewInfo.Rotation.ToVector(),
//            FinalUp
//        );
//    }
//    else
//    {
//        if (IsPerspective())
//        {
//            View = JungleMath::CreateViewMatrix(PerspectiveCamera.GetLocation(),
//                PerspectiveCamera.GetLocation() + PerspectiveCamera.GetForwardVector(),
//                FVector{ 0.0f,0.0f, 1.0f }
//            );
//        }
//        else 
//        {
//            UpdateOrthoCameraLoc();
//            if (ViewportType == LVT_OrthoXY || ViewportType == LVT_OrthoNegativeXY)
//            {
//                View = JungleMath::CreateViewMatrix(OrthogonalCamera.GetLocation(),
//                    Pivot, FVector(0.0f, -1.0f, 0.0f)
//                );
//            }
//            else
//            {
//                View = JungleMath::CreateViewMatrix(OrthogonalCamera.GetLocation(),
//                    Pivot, FVector(0.0f, 0.0f, 1.0f)
//                );
//            }
//        }
//    }
//}
//
//void FEditorViewportClient::UpdateProjectionMatrix()
//{
//    AspectRatio = GetViewport()->GetD3DViewport().Width / GetViewport()->GetD3DViewport().Height;
//
//    if (GEngine && GEngine->ActiveWorld->WorldType == EWorldType::PIE && Engine==GEngine)
//    {
//        FMinimalViewInfo ViewInfo;
//        GetViewInfo(ViewInfo);
//        
//        Projection = JungleMath::CreateProjectionMatrix(
//            FMath::DegreesToRadians(ViewInfo.FOV),
//            AspectRatio,
//            ViewInfo.PerspectiveNearClip,
//            ViewInfo.PerspectiveFarClip
//        );
//    }else
//    {
//        if (IsPerspective())
//        {
//            Projection = JungleMath::CreateProjectionMatrix(
//                FMath::DegreesToRadians(ViewFOV),
//                AspectRatio,
//                NearClip,
//                FarClip
//            );
//        }
//        else
//        {
//            // 오쏘그래픽 너비는 줌 값과 가로세로 비율에 따라 결정됩니다.
//            const float OrthoWidth = OrthoSize * AspectRatio;
//            const float OrthoHeight = OrthoSize;
//
//            // 오쏘그래픽 투영 행렬 생성 (nearPlane, farPlane 은 기존 값 사용)
//            Projection = JungleMath::CreateOrthoProjectionMatrix(
//                OrthoWidth,
//                OrthoHeight,
//                NearClip,
//                FarClip
//            );
//        }
//    }
//}

ELevelViewportType FEditorViewportClient::GetViewportType() const
{
    ELevelViewportType EffectiveViewportType = ViewportType;
    if (EffectiveViewportType == LVT_None)
    {
        EffectiveViewportType = LVT_Perspective;
    }
    return EffectiveViewportType;
}

void FEditorViewportClient::SetViewportType(ELevelViewportType InViewportType)
{
    ViewportType = InViewportType;
}

bool FEditorViewportClient::IsPerspective() const
{
    return ViewportType == LVT_Perspective;
}

void FEditorViewportClient::UpdateOrthoCameraLoc()
{
//    switch (ViewportType)
//    {
//    case LVT_OrthoXY: // Top
//        ViewportCamera->ViewLocation = Pivot + FVector::UpVector * ViewportCamera->FarClip * 0.5f;
//        ViewportCamera->ViewRotation = FVector(0.0f, 90.0f, -90.0f);
//        break;
//    case LVT_OrthoXZ: // Front
//        OrthogonalCamera.SetLocation(Pivot + FVector::ForwardVector * FarClip * 0.5f);
//        OrthogonalCamera.SetRotation(FVector(0.0f, 0.0f, 180.0f));
//        break;
//    case LVT_OrthoYZ: // Left
//        OrthogonalCamera.SetLocation(Pivot + FVector::RightVector * FarClip * 0.5f);
//        OrthogonalCamera.SetRotation(FVector(0.0f, 0.0f, 270.0f));
//        break;
//    case LVT_OrthoNegativeXY: // Bottom
//        OrthogonalCamera.SetLocation(Pivot + FVector::UpVector * -1.0f * FarClip * 0.5f);
//        OrthogonalCamera.SetRotation(FVector(0.0f, -90.0f, 90.0f));
//        break;
//    case LVT_OrthoNegativeXZ: // Back
//        OrthogonalCamera.SetLocation(Pivot + FVector::ForwardVector * -1.0f * FarClip * 0.5f);
//        OrthogonalCamera.SetRotation(FVector(0.0f, 0.0f, 0.0f));
//        break;
//    case LVT_OrthoNegativeYZ: // Right
//        OrthogonalCamera.SetLocation(Pivot + FVector::RightVector * -1.0f * FarClip * 0.5f);
//        OrthogonalCamera.SetRotation(FVector(0.0f, 0.0f, 90.0f));
//        break;
//    case LVT_None:
//    case LVT_Perspective:
//    case LVT_MAX:
//    default:
//        break;
//    }
}

//void FEditorViewportClient::SetOthoSize(const float InValue)
//{
//    OrthoSize += InValue;
//    OrthoSize = FMath::Max(OrthoSize, 0.1f);
//}

void FEditorViewportClient::LoadConfig(const TMap<FString, FString>& Config)
{
    //FString ViewportNum = std::to_string(ViewportIndex);
    //CameraSpeedSetting = GetValueFromConfig(Config, "CameraSpeedSetting" + ViewportNum, 1);
    //CameraSpeed = GetValueFromConfig(Config, "CameraSpeedScalar" + ViewportNum, 1.0f);
    //GridSize = GetValueFromConfig(Config, "GridSize"+ ViewportNum, 10.0f);
    //PerspectiveCamera.ViewLocation.X = GetValueFromConfig(Config, "PerspectiveCameraLocX" + ViewportNum, 0.0f);
    //PerspectiveCamera.ViewLocation.Y = GetValueFromConfig(Config, "PerspectiveCameraLocY" + ViewportNum, 0.0f);
    //PerspectiveCamera.ViewLocation.Z = GetValueFromConfig(Config, "PerspectiveCameraLocZ" + ViewportNum, 0.0f);
    //PerspectiveCamera.ViewRotation.X = GetValueFromConfig(Config, "PerspectiveCameraRotX" + ViewportNum, 0.0f);
    //PerspectiveCamera.ViewRotation.Y = GetValueFromConfig(Config, "PerspectiveCameraRotY" + ViewportNum, 0.0f);
    //PerspectiveCamera.ViewRotation.Z = GetValueFromConfig(Config, "PerspectiveCameraRotZ" + ViewportNum, 0.0f);
    //ShowFlag = GetValueFromConfig(Config, "ShowFlag" + ViewportNum, 63.0f);
    //ViewMode = static_cast<EViewModeIndex>(GetValueFromConfig(Config, "ViewMode" + ViewportNum, 0));
    //ViewportType = static_cast<ELevelViewportType>(GetValueFromConfig(Config, "ViewportType" + ViewportNum, 3));
}

void FEditorViewportClient::SaveConfig(TMap<FString, FString>& Config) const
{
    //const FString ViewportNum = std::to_string(ViewportIndex);
    //Config["CameraSpeedSetting"+ ViewportNum] = std::to_string(CameraSpeedSetting);
    //Config["CameraSpeedScalar"+ ViewportNum] = std::to_string(CameraSpeed);
    //Config["GridSize"+ ViewportNum] = std::to_string(GridSize);
    //Config["PerspectiveCameraLocX" + ViewportNum] = std::to_string(PerspectiveCamera.GetLocation().X);
    //Config["PerspectiveCameraLocY" + ViewportNum] = std::to_string(PerspectiveCamera.GetLocation().Y);
    //Config["PerspectiveCameraLocZ" + ViewportNum] = std::to_string(PerspectiveCamera.GetLocation().Z);
    //Config["PerspectiveCameraRotX" + ViewportNum] = std::to_string(PerspectiveCamera.GetRotation().X);
    //Config["PerspectiveCameraRotY" + ViewportNum] = std::to_string(PerspectiveCamera.GetRotation().Y);
    //Config["PerspectiveCameraRotZ" + ViewportNum] = std::to_string(PerspectiveCamera.GetRotation().Z);
    //Config["ShowFlag"+ ViewportNum] = std::to_string(ShowFlag);
    //Config["ViewMode" + ViewportNum] = std::to_string(static_cast<int32>(ViewMode));
    //Config["ViewportType" + ViewportNum] = std::to_string(ViewportType);
}

TMap<FString, FString> FEditorViewportClient::ReadIniFile(const FString& FilePath)
{
    TMap<FString, FString> Config;
    std::ifstream File(FilePath.ToUTF8String());
    std::string Line;

    while (std::getline(File, Line))
    {
        if (Line.empty() || Line[0] == '[' || Line[0] == ';')
        {
            continue;
        }
        std::istringstream SS(Line);
        std::string Key, Value;
        if (std::getline(SS, Key, '=') && std::getline(SS, Value))
        {
            Config[Key] = Value;
        }
    }
    return Config;
}

auto FEditorViewportClient::WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config) -> void
{
    std::ofstream File(FilePath.ToUTF8String());
    for (const auto& Pair : Config)
    {
        File << Pair.Key.ToUTF8String() << "=" << Pair.Value.ToUTF8String() << "\n";
    }
}
