// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_CONFIG_API_INCLUDED
#define WINGSTALL_CONFIG_API_INCLUDED

#ifndef WINGSTALL_CONFIG_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "wingstall_config.lib")
#else
#pragma comment(lib, "wingstall_configd.lib")
#endif

#endif // WINGSTALL_CONFIG_EXPORTS

#define CONFIG_API

#endif // WINGSTALL_CONFIG_API_INCLUDED
