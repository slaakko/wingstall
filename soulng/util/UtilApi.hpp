// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_API_INCLUDED
#define SOULNG_UTIL_API_INCLUDED

#ifndef SOULNG_UTIL_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "soulng_util.lib")
#else
#pragma comment(lib, "soulng_utild.lib")
#endif

#endif // SOULNG_UTIL_EXPORTS

#define UTIL_API

#endif // SOULNG_UTIL_API_INCLUDED
