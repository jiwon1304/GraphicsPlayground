#pragma once
#include "ThirdParty/sol2/include/sol/sol.hpp"
#include "Templates/TemplateUtilities.h"


template <typename T>
struct FLuaCustomBind
{
    static void Bind([[maybe_unused]] sol::state& State)
    {
        static_assert(TAlwaysFalse<T>, "Binding not implemented for this type!");
    }
};
