// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef INIT_DONE_INCLUDED
#define INIT_DONE_INCLUDED
#include <wing/WingApi.hpp>
#include <Windows.h>

namespace cmajor { namespace wing {

WING_API void Init(HINSTANCE instance);
WING_API void Done();

} } // cmajor::wing

#endif // INIT_DONE_INCLUDED
