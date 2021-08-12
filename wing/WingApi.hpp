// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_API_INCLUDED
#define WING_API_INCLUDED

#ifndef WING_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "wing.lib")
#else
#pragma comment(lib, "wingd.lib")
#endif

#endif // WING_EXPORTS

#define WING_API

#endif // WING_API_INCLUDED
