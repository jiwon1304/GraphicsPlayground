#include "Launch/ImGuiManager.h"
#include "OpenGLDrv/OpenGLFwd.h"

class FImGuiManagerOpenGL : public FImGuiManager
{
public:
    void Initialize(FOpenGL::Window* InWindow);
    void BeginFrame() const override;
    void EndFrame() const override;
    void Shutdown() override;
};