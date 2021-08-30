// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_CONFIG_INCLUDED
#define WINGSTALL_CONFIG_INCLUDED
#include <wingstall_config/api.hpp>
#include <sngxml/dom/Document.hpp>

namespace wingstall { namespace config {

std::string ConfigFilePath();
std::string DefaultBoostIncludeDir();
std::string DefaultBoostLibDir();
std::string DefaultVCVarsFilePath();
std::unique_ptr<sngxml::dom::Document> ConfigurationDocument();

} } // namespace wingstall::config

#endif // WINGSTALL_CONFIG_INCLUDED
