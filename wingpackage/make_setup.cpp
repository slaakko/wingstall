// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/make_setup.hpp>
#include <wingpackage/package.hpp>
#include <wingstall_config/config.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;
using namespace soulng::unicode;

void CoutLogger::WriteLine(const std::string& line) 
{
    std::cout << line << std::endl;
}

std::string BoostIncludeDir()
{
    try
    {
        std::unique_ptr<sngxml::dom::Document> configDoc = wingstall::config::ConfigurationDocument();
        if (configDoc)
        {
            sngxml::dom::Element* rootElement = configDoc->DocumentElement();
            std::u32string boostIncludeDirAttr = rootElement->GetAttribute(U"boostIncludeDir");
            if (!boostIncludeDirAttr.empty())
            {
                return ToUtf8(boostIncludeDirAttr);
            }
            throw std::runtime_error("'boostIncludeDir' attribute is empty or does not exist");
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "could not access configuration document '" + wingstall::config::ConfigFilePath() + "': " + ex.what() << std::endl;
    }
    return wingstall::config::DefaultBoostIncludeDir();
}

std::string BoostLibDir()
{
    try
    {
        std::unique_ptr<sngxml::dom::Document> configDoc = wingstall::config::ConfigurationDocument();
        if (configDoc)
        {
            sngxml::dom::Element* rootElement = configDoc->DocumentElement();
            std::u32string boosLibDirAttr = rootElement->GetAttribute(U"boostLibDir");
            if (!boosLibDirAttr.empty())
            {
                return ToUtf8(boosLibDirAttr);
            }
            throw std::runtime_error("'boostLibDir' attribute is empty or does not exist");
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "could not access configuration document '" + wingstall::config::ConfigFilePath() + "': " + ex.what() << std::endl;
    }
    return wingstall::config::DefaultBoostLibDir();
}

std::string VCPlatformToolset()
{
    try
    {
        std::unique_ptr<sngxml::dom::Document> configDoc = wingstall::config::ConfigurationDocument();
        if (configDoc)
        {
            sngxml::dom::Element* rootElement = configDoc->DocumentElement();
            std::u32string vcPlatformToolsetAttr = rootElement->GetAttribute(U"vcPlatformToolset");
            if (!vcPlatformToolsetAttr.empty())
            {
                return ToUtf8(vcPlatformToolsetAttr);
            }
            throw std::runtime_error("'vcPlatformToolset' attribute is empty or does not exist");
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "could not access configuration document '" + wingstall::config::ConfigFilePath() + "': " + ex.what() << std::endl;
    }
    return wingstall::config::DefaultVCPlatformToolset();
}

std::string SetupIconResourceName()
{
    return "setup_icon";
}

void MakeMainFile(const std::string& packageBinFilePath, bool verbose, Logger* logger);

void MakeDataFile(const std::string& packageBinFilePath, const std::string& appName, const std::string& appVersion, const std::string& compression, const std::string& defaultContainingDirPath,
    const std::string& installDirName, int64_t uncompressedPackageSize, bool verbose, Logger* logger);

void MakeProjectFile(const std::string& packageBinFilePath, bool verbose, Logger* logger);

void MakeSetup(const std::string& packageBinFilePath, bool verbose)
{
    CoutLogger coutLogger;
    MakeSetup(packageBinFilePath, verbose, &coutLogger);
}

void MakeSetup(const std::string& packageBinFilePath, bool verbose, Logger* logger)
{
    std::string packageInfoFilePath = Path::ChangeExtension(packageBinFilePath, ".info.xml");
    if (verbose && logger)
    {
        logger->WriteLine("> " + packageInfoFilePath);
    }
    std::unique_ptr<sngxml::dom::Document> packageInfoDoc = sngxml::dom::ReadDocument(packageInfoFilePath);
    std::unique_ptr<sngxml::xpath::XPathObject> packageInfoObject = sngxml::xpath::Evaluate(U"/packageInfo", packageInfoDoc.get());
    if (packageInfoObject)
    {
        if (packageInfoObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(packageInfoObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string appNameAttr = element->GetAttribute(U"appName");
                    if (appNameAttr.empty())
                    {
                        throw std::runtime_error("root element has no 'appName' attribute in package info document '" + packageInfoFilePath + "' line " +
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                    std::string appName = ToUtf8(appNameAttr);
                    std::u32string appVersionAttr = element->GetAttribute(U"appVersion");
                    if (appVersionAttr.empty())
                    {
                        throw std::runtime_error("root element has no 'appVersion' attribute in package info document '" + packageInfoFilePath + "' line " +
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                    std::string appVersion = ToUtf8(appVersionAttr);
                    std::u32string compressionAttr = element->GetAttribute(U"compression");
                    if (compressionAttr.empty())
                    {
                        throw std::runtime_error("root element has no 'compression' attribute in package info document '" + packageInfoFilePath + "' line " +
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                    std::string compression = ToUtf8(compressionAttr);
                    std::u32string defaultContainingDirPathAttr = element->GetAttribute(U"defaultContainingDirPath");
                    if (defaultContainingDirPathAttr.empty())
                    {
                        throw std::runtime_error("root element has no 'defaultContainingDirPath' attribute in package info document '" + packageInfoFilePath + "' line " +
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                    std::string defaultContainingDirPath = ToUtf8(defaultContainingDirPathAttr);
                    std::u32string installDirNameAttr = element->GetAttribute(U"installDirName");
                    if (installDirNameAttr.empty())
                    {
                        throw std::runtime_error("root element has no 'installDirName' attribute in package info document '" + packageInfoFilePath + "' line " +
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                    std::string installDirName = ToUtf8(installDirNameAttr);
                    std::u32string uncompressedPackageSizeAttr = element->GetAttribute(U"uncompressedPackageSize");
                    if (uncompressedPackageSizeAttr.empty())
                    {
                        throw std::runtime_error("root element has no 'uncompressedPackageSize' attribute in package info document '" + packageInfoFilePath + "' line " +
                            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                    }
                    int64_t uncompressedPackageSize = boost::lexical_cast<int64_t>(ToUtf8(uncompressedPackageSizeAttr));
                    MakeDataFile(packageBinFilePath, appName, appVersion, compression, defaultContainingDirPath, installDirName, uncompressedPackageSize, verbose, logger);
                    MakeMainFile(packageBinFilePath, verbose, logger);
                    MakeProjectFile(packageBinFilePath, verbose, logger);
                }
            }
            else
            {
                throw std::runtime_error("one 'packageInfo' element expected in package info document '" + packageInfoFilePath + "'");
            }
        }
    }
}

void MakeMainFile(const std::string& packageBinFilePath, bool verbose, Logger* logger)
{
    std::string mainFileBasePath = GetFullPath(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "main"));
    std::string directoryPath = Path::GetDirectoryName(mainFileBasePath);
    boost::system::error_code ec;
    boost::filesystem::create_directories(directoryPath, ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    std::string sourceFilePath = Path::ChangeExtension(mainFileBasePath, ".cpp");
    std::ofstream sourceFile(sourceFilePath);
    CodeFormatter sourceFormatter(sourceFile);
    sourceFormatter.WriteLine("#include <winggui/install_window.hpp>");
    sourceFormatter.WriteLine("#include \"data.hpp\"");
    sourceFormatter.WriteLine("#include <stdexcept>");
    sourceFormatter.WriteLine("#include <wing/BinaryResourcePtr.hpp>");
    sourceFormatter.WriteLine("#include <wing/InitDone.hpp>");
    sourceFormatter.WriteLine("#include <sngxml/xpath/InitDone.hpp>");
    sourceFormatter.WriteLine("#include <soulng/util/InitDone.hpp>");
    sourceFormatter.WriteLine("#include <soulng/util/Path.hpp>");
    sourceFormatter.WriteLine("#include <soulng/util/System.hpp>");
    sourceFormatter.WriteLine("#include <soulng/util/Unicode.hpp>");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("using namespace soulng::util;");
    sourceFormatter.WriteLine("using namespace soulng::unicode;");
    sourceFormatter.WriteLine("using namespace wing;");
    sourceFormatter.WriteLine("using namespace wingstall::winggui;");
    sourceFormatter.WriteLine("using namespace wingstall::wingpackage;");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("void InitApplication(HINSTANCE instance)");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("soulng::util::Init();");
    sourceFormatter.WriteLine("sngxml::xpath::Init();");
    sourceFormatter.WriteLine("wing::Init(instance);");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("try");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("InitApplication(instance);");
    sourceFormatter.WriteLine("std::string currentExecutableName = Path::GetFileName(GetFullPath(GetPathToExecutable()));");
    sourceFormatter.WriteLine("BinaryResourcePtr unicodeDBResource(currentExecutableName, setup::UnicodeDBResourceName());");
    sourceFormatter.WriteLine("CharacterTable::Instance().SetDeflateData(unicodeDBResource.Data(), unicodeDBResource.Size(), setup::UncompressedUnicodeDBSize());");
    sourceFormatter.WriteLine("BinaryResourcePtr packageResource(currentExecutableName, setup::PackageResourceName());");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::appName, new StringItem(setup::AppName()));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::appVersion, new StringItem(setup::AppVersion()));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::installDirName, new StringItem(setup::InstallDirName()));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::defaultContainingDirPath, new StringItem(setup::DefaultContainingDirPath()));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(setup::Compression())));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::dataSource, new IntegerItem(static_cast<int64_t>(DataSource::memory)));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::packageDataAddress, new IntegerItem(reinterpret_cast<int64_t>(packageResource.Data())));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::compressedPackageSize, new IntegerItem(packageResource.Size()));");
    sourceFormatter.WriteLine("SetInfoItem(InfoItemKind::uncompressedPackageSize, new IntegerItem(setup::UncompressedPackageSize()));");
    sourceFormatter.WriteLine("Icon& setupIcon = Application::GetResourceManager().GetIcon(setup::SetupIconResourceName());");
    sourceFormatter.WriteLine("Package package;");
    sourceFormatter.WriteLine("InstallWindow installWindow;");
    sourceFormatter.WriteLine("installWindow.SetPackage(&package);");
    sourceFormatter.WriteLine("installWindow.SetIcon(setupIcon);");
    sourceFormatter.WriteLine("installWindow.SetSmallIcon(setupIcon);");
    sourceFormatter.WriteLine("Application::Run(installWindow);");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine("catch (const std::exception& ex)");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("ShowErrorMessageBox(nullptr, ex.what());");
    sourceFormatter.WriteLine("return 1;");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine("return 0;");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();

    if (verbose && logger)
    {
        logger->WriteLine("==> " + sourceFilePath);
    }
}

void MakeDataFile(const std::string& packageBinFilePath, const std::string& appName, const std::string& appVersion, const std::string& compression, const std::string& defaultContainingDirPath,
    const std::string& installDirName, int64_t uncompressedPackageSize, bool verbose, Logger* logger)
{
    std::string wingstallRootDir = WingstallRoot();
    std::string compressedUnicodeDBFilePath = soulng::unicode::CharacterTable::Instance().DeflateFilePath();
    std::string setupIconFilePath = GetFullPath(Path::Combine(Path::Combine(wingstallRootDir, "icon"), "setup.ico"));
    std::string packageResourceName = Path::GetFileNameWithoutExtension(packageBinFilePath);
    std::string unicodeDBResourceName = "wng_unicode_db";
    int64_t uncompressedUnicodeDBSize = soulng::unicode::CharacterTable::Instance().GetUncompressedFileSize();

    std::string dataFileBasePath = GetFullPath(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "data"));
    std::string directoryPath = Path::GetDirectoryName(dataFileBasePath);
    boost::system::error_code ec;
    boost::filesystem::create_directories(directoryPath, ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    std::string resourceFilePath = GetFullPath(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "resource.rc"));
    std::ofstream resourceFile(resourceFilePath);
    CodeFormatter resourceFormatter(resourceFile);
    resourceFormatter.WriteLine(unicodeDBResourceName + " RCDATA \"" + StringStr(MakeNativePath(compressedUnicodeDBFilePath)) + "\"");
    resourceFormatter.WriteLine(SetupIconResourceName() + " ICON \"" + StringStr(MakeNativePath(setupIconFilePath)) + "\"");
    resourceFormatter.WriteLine(packageResourceName + " RCDATA \"" + StringStr(MakeNativePath(MakeRelativeDirPath(packageBinFilePath, directoryPath))) + "\"");

    std::string headerFilePath = Path::ChangeExtension(dataFileBasePath, ".hpp");
    std::string headerFileName = Path::GetFileName(headerFilePath);
    std::ofstream headerFile(headerFilePath);
    CodeFormatter headerFormatter(headerFile);
    std::string sourceFilePath = Path::ChangeExtension(dataFileBasePath, ".cpp");
    std::ofstream sourceFile(sourceFilePath);
    CodeFormatter sourceFormatter(sourceFile);
    int64_t size = boost::filesystem::file_size(packageBinFilePath);
    if (verbose && logger)
    {
        logger->WriteLine("> " + packageBinFilePath);
    }

    headerFormatter.WriteLine("#ifndef DATA_H");
    headerFormatter.WriteLine("#define DATA_H");
    headerFormatter.WriteLine("#include <string>");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("namespace setup {");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string PackageResourceName();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("int64_t UncompressedPackageSize();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string UnicodeDBResourceName();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("int64_t UncompressedUnicodeDBSize();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string SetupIconResourceName();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string AppName();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string AppVersion();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("int Compression();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string DefaultContainingDirPath();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("std::string InstallDirName();");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("} // setup");
    headerFormatter.WriteLine();
    headerFormatter.WriteLine("#endif // DATA_H");

    sourceFormatter.WriteLine("#include \"" + headerFileName + "\"");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("namespace setup {");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string PackageResourceName()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + packageResourceName + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("int64_t UncompressedPackageSize()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return " + std::to_string(uncompressedPackageSize) + ";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string UnicodeDBResourceName()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + unicodeDBResourceName + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("int64_t UncompressedUnicodeDBSize()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return " + std::to_string(uncompressedUnicodeDBSize) + ";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string SetupIconResourceName()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + SetupIconResourceName() + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string AppName()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + appName + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string AppVersion()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + appVersion + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("int Compression()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return " + std::to_string(static_cast<int>(wingstall::wingpackage::ParseCompressionStr(compression))) + "; // " + compression);
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string DefaultContainingDirPath()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + defaultContainingDirPath + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("std::string InstallDirName()");
    sourceFormatter.WriteLine("{");
    sourceFormatter.IncIndent();
    sourceFormatter.WriteLine("return \"" + installDirName + "\";");
    sourceFormatter.DecIndent();
    sourceFormatter.WriteLine("}");
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine();
    sourceFormatter.WriteLine("} // setup");
    sourceFormatter.WriteLine();

    if (verbose && logger)
    {
        logger->WriteLine("==> " + headerFilePath);
        logger->WriteLine("==> " + sourceFilePath);
        logger->WriteLine("==> " + resourceFilePath);
    }
}

void MakeProjectFile(const std::string& packageBinFilePath, bool verbose, Logger* logger)
{
    boost::uuids::uuid projectGuid = boost::uuids::random_generator()();
    std::string projectGuidStr = "{" + boost::lexical_cast<std::string>(projectGuid) + "}";

    std::string wingstallRootDir = WingstallRoot();
    std::string wingstallIncludeDir = wingstallRootDir;
    std::string wingstallLibDir = Path::Combine(wingstallRootDir, "lib");
    std::string projectFilePath = GetFullPath(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "setup.vcxproj"));
    std::string directoryPath = Path::GetDirectoryName(projectFilePath);
    boost::system::error_code ec;
    boost::filesystem::create_directories(directoryPath, ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    std::ofstream projectFile(projectFilePath);
    CodeFormatter formatter(projectFile);
    sngxml::dom::Document projectDoc;
    projectDoc.SetXmlEncoding(U"utf-8");
    projectDoc.SetXmlVersion(U"1.0");
    sngxml::dom::Element* projectElement = new sngxml::dom::Element(U"Project");
    projectElement->SetAttribute(U"DefaultTargets", U"Build");
    projectElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");

    sngxml::dom::Element* itemGroupElement = new sngxml::dom::Element(U"ItemGroup");
    itemGroupElement->SetAttribute(U"Label", U"ProjectConfigurations");

    sngxml::dom::Element* debugX64ConfigElement = new sngxml::dom::Element(U"ProjectConfiguration");
    debugX64ConfigElement->SetAttribute(U"Include", U"Debug|x64");
    sngxml::dom::Element* debugConfigElement = new sngxml::dom::Element(U"Configuration");
    debugX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigElement));
    sngxml::dom::Text* debugConfigText = new sngxml::dom::Text(U"Debug");
    debugConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigText));
    sngxml::dom::Element* debugPlatformElement = new sngxml::dom::Element(U"Platform");
    debugX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPlatformElement));
    sngxml::dom::Text* debugPlatformText = new sngxml::dom::Text(U"x64");
    debugPlatformElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPlatformText));

    sngxml::dom::Element* releaseX64ConfigElement = new sngxml::dom::Element(U"ProjectConfiguration");
    releaseX64ConfigElement->SetAttribute(U"Include", U"Release|x64");
    sngxml::dom::Element* releaseConfigElement = new sngxml::dom::Element(U"Configuration");
    releaseX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigElement));
    sngxml::dom::Text* releaseConfigText = new sngxml::dom::Text(U"Release");
    releaseConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigText));
    sngxml::dom::Element* releasePlatformElement = new sngxml::dom::Element(U"Platform");
    releaseX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePlatformElement));
    sngxml::dom::Text* releasePlatformText = new sngxml::dom::Text(U"x64");
    releasePlatformElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePlatformText));

    itemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigElement));
    itemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigElement));
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(itemGroupElement));

    // Globals PropertyGroup
    sngxml::dom::Element* globalsPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
    globalsPropertyGroup->SetAttribute(U"Label", U"Globals");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalsPropertyGroup));
    sngxml::dom::Element* vcProjectVersionElement = new sngxml::dom::Element(U"VCProjectVersion");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(vcProjectVersionElement));
    sngxml::dom::Text* projectVersionText = new sngxml::dom::Text(U"16.0");
    vcProjectVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectVersionText));
    sngxml::dom::Element* keywordElement = new sngxml::dom::Element(U"Keyword");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement));
    sngxml::dom::Text* keywordText = new sngxml::dom::Text(U"Win32Proj");
    keywordElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordText));

    sngxml::dom::Element* projectGuidElement = new sngxml::dom::Element(U"ProjectGuid");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidElement));
    sngxml::dom::Text* projectGuidText = new sngxml::dom::Text(ToUtf32(projectGuidStr));
    projectGuidElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidText));

    sngxml::dom::Element* rootNamespaceElement = new sngxml::dom::Element(U"RootNamespace");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceElement));
    sngxml::dom::Text* rootNamespaceText = new sngxml::dom::Text(U"setup");
    rootNamespaceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceText));

    sngxml::dom::Element* windowsTargetPlatformVersionElement = new sngxml::dom::Element(U"WindowsTargetPlatformVersion");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionElement));
    sngxml::dom::Text* windowsTargetPlatformVersionText = new sngxml::dom::Text(U"10.0");
    windowsTargetPlatformVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionText));

    // Import Project
    sngxml::dom::Element* importProjectElement = new sngxml::dom::Element(U"Import");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importProjectElement));
    importProjectElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.Default.props");

    // DebugX64Config PropertyGroup
    sngxml::dom::Element* debugX64ConfigPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigPropertyGroup));
    debugX64ConfigPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
    debugX64ConfigPropertyGroup->SetAttribute(U"Label", U"Configuration");

    sngxml::dom::Element* debugX64ConfigConfigurationTypeElement = new sngxml::dom::Element(U"ConfigurationType");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigConfigurationTypeElement));
    sngxml::dom::Text* debugX64ConfigConfigurationTypeText = new sngxml::dom::Text(U"Application");
    debugX64ConfigConfigurationTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigConfigurationTypeText));

    sngxml::dom::Element* debugX64ConfigUseDebugLibrariesElement = new sngxml::dom::Element(U"UseDebugLibraries");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigUseDebugLibrariesElement));
    sngxml::dom::Text* debugX64ConfigUseDebugLibrariesText = new sngxml::dom::Text(U"true");
    debugX64ConfigUseDebugLibrariesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigUseDebugLibrariesText));

    sngxml::dom::Element* debugX64ConfigPlatformToolsetElement = new sngxml::dom::Element(U"PlatformToolset");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigPlatformToolsetElement));
    sngxml::dom::Text* debugX64ConfigPlatformToolsetText = new sngxml::dom::Text(ToUtf32(VCPlatformToolset()));
    debugX64ConfigPlatformToolsetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigPlatformToolsetText));

    sngxml::dom::Element* debugX64ConfigCharacterSetElement = new sngxml::dom::Element(U"CharacterSet");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigCharacterSetElement));
    sngxml::dom::Text* debugX64ConfigCharacterSetText = new sngxml::dom::Text(U"Unicode");
    debugX64ConfigCharacterSetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigCharacterSetText));

    // ReleaseX64Config PropertyGroup
    sngxml::dom::Element* releaseX64ConfigPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigPropertyGroup));
    releaseX64ConfigPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
    releaseX64ConfigPropertyGroup->SetAttribute(U"Label", U"Configuration");

    sngxml::dom::Element* releaseX64ConfigConfigurationTypeElement = new sngxml::dom::Element(U"ConfigurationType");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigConfigurationTypeElement));
    sngxml::dom::Text* releaseX64ConfigConfigurationTypeText = new sngxml::dom::Text(U"Application");
    releaseX64ConfigConfigurationTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigConfigurationTypeText));

    sngxml::dom::Element* releaseX64ConfigUseDebugLibrariesElement = new sngxml::dom::Element(U"UseDebugLibraries");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigUseDebugLibrariesElement));
    sngxml::dom::Text* releaseX64ConfigUseDebugLibrariesText = new sngxml::dom::Text(U"false");
    releaseX64ConfigUseDebugLibrariesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigUseDebugLibrariesText));

    sngxml::dom::Element* releaseX64ConfigPlatformToolsetElement = new sngxml::dom::Element(U"PlatformToolset");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigPlatformToolsetElement));
    sngxml::dom::Text* releaseX64ConfigPlatformToolsetText = new sngxml::dom::Text(ToUtf32(VCPlatformToolset()));
    releaseX64ConfigPlatformToolsetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigPlatformToolsetText));

    sngxml::dom::Element* releaseX64ConfigWholeProgramOptimizationElement = new sngxml::dom::Element(U"WholeProgramOptimization");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigWholeProgramOptimizationElement));
    sngxml::dom::Text* releaseX64WholeProgramOptimizationText = new sngxml::dom::Text(U"true");
    releaseX64ConfigWholeProgramOptimizationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WholeProgramOptimizationText));

    sngxml::dom::Element* releaseX64ConfigCharacterSetElement = new sngxml::dom::Element(U"CharacterSet");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigCharacterSetElement));
    sngxml::dom::Text* releaseX64ConfigCharacterSetText = new sngxml::dom::Text(U"Unicode");
    releaseX64ConfigCharacterSetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigCharacterSetText));

    // Import Project2
    sngxml::dom::Element* importProject2Element = new sngxml::dom::Element(U"Import");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importProject2Element));
    importProject2Element->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.props");

    // ImportExtensionSettings
    sngxml::dom::Element* importExtensionSettingsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importExtensionSettingsElement));
    importExtensionSettingsElement->SetAttribute(U"Label", U"ExtensionSettings");

    // ImportShared
    sngxml::dom::Element* importSharedElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importSharedElement));
    importSharedElement->SetAttribute(U"Label", U"Shared");

    // Import x64/Debug PropertySheets
    sngxml::dom::Element* importX64DebugPropertySheetsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64DebugPropertySheetsElement));
    importX64DebugPropertySheetsElement->SetAttribute(U"Label", U"PropertySheets");
    importX64DebugPropertySheetsElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
    sngxml::dom::Element* importX64DebugPropertySheetsProjectElement = new sngxml::dom::Element(U"Import");
    importX64DebugPropertySheetsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64DebugPropertySheetsProjectElement));
    importX64DebugPropertySheetsProjectElement->SetAttribute(U"Project", U"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
    importX64DebugPropertySheetsProjectElement->SetAttribute(U"Condition", U"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
    importX64DebugPropertySheetsProjectElement->SetAttribute(U"Label", U"LocalAppDataPlatform");

    // Import x64/Release PropertySheets
    sngxml::dom::Element* importX64ReleasePropertySheetsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64ReleasePropertySheetsElement));
    importX64ReleasePropertySheetsElement->SetAttribute(U"Label", U"PropertySheets");
    importX64ReleasePropertySheetsElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
    sngxml::dom::Element* importX64ReleasePropertySheetsProjectElement = new sngxml::dom::Element(U"Import");
    importX64ReleasePropertySheetsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64ReleasePropertySheetsProjectElement));
    importX64ReleasePropertySheetsProjectElement->SetAttribute(U"Project", U"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
    importX64ReleasePropertySheetsProjectElement->SetAttribute(U"Condition", U"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
    importX64ReleasePropertySheetsProjectElement->SetAttribute(U"Label", U"LocalAppDataPlatform");

    // UserMacros
    sngxml::dom::Element* userMacrosElement = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(userMacrosElement));
    userMacrosElement->SetAttribute(U"Label", U"UserMacros");

    // LinkIncremental DebugX64
    sngxml::dom::Element* linkIncrementalDebugX64ConditionElement = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalDebugX64ConditionElement));
    linkIncrementalDebugX64ConditionElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
    sngxml::dom::Element* linkIncrementalDebugX64Element = new sngxml::dom::Element(U"LinkIncremental");
    linkIncrementalDebugX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalDebugX64Element));
    sngxml::dom::Text* linkIncrementalDebugX64ConditionText = new sngxml::dom::Text(U"true");
    linkIncrementalDebugX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalDebugX64ConditionText));
    sngxml::dom::Element* targetNameDebugX64Element = new sngxml::dom::Element(U"TargetName");
    linkIncrementalDebugX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameDebugX64Element));
    sngxml::dom::Text* targetNameDebugX64Text = new sngxml::dom::Text(U"setupd");
    targetNameDebugX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameDebugX64Text));

    // LinkIncremental ReleaseX64
    sngxml::dom::Element* linkIncrementalReleaseX64ConditionElement = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalReleaseX64ConditionElement));
    linkIncrementalReleaseX64ConditionElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
    sngxml::dom::Element* linkIncrementalReleaseX64Element = new sngxml::dom::Element(U"LinkIncremental");
    linkIncrementalReleaseX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalReleaseX64Element));
    sngxml::dom::Text* linkIncrementalReleaseX64ConditionText = new sngxml::dom::Text(U"false");
    linkIncrementalReleaseX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalReleaseX64ConditionText));
    sngxml::dom::Element* targetNameReleaseX64Element = new sngxml::dom::Element(U"TargetName");
    linkIncrementalReleaseX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameReleaseX64Element));
    sngxml::dom::Text* targetNameReleaseX64Text = new sngxml::dom::Text(U"setup");
    targetNameReleaseX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameReleaseX64Text));

    // ItemDefinitionGroupClCompile DebugX64

    sngxml::dom::Element* debugX64ItemDefinitionGroupCompileElement = new sngxml::dom::Element(U"ItemDefinitionGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ItemDefinitionGroupCompileElement));
    debugX64ItemDefinitionGroupCompileElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");

    sngxml::dom::Element* debugX64CompileElement = new sngxml::dom::Element(U"ClCompile");
    debugX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64CompileElement));

    sngxml::dom::Element* debugX64WarningLevelElement = new sngxml::dom::Element(U"WarningLevel");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningLevelElement));
    sngxml::dom::Text* debugX64WarningLevelText = new sngxml::dom::Text(U"Level3");
    debugX64WarningLevelElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningLevelText));

    sngxml::dom::Element* debugX64SDLCheckElement = new sngxml::dom::Element(U"SDLCheck");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SDLCheckElement));
    sngxml::dom::Text* debugX64SDLCheckText = new sngxml::dom::Text(U"true");
    debugX64SDLCheckElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SDLCheckText));

    sngxml::dom::Element* debugX64PreprocessorElement = new sngxml::dom::Element(U"PreprocessorDefinitions");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64PreprocessorElement));
    sngxml::dom::Text* debugX64PreprocessorText = new sngxml::dom::Text(U"_DEBUG;_WINDOWS;%(PreprocessorDefinitions)");
    debugX64PreprocessorElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64PreprocessorText));

    sngxml::dom::Element* debugX64ConformanceElement = new sngxml::dom::Element(U"ConformanceMode");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConformanceElement));
    sngxml::dom::Text* debugX64ConformanceText = new sngxml::dom::Text(U"true");
    debugX64ConformanceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConformanceText));

    sngxml::dom::Element* debugX64IncludeElement = new sngxml::dom::Element(U"AdditionalIncludeDirectories");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64IncludeElement));
    sngxml::dom::Text* debugX64IncludeText = new sngxml::dom::Text(U"..;" + ToUtf32(wingstallIncludeDir) + U";" + ToUtf32(BoostIncludeDir()));
    debugX64IncludeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64IncludeText));

    sngxml::dom::Element* debugX64FormatElement = new sngxml::dom::Element(U"DebugInformationFormat");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64FormatElement));
    sngxml::dom::Text* debugX64FormatText = new sngxml::dom::Text(U"ProgramDatabase");
    debugX64FormatElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64FormatText));

    sngxml::dom::Element* debugX64WarningsElement = new sngxml::dom::Element(U"DisableSpecificWarnings");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningsElement));
    sngxml::dom::Text* debugX64WarningsText = new sngxml::dom::Text(U"4251;4275;4244;4267");
    debugX64WarningsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningsText));

    sngxml::dom::Element* debugX64LinkElement = new sngxml::dom::Element(U"Link");
    debugX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64LinkElement));

    sngxml::dom::Element* debugX64SubSystemElement = new sngxml::dom::Element(U"SubSystem");
    debugX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SubSystemElement));
    sngxml::dom::Text* debugX64SubSystemText = new sngxml::dom::Text(U"Windows");
    debugX64SubSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SubSystemText));

    sngxml::dom::Element* debugX64DebugInfoElement = new sngxml::dom::Element(U"GenerateDebugInformation");
    debugX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64DebugInfoElement));
    sngxml::dom::Text* debugX64DebugInfoText = new sngxml::dom::Text(U"true");
    debugX64DebugInfoElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64DebugInfoText));

    sngxml::dom::Element* debugX64LibraryDirsElement = new sngxml::dom::Element(U"AdditionalLibraryDirectories");
    debugX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64LibraryDirsElement));
    sngxml::dom::Text* debugX64LibraryDirsText = new sngxml::dom::Text(U"$(OutDir);" + ToUtf32(wingstallLibDir) + U";" + ToUtf32(BoostLibDir()));
    debugX64LibraryDirsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64LibraryDirsText));

    sngxml::dom::Element* debugX64UACElement = new sngxml::dom::Element(U"UACExecutionLevel");
    debugX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64UACElement));
    sngxml::dom::Text* debugX64UACText = new sngxml::dom::Text(U"RequireAdministrator");
    debugX64UACElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64UACText));

    // ItemDefinitionGroupClCompile ReleaseX64

    sngxml::dom::Element* releaseX64ItemDefinitionGroupCompileElement = new sngxml::dom::Element(U"ItemDefinitionGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ItemDefinitionGroupCompileElement));
    releaseX64ItemDefinitionGroupCompileElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");

    sngxml::dom::Element* releaseX64CompileElement = new sngxml::dom::Element(U"ClCompile");
    releaseX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64CompileElement));

    sngxml::dom::Element* releaseX64WarningLevelElement = new sngxml::dom::Element(U"WarningLevel");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningLevelElement));
    sngxml::dom::Text* releaseX64WarningLevelText = new sngxml::dom::Text(U"Level3");
    releaseX64WarningLevelElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningLevelText));

    sngxml::dom::Element* releaseX64FunctionLevelLinkingElement = new sngxml::dom::Element(U"FunctionLevelLinking");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64FunctionLevelLinkingElement));
    sngxml::dom::Text* releaseX64FunctionLevelLinkingText = new sngxml::dom::Text(U"true");
    releaseX64FunctionLevelLinkingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64FunctionLevelLinkingText));

    sngxml::dom::Element* releaseX64IntrinsicElement = new sngxml::dom::Element(U"IntrinsicFunctions");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IntrinsicElement));
    sngxml::dom::Text* releaseX64IntrinsicsText = new sngxml::dom::Text(U"true");
    releaseX64IntrinsicElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IntrinsicsText));

    sngxml::dom::Element* releaseX64SDLCheckElement = new sngxml::dom::Element(U"SDLCheck");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SDLCheckElement));
    sngxml::dom::Text* releaseX64SDLCheckText = new sngxml::dom::Text(U"true");
    releaseX64SDLCheckElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SDLCheckText));

    sngxml::dom::Element* releaseX64PreprocessorElement = new sngxml::dom::Element(U"PreprocessorDefinitions");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64PreprocessorElement));
    sngxml::dom::Text* releaseX64PreprocessorText = new sngxml::dom::Text(U"NDEBUG;_WINDOWS;%(PreprocessorDefinitions)");
    releaseX64PreprocessorElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64PreprocessorText));

    sngxml::dom::Element* releaseX64ConformanceElement = new sngxml::dom::Element(U"ConformanceMode");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConformanceElement));
    sngxml::dom::Text* releaseX64ConformanceText = new sngxml::dom::Text(U"true");
    releaseX64ConformanceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConformanceText));

    sngxml::dom::Element* releaseX64IncludeElement = new sngxml::dom::Element(U"AdditionalIncludeDirectories");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IncludeElement));
    sngxml::dom::Text* releaseX64IncludeText = new sngxml::dom::Text(U"..;" + ToUtf32(wingstallIncludeDir) + U";" + ToUtf32(BoostIncludeDir()));
    releaseX64IncludeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IncludeText));

    sngxml::dom::Element* releaseX64WarningsElement = new sngxml::dom::Element(U"DisableSpecificWarnings");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningsElement));
    sngxml::dom::Text* releaseX64WarningsText = new sngxml::dom::Text(U"4251;4275;4244;4267");
    releaseX64WarningsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningsText));

    sngxml::dom::Element* releaseX64LinkElement = new sngxml::dom::Element(U"Link");
    releaseX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64LinkElement));

    sngxml::dom::Element* releaseX64SubSystemElement = new sngxml::dom::Element(U"SubSystem");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SubSystemElement));
    sngxml::dom::Text* releaseX64SubSystemText = new sngxml::dom::Text(U"Windows");
    releaseX64SubSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SubSystemText));

    sngxml::dom::Element* releaseX64EnableComDataFoldingElement = new sngxml::dom::Element(U"EnableCOMDATFolding");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64EnableComDataFoldingElement));
    sngxml::dom::Text* releaseX64EnableComDataFoldingText = new sngxml::dom::Text(U"true");
    releaseX64EnableComDataFoldingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64EnableComDataFoldingText));

    sngxml::dom::Element* releaseX64OptimizeReferencesElement = new sngxml::dom::Element(U"OptimizeReferences");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64OptimizeReferencesElement));
    sngxml::dom::Text* releaseX64OptimizeReferencesText = new sngxml::dom::Text(U"true");
    releaseX64OptimizeReferencesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64OptimizeReferencesText));

    sngxml::dom::Element* releaseX64DebugInfoElement = new sngxml::dom::Element(U"GenerateDebugInformation");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64DebugInfoElement));
    sngxml::dom::Text* releaseX64DebugInfoText = new sngxml::dom::Text(U"true");
    releaseX64DebugInfoElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64DebugInfoText));

    sngxml::dom::Element* releaseX64LibraryDirsElement = new sngxml::dom::Element(U"AdditionalLibraryDirectories");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64LibraryDirsElement));
    sngxml::dom::Text* releaseX64LibraryDirsText = new sngxml::dom::Text(U"$(OutDir);" + ToUtf32(wingstallLibDir) + U";" + ToUtf32(BoostLibDir()));
    releaseX64LibraryDirsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64LibraryDirsText));

    sngxml::dom::Element* releaseX64UACElement = new sngxml::dom::Element(U"UACExecutionLevel");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64UACElement));
    sngxml::dom::Text* releaseX64UACText = new sngxml::dom::Text(U"RequireAdministrator");
    releaseX64UACElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64UACText));

    sngxml::dom::Element* clCompileItemGroupElement = new sngxml::dom::Element(U"ItemGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileItemGroupElement));

    sngxml::dom::Element* clCompileDataEement = new sngxml::dom::Element(U"ClCompile");
    clCompileItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileDataEement));
    clCompileDataEement->SetAttribute(U"Include", U"data.cpp");

    sngxml::dom::Element* clCompileMainEement = new sngxml::dom::Element(U"ClCompile");
    clCompileItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileMainEement));
    clCompileMainEement->SetAttribute(U"Include", U"main.cpp");

    sngxml::dom::Element* clIncludeItemGroupElement = new sngxml::dom::Element(U"ItemGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clIncludeItemGroupElement));

    sngxml::dom::Element* clIncludeDataElement = new sngxml::dom::Element(U"ClInclude");
    clIncludeItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clIncludeDataElement));
    clIncludeDataElement->SetAttribute(U"Include", U"data.hpp");

    sngxml::dom::Element* resourceItemGroupElement = new sngxml::dom::Element(U"ItemGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(resourceItemGroupElement));

    sngxml::dom::Element* resourceCompileResourceEement = new sngxml::dom::Element(U"ResourceCompile");
    resourceItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(resourceCompileResourceEement));
    resourceCompileResourceEement->SetAttribute(U"Include", U"resource.rc");

    sngxml::dom::Element* importTargetsElement = new sngxml::dom::Element(U"Import");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importTargetsElement));
    importTargetsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.targets");

    sngxml::dom::Element* importExtensionTargetsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importExtensionTargetsElement));
    importExtensionTargetsElement->SetAttribute(U"Label", U"ExtensionTargets");

    projectDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement));
    projectDoc.Write(formatter);

    if (verbose && logger)
    {
        logger->WriteLine("==> " + projectFilePath);
    }
}

} } // namespace wingstall::wingpackage
