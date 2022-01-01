// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_PARSER_API_INCLUDED
#define SOULNG_PARSER_API_INCLUDED

#ifndef NDEBUG
#define SOULNG_PARSER_DEBUG_SUPPORT
#endif

#ifndef SOULNG_PARSER_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "soulng_parser.lib")
#else
#pragma comment(lib, "soulng_parserd.lib")
#endif

#endif // SOULNG_PARSER_EXPORTS

#define SOULNG_PARSER_API

#endif // SOULNG_PARSER_API_INCLUDED
