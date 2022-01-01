// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingstall_config/build.props.hpp>
#include <wingstall_config/config.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <fstream>

namespace wingstall { namespace config {

using namespace soulng::util;
using namespace soulng::unicode;

void MakeBuildPropsFile(const std::string& boostIncludeDir, const std::string& boostLibDir)
{
    std::string configDir = GetFullPath(WingstallConfigDir());
    std::string buildPropsFilePath = Path::Combine(configDir, "build.props");
    sngxml::dom::Document buildPropsDoc;
    buildPropsDoc.SetXmlVersion(U"1.0");
    buildPropsDoc.SetXmlEncoding(U"utf-8");
    std::unique_ptr<sngxml::dom::Element> rootElement(new sngxml::dom::Element(U"Project"));
    rootElement->SetAttribute(U"ToolsVersion", U"4.0");
    rootElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");
    std::unique_ptr<sngxml::dom::Element> importGroupElement(new sngxml::dom::Element(U"ImportGroup"));
    importGroupElement->SetAttribute(U"Label", U"PropertySheets");
    rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importGroupElement.release()));

    std::unique_ptr<sngxml::dom::Element> userMactorsPropertyGroupElement(new sngxml::dom::Element(U"PropertyGroup"));
    userMactorsPropertyGroupElement->SetAttribute(U"Label", U"UserMacros");
    std::unique_ptr<sngxml::dom::Element> boostIncludeDirElement(new sngxml::dom::Element(U"BOOST_INCLUDE_DIR"));
    std::unique_ptr<sngxml::dom::Text> boostIncludeDirText(new sngxml::dom::Text(ToUtf32(boostIncludeDir)));
    boostIncludeDirElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostIncludeDirText.release()));
    userMactorsPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostIncludeDirElement.release()));
    std::unique_ptr<sngxml::dom::Element> boostLibDirElement(new sngxml::dom::Element(U"BOOST_LIB_DIR"));
    std::unique_ptr<sngxml::dom::Text> boostLibDirText(new sngxml::dom::Text(ToUtf32(boostLibDir)));
    boostLibDirElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostLibDirText.release()));
    userMactorsPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostLibDirElement.release()));
    rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(userMactorsPropertyGroupElement.release()));

    std::unique_ptr<sngxml::dom::Element> buildPropertyGroupElement(new sngxml::dom::Element(U"PropertyGroup"));
    std::unique_ptr<sngxml::dom::Element> propertySheetDisplayNameElement(new sngxml::dom::Element(U"_PropertySheetDisplayName"));
    std::unique_ptr<sngxml::dom::Text> propertySheetDisplayNameText(new sngxml::dom::Text(U"Build"));
    propertySheetDisplayNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(propertySheetDisplayNameText.release()));
    buildPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(propertySheetDisplayNameElement.release()));
    rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(buildPropertyGroupElement.release()));

    std::unique_ptr<sngxml::dom::Element> itemDefinitionGroupElement(new sngxml::dom::Element(U"ItemDefinitionGroup"));
    std::unique_ptr<sngxml::dom::Element> clCompileElement(new sngxml::dom::Element(U"ClCompile"));
    std::unique_ptr<sngxml::dom::Element> debugInformationFormatElement(new sngxml::dom::Element(U"DebugInformationFormat"));
    std::unique_ptr<sngxml::dom::Text> debugInformationFormatText(new sngxml::dom::Text(U"ProgramDatabase"));
    debugInformationFormatElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugInformationFormatText.release()));
    clCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugInformationFormatElement.release()));
    itemDefinitionGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileElement.release()));
    rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(itemDefinitionGroupElement.release()));

    std::unique_ptr<sngxml::dom::Element> itemGroupElement(new sngxml::dom::Element(U"ItemGroup"));

    std::unique_ptr<sngxml::dom::Element> boostIncludeDirBuildMacroElement(new sngxml::dom::Element(U"BuildMacro"));
    boostIncludeDirBuildMacroElement->SetAttribute(U"Include", U"BOOST_INCLUDE_DIR");
    std::unique_ptr<sngxml::dom::Element> boostIncludeDirValueElement(new sngxml::dom::Element(U"Value"));
    std::unique_ptr<sngxml::dom::Text> boostIncludeDirValueText(new sngxml::dom::Text(U"$(BOOST_INCLUDE_DIR)"));
    boostIncludeDirValueElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostIncludeDirValueText.release()));
    boostIncludeDirBuildMacroElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostIncludeDirValueElement.release()));
    itemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostIncludeDirBuildMacroElement.release()));

    std::unique_ptr<sngxml::dom::Element> boostLibDirBuildMacroElement(new sngxml::dom::Element(U"BuildMacro"));
    boostLibDirBuildMacroElement->SetAttribute(U"Include", U"BOOST_LIB_DIR");
    std::unique_ptr<sngxml::dom::Element> boostLibDirValueElement(new sngxml::dom::Element(U"Value"));
    std::unique_ptr<sngxml::dom::Text> boostLibDirValueText(new sngxml::dom::Text(U"$(BOOST_LIB_DIR)"));
    boostLibDirValueElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostLibDirValueText.release()));
    boostLibDirBuildMacroElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostLibDirValueElement.release()));
    itemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boostLibDirBuildMacroElement.release()));

    rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(itemGroupElement.release()));

    buildPropsDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement.release()));

    std::ofstream buildPropsNewFile(buildPropsFilePath + ".new");
    CodeFormatter newFormatter(buildPropsNewFile);
    newFormatter.SetIndentSize(2);
    buildPropsDoc.Write(newFormatter);

    std::ofstream buildPropsFile(buildPropsFilePath);
    CodeFormatter formatter(buildPropsFile);
    formatter.SetIndentSize(2);
    buildPropsDoc.Write(formatter);
}

} } // namespace wingstall::config
