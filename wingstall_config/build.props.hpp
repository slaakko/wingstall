// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_CONFIG_BUILD_PROPS_INCLUDED
#define WINGSTALL_CONFIG_BUILD_PROPS_INCLUDED
#include <wingstall_config/api.hpp>
#include <string>

namespace wingstall { namespace config {

void MakeBuildPropsFile(const std::string& boostIncludeDir, const std::string& boostLibDir);

} } // namespace wingstall::config

#endif // WINGSTALL_CONFIG_BUILD_PROPS_INCLUDED
