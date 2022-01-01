// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_CONFIG_CONFIG_INCLUDED
#define WINGSTALL_CONFIG_CONFIG_INCLUDED
#include <wingstall_config/api.hpp>
#include <sngxml/dom/Document.hpp>

namespace wingstall { namespace config {

std::string WingstallConfigDir();
std::string ConfigFilePath();
std::string DefaultBoostIncludeDir();
std::string DefaultBoostLibDir();
std::string DefaultVCVarsFilePath();
std::string DefaultVCPlatformToolset();
std::unique_ptr<sngxml::dom::Document> ConfigurationDocument();
std::string BoostIncludeDir(sngxml::dom::Document* configDocument);
std::string BoostLibDir(sngxml::dom::Document* configDocument);
std::string VCVarsFilePath(sngxml::dom::Document* configDocument);
std::string VCPlatformToolset(sngxml::dom::Document* configDocument);
void SetBoostIncludeDir(sngxml::dom::Document* configDocument, const std::string& boostIncludeDir);
void SetBoostLibDir(sngxml::dom::Document* configDocument, const std::string& boostLibDir);
void SetVCVarsFilePath(sngxml::dom::Document* configDocument, const std::string& vcVarsFilePath);
void SetVCPlatformToolset(sngxml::dom::Document* configDocument, const std::string& vcPlatformToolset);
void SaveConfiguration(sngxml::dom::Document* configDocument);

} } // namespace wingstall::config

#endif // WINGSTALL_CONFIG_CONFIG_INCLUDED
