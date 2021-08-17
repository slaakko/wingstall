// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_ENVIRONMENT_INCLUDED
#define WING_ENVIRONMENT_INCLUDED
#include <wing/Registry.hpp>

namespace wing {

WING_API bool HasSystemEnvironmentVariable(const std::string& name);

WING_API std::string GetSystemEnvironmentVariable(const std::string& name);

WING_API void SetSystemEnvironmentVariable(const std::string& name, const std::string& value, RegistryValueKind valueKind);

WING_API void DeleteSystemEnvironmentVariable(const std::string& name);

WING_API bool HasPathDirectory(const std::string& directory);

WING_API void AppendPathDirectory(const std::string& directory);

WING_API void RemovePathDirectory(const std::string& directory);

WING_API void BroadcastEnvironmentChangedMessage();

} // wing

#endif // WING_ENVIRONMENT_INCLUDED
