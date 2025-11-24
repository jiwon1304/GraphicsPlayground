#include "Launch/ImGuiManager.h"

#include <d3d11.h>

class FImGuiManagerD3D11 : public FImGuiManager
{
public:
    void Initialize(HWND hWnd, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext);
    void BeginFrame() const override;
    void EndFrame() const override;
    void Shutdown() override;
};