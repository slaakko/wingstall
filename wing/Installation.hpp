// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_INSTALLATION_INCLUDED
#define WING_INSTALLATION_INCLUDED
#include <wing/Registry.hpp>
#include <vector>

namespace wing {

std::vector<std::string> GetInstalledSoftwareRegistryKeyPaths();

RegistryKey OpenOrCreateRegistryKeyForSoftwareKey(const std::string& softwareKey);
void DeleteRegistryKeyForSoftwareKey(const std::string& softwareKey);

} // wing

#endif // WING_INSTALLATION_INCLUDED
