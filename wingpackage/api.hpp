// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_API_INCLUDED
#define WINGSTALL_WINGPACKAGE_API_INCLUDED

#ifndef WINGPACKAGE_EXPORTS

#ifdef NDEBUG

#pragma comment(lib, "wingpackage.lib")

#else

#pragma comment(lib, "wingpackaged.lib")

#endif 

#endif

#endif // WINGSTALL_WINGPACKAGE_API_INCLUDED
