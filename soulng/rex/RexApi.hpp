// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_API_INCLUDED
#define SOULNG_REX_API_INCLUDED

#ifndef SOULNG_REX_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "soulng_rex.lib")
#else
#pragma comment(lib, "soulng_rexd.lib")
#endif

#endif // SOULNG_REX_EXPORTS

#define SOULNG_REX_API

#endif // SOULNG_REX_API_INCLUDED
