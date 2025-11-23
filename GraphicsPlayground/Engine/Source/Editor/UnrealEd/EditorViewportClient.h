#pragma once
#include <sstream>

#include "Core/Math/Vector.h"
#include "Container/Map.h"
#include "ViewportClient.h"
#include "Launch/EngineLoop.h"
#include "Launch/EngineBaseTypes.h"
#include "Core/Math/Rect.h"
#include "SlateCore/Input/Events.h"
#include "Core/Math/Matrix.h"



struct FPointerEvent;
class FViewportResource;
class ATransformGizmo;
class USceneComponent;
struct FMinimalViewInfo;
class UEngine;

enum class EViewScreenLocation : uint8
{
    EVL_TopLeft,
    EVL_TopRight,
    EVL_BottomLeft,
    EVL_BottomRight,
    EVL_MAX,
};

/**
 * Contains where to draw FViewport. 
 * @todo : FViewportClient로 필요한거는 옮기기
 */
class FEditorViewportClient : public FViewportClient
{
public:
    FEditorViewportClient();
    virtual ~FEditorViewportClient() override;

    virtual UWorld* GetWorld() const override { return nullptr; }
    virtual void Initialize(const FIntRect& InRect, FViewportCamera* InCamera) override;
    void Tick(float DeltaTime);

    /**
     * Inputs
     * Inputs are accumulated and processed in Tick
     */
public:
    void InputKey(const FKeyEvent& InKeyEvent);
    void MouseMove(const FPointerEvent& InMouseEvent);
    /**
     * ScreenPos를 World Space로 Deprojection 합니다.
     * @param ScreenPos Point on the application's window (not this viewport/split)
     * @param OutWorldOrigin Origin Vector (World Space)
     * @param OutWorldDir Direction Vector (World Space)
     */
    void DeprojectScreenToWorld(const FIntPoint& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDir) const;

    void DeprojectScreenToView(const FIntPoint& ScreenPos, FVector& OutViewOrigin, FVector& OutViewDir) const;

private:
    // Currently pressed keys. (Pressed but not yet released)
    TSet<EKeys::Type> PressedKeys;

    // Camera
public:
    float CameraMovementSpeed = 1.0f;
    float CameraRotationSensitivity = 0.1f;

    ELevelViewportType GetViewportType() const;
    void SetViewportType(ELevelViewportType InViewportType);

    bool IsPerspective() const;
protected:
    void UpdateCamera(float DeltaTime);

    void GetViewInfo(FMinimalViewInfo& OutViewInfo) const;

    ELevelViewportType ViewportType;

public:

public:
    static FVector Pivot;
    static float OrthoSize;
    uint64 ShowFlag;
    EViewModeIndex ViewMode;

    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    

    void UpdateOrthoCameraLoc();
    
    EViewModeIndex GetViewMode() const { return ViewMode; }
    void SetViewMode(EViewModeIndex InViewMode) { ViewMode = InViewMode; }
    
    uint64 GetShowFlag() const { return ShowFlag; }
    void SetShowFlag(uint64 InShowFlag) { ShowFlag = InShowFlag; }

public:
    void LoadConfig(const TMap<FString, FString>& Config);
    void SaveConfig(TMap<FString, FString>& Config) const;

private:
    static TMap<FString, FString> ReadIniFile(const FString& FilePath);
    static void WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config);

private:
    template <typename T>
    T GetValueFromConfig(const TMap<FString, FString>& config, const FString& key, T defaultValue)
    {
        if (const FString* Value = config.Find(key))
        {
            std::istringstream iss(Value->ToUTF8String());
            T value;
            if (iss >> value)
            {
                return value;
            }
        }
        return defaultValue;
    }

public:
    // Gizmo
    // void SetGizmoActor(ATransformGizmo* gizmo) { GizmoActor = gizmo; }
    ATransformGizmo* GetGizmoActor() const { return GizmoActor; }

    void SetPickedGizmoComponent(USceneComponent* component) { PickedGizmoComponent = component; }
    USceneComponent* GetPickedGizmoComponent() const { return PickedGizmoComponent; }

    void SetShowGizmo(bool bShow) { bShowGizmo = bShow; }
    bool IsShowGizmo() const { return bShowGizmo; }

private:
    ATransformGizmo* GizmoActor = nullptr;
    USceneComponent* PickedGizmoComponent = nullptr;
    bool bShowGizmo = true;

    UEngine* Engine = nullptr;
};
