#include "Core/Container/String.h"

int GWindowWidth = 1400;
int GWindowHeight = 1000;
const TCHAR* GWindowName = TEXT("GraphicsPlayground");

FString GEditorIni;

double GSystemStartTime = 0.0;

uint64 GFrameCounter = 0;
uint64 GFrameCounterRenderThread = 0;

uint32 GGameThreadId = 0;
uint32 GRHIThreadId = 0;
uint32 GRenderThreadId = 0;
