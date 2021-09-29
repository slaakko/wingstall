// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingstall_config/config.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace wingstall { namespace config {

using namespace soulng::unicode;

std::string WingstallConfigDir()
{
    std::string configDir = Path::Combine(WingstallRoot(), "config");
    boost::filesystem::create_directories(configDir);
    return configDir;
}

std::string defaultBoostIncludeDir = "C:\\boost\\include\\boost-1_74";
std::string defaultBoostLibDir = "C:\\boost\\lib";
std::string defaultVCVarsFilePath = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat)";

std::string ConfigFilePath()
{
    return Path::Combine(GetFullPath(WingstallConfigDir()), "configuration.xml");
}

std::string DefaultBoostIncludeDir()
{
    return defaultBoostIncludeDir;
}

std::string DefaultBoostLibDir()
{
    return defaultBoostLibDir;
}

std::string DefaultVCVarsFilePath()
{
    return defaultVCVarsFilePath;
}

std::unique_ptr<sngxml::dom::Document> ConfigurationDocument()
{
    std::string operation;
    try
    {
        std::unique_ptr<sngxml::dom::Document> configDoc;
        if (boost::filesystem::exists(ConfigFilePath()))
        {
            operation = "read/write";
            configDoc = sngxml::dom::ReadDocument(ConfigFilePath());
            std::u32string vcVarsFilePath = configDoc->DocumentElement()->GetAttribute(U"vcVarsFilePath");
            if (vcVarsFilePath.empty())
            {
                configDoc->DocumentElement()->SetAttribute(U"vcVarsFilePath", ToUtf32(defaultVCVarsFilePath));
            }
            std::ofstream configFile(ConfigFilePath());
            CodeFormatter formatter(configFile);
            configDoc->Write(formatter);
        }
        else
        {
            operation = "write";
            configDoc.reset(new sngxml::dom::Document());
            sngxml::dom::Element* rootElement = new sngxml::dom::Element(U"configuration");
            configDoc->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement));
            rootElement->SetAttribute(U"boostIncludeDir", ToUtf32(defaultBoostIncludeDir));
            rootElement->SetAttribute(U"boostLibDir", ToUtf32(defaultBoostLibDir));
            rootElement->SetAttribute(U"vcVarsFilePath", ToUtf32(defaultVCVarsFilePath));
            std::ofstream configFile(ConfigFilePath());
            CodeFormatter formatter(configFile);
            configDoc->Write(formatter);
        }
        return configDoc;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "could not " << operation << " configuration document '" + ConfigFilePath() + "': " + ex.what() << std::endl;;
    }
    return std::unique_ptr<sngxml::dom::Document>();
}

std::string BoostIncludeDir(sngxml::dom::Document* configDocument)
{
    return ToUtf8(configDocument->DocumentElement()->GetAttribute(U"boostIncludeDir"));
}

std::string BoostLibDir(sngxml::dom::Document* configDocument)
{
    return ToUtf8(configDocument->DocumentElement()->GetAttribute(U"boostLibDir"));
}

std::string VCVarsFilePath(sngxml::dom::Document* configDocument)
{
    return ToUtf8(configDocument->DocumentElement()->GetAttribute(U"vcVarsFilePath"));
}

void SetBoostIncludeDir(sngxml::dom::Document* configDocument, const std::string& boostIncludeDir)
{
    configDocument->DocumentElement()->SetAttribute(U"boostIncludeDir", ToUtf32(boostIncludeDir));
}

void SetBoostLibDir(sngxml::dom::Document* configDocument, const std::string& boostLibDir)
{
    configDocument->DocumentElement()->SetAttribute(U"boostLibDir", ToUtf32(boostLibDir));
}

void SetVCVarsFilePath(sngxml::dom::Document* configDocument, const std::string& vcVarsFilePath)
{
    configDocument->DocumentElement()->SetAttribute(U"vcVarsFilePath", ToUtf32(vcVarsFilePath));
}

void SaveConfiguration(sngxml::dom::Document* configDocument)
{
    std::ofstream configFile(ConfigFilePath());
    CodeFormatter formatter(configFile);
    configDocument->Write(formatter);
}

} } // namespace wingstall::config
