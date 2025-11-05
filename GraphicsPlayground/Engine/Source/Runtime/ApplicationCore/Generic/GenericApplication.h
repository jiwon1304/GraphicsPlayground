#pragma once

#include "Core/HAL/PlatformType.h"

class FGenericSlateAppMessageHandler;
class FGenericWindow;
struct FGenericWindowInitParams;

struct FPlatformRect
{
	int32 Left;
	int32 Top;
	int32 Right;
	int32 Bottom;

	FPlatformRect() : Left(0), Top(0), Right(0), Bottom(0) {}
	FPlatformRect(int32 InLeft, int32 InTop, int32 InRight, int32 InBottom) : Left(InLeft), Top(InTop), Right(InRight), Bottom(InBottom) {}
};

struct FGenericApplicationInitParams
{
    const char* AppName;
    const wchar_t* AppNameW;
};

/**
 * Corresponds to Windows hInstance
 * Does not track FGenericWindow lifetime
 */
class FGenericApplication
{
public:
    FGenericApplication(FGenericApplicationInitParams* InInitParams) {}
    virtual ~FGenericApplication() = default;

    virtual std::shared_ptr<FGenericWindow> MakeWindow(std::shared_ptr<FGenericWindowInitParams> Params) = 0;

    void SetMessageHandler(std::shared_ptr<FGenericSlateAppMessageHandler> InSlateAppMessageHandler) { SlateAppMessageHandler = InSlateAppMessageHandler; }

    std::shared_ptr<FGenericSlateAppMessageHandler> GetMessageHandler() const { return SlateAppMessageHandler; }
protected:
    std::shared_ptr<FGenericSlateAppMessageHandler> SlateAppMessageHandler;
};