// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef INIT_DONE_INCLUDED
#define INIT_DONE_INCLUDED
#include <wing/WingApi.hpp>
#include <Windows.h>

namespace wing {

WING_API void Init(HINSTANCE instance);
WING_API void Done();

} // wing

#endif // INIT_DONE_INCLUDED
