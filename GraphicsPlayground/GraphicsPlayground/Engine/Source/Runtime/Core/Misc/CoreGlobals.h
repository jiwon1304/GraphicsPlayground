#include "Runtime/Core/Container/String.h"

extern int GWindowWidth;
extern int GWindowHeight;
extern const TCHAR* GWindowName;

extern FString GEditorIni;
extern FString GEditorIni;

extern FString GWindowName;

extern double GSystemStartTime;

extern uint64 GFrameCounter;
extern uint64 GFrameCounterRenderThread;

extern uint32 GGameThreadId;
extern uint32 GRenderThreadId;

// editor의 near plane등등... 
// TODO : defines.h에서 옮기기

/**
 * Threads
 */
extern bool IsInGameThread();
extern bool IsInRenderThread();
extern bool IsInRHIThread();

