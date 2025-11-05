#include "MacApplication.h"
#include "MacWindow.h"

std::shared_ptr<FGenericWindow> FMacApplication::MakeWindow(std::shared_ptr<FGenericWindowInitParams> Params)
{
    std::shared_ptr<FMacWindow> NewWindow = std::make_shared<FMacWindow>(Params);
    return NewWindow;
}