// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/package.hpp>
#include <wingpackage/component.hpp>
#include <wingpackage/preinstall_component.hpp>
#include <wingpackage/uninstall_component.hpp>
#include <wingpackage/installation_component.hpp>
#include <wingpackage/path_matcher.hpp>
#include <wingpackage/environment.hpp>
#include <wingpackage/links.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <soulng/util/BZip2Stream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/DeflateStream.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/MemoryStream.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <thread>
#include <fstream>

namespace wingstall { namespace wingpackage {

using namespace soulng::unicode;

PackageObserver::~PackageObserver()
{
}

PackageStreamObserver::PackageStreamObserver(Package* package_) : package(package_)
{
}

void PackageStreamObserver::PositionChanged(Stream* stream)
{
    package->NotifyStreamPositionChanged();
}

std::string CompressionStr(Compression compression)
{
    switch (compression)
    {
        case Compression::none: return "none";
        case Compression::deflate: return "deflate";
        case Compression::bzip2: return "bzip2";
    }
    return std::string();
}

Compression ParseCompressionStr(const std::string& compressionStr)
{
    if (compressionStr == "none") return Compression::none;
    else if (compressionStr == "deflate") return Compression::deflate;
    else if (compressionStr == "bzip2") return Compression::bzip2;
    else throw std::runtime_error("invalid compression name");
}

AbortException::AbortException() : std::runtime_error("abort")
{
}

Package::Package() : 
    Node(NodeKind::package), id(boost::uuids::nil_uuid()), compression(Compression::none), component(), file(), stream(),
    streamObserver(this), size(0), interrupted(false), action(Action::continueAction), status(Status::idle), includeUninstaller(false),
    fileCount(0), fileIndex(0), includeFileContent(false), fileContentSize(0), fileContentPos(0)
{
    variables.SetParent(this);
    SetInstallationComponent(new InstallationComponent());
}

Package::Package(const std::string& name_) : 
    Node(NodeKind::package, name_), id(boost::uuids::nil_uuid()), compression(Compression::none), component(), file(), stream(),
    streamObserver(this), size(0), interrupted(false), action(Action::continueAction), status(Status::idle), includeUninstaller(false),
    fileCount(0), fileIndex(0), includeFileContent(false), fileContentSize(0), fileContentPos(0)
{
    variables.SetParent(this);
    SetInstallationComponent(new InstallationComponent());
}

Package::Package(PathMatcher& pathMatcher, sngxml::dom::Document* doc) : 
    Node(NodeKind::package), id(boost::uuids::nil_uuid()), compression(Compression::none), component(), file(), stream(),
    streamObserver(this), size(0), interrupted(false), action(Action::continueAction), status(Status::idle), includeUninstaller(false),
    fileCount(0), fileIndex(0), includeFileContent(false), fileContentSize(0), fileContentPos(0)
{
    variables.SetParent(this);
    std::unique_ptr<sngxml::xpath::XPathObject> packageObject = sngxml::xpath::Evaluate(U"/package", doc);
    if (packageObject)
    {
        if (packageObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(packageObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string nameAttr = element->GetAttribute(U"name");
                    if (!nameAttr.empty())
                    {
                        SetName(ToUtf8(nameAttr));
                    }
                    else
                    {
                        throw std::runtime_error("package element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
                    }
                    std::u32string sourceRootDirAttr = element->GetAttribute(U"sourceRootDir");
                    if (!sourceRootDirAttr.empty())
                    {
                        pathMatcher.SetSourceRootDir(ToUtf8(sourceRootDirAttr));
                        SetSourceRootDir(pathMatcher.SourceRootDir());
                    }
                    else
                    {
                        throw std::runtime_error("package element has no 'sourceRootDir' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
                    }
                    std::u32string targetRootDirAttr = element->GetAttribute(U"targetRootDir");
                    if (!targetRootDirAttr.empty())
                    {
                        SetTargetRootDir(ToUtf8(targetRootDirAttr));
                    }
                    std::u32string appNameAttr = element->GetAttribute(U"appName");
                    if (!appNameAttr.empty())
                    {
                        SetAppName(ToUtf8(appNameAttr));
                    }
                    std::u32string publisherAttr = element->GetAttribute(U"publisher");
                    if (!publisherAttr.empty())
                    {
                        SetPublisher(ToUtf8(publisherAttr));
                    }
                    std::u32string iconFilePathAttr = element->GetAttribute(U"iconFilePath");
                    if (!iconFilePathAttr.empty())
                    {
                        SetIconFilePath(ToUtf8(iconFilePathAttr));
                    }
                    std::u32string compressionAttr = element->GetAttribute(U"compression");
                    if (!compressionAttr.empty())
                    {
                        SetCompression(ParseCompressionStr(ToUtf8(compressionAttr)));
                    }
                    std::u32string versionAttr = element->GetAttribute(U"version");
                    if (!versionAttr.empty())
                    {
                        SetVersion(ToUtf8(versionAttr));
                    }
                    else
                    {
                        SetVersion("1.0.0");
                    }
                    std::u32string includeUninstallerAttr = element->GetAttribute(U"includeUninstaller");
                    if (!includeUninstallerAttr.empty())
                    {
                        try
                        {
                            includeUninstaller = ParseBool(ToUtf8(includeUninstallerAttr));
                        }
                        catch (const std::exception& ex)
                        {
                            throw std::runtime_error("could not parse 'includeUninstaller' attribute: " + std::string(ex.what()));
                        }
                    }
                    SetId(boost::uuids::random_generator()());
                }
                else
                {
                    throw std::runtime_error("one package element node expected in package XML document '" + pathMatcher.XmlFilePath() + "'");
                }
            }
            else
            {
                throw std::runtime_error("one package node expected in package XML document '" + pathMatcher.XmlFilePath() + "'");
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> preinstallObject = sngxml::xpath::Evaluate(U"/package/preinstall", doc);
    if (preinstallObject)
    {
        if (preinstallObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(preinstallObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    SetPreinstallComponent(new PreinstallComponent(pathMatcher, element));
                }
            }
            else if (n > 1)
            {
                throw std::runtime_error("at most one 'preinstall' element allowed in package XML document '" + pathMatcher.XmlFilePath() + "'");
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> componentObject = sngxml::xpath::Evaluate(U"/package/component", doc);
    if (componentObject)
    {
        if (componentObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(componentObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Component* component = new Component(pathMatcher, element);
                    AddComponent(component);
                }
            }
        }
    }
    if (includeUninstaller)
    {
        UninstallComponent* uninstallComponent = new UninstallComponent();
        AddComponent(uninstallComponent);
        uninstallComponent->Initialize();
    }
    std::unique_ptr<sngxml::xpath::XPathObject> environmentObject = sngxml::xpath::Evaluate(U"/package/environment", doc);
    if (environmentObject)
    {
        if (environmentObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(environmentObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    SetEnvironment(new Environment(pathMatcher, element));
                }
            }
            else if (n > 1)
            {
                throw std::runtime_error("at most one environment node expected in package XML document '" + pathMatcher.XmlFilePath() + "'");
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> linksObject = sngxml::xpath::Evaluate(U"/package/links", doc);
    if (linksObject)
    {
        if (linksObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(linksObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    SetLinks(new Links(pathMatcher, element));
                }
            }
            else if (n > 1)
            {
                throw std::runtime_error("at most one 'links' node expected in package XML document '" + pathMatcher.XmlFilePath() + "'");
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> uninstallObject = sngxml::xpath::Evaluate(U"package/uninstall/run", doc);
    if (uninstallObject)
    {
        if (uninstallObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(uninstallObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string commandAttr = element->GetAttribute(U"command");
                    if (!commandAttr.empty())
                    {
                        uninstallCommands.push_back(ToUtf8(commandAttr));
                    }
                    else
                    {
                        throw std::runtime_error("'uninstall/run' element does not have 'command' attribute in package XML document '" + pathMatcher.XmlFilePath() + "'");
                    }
                }
            }
        }
    }
    SetInstallationComponent(new InstallationComponent());
}

void Package::SetStatus(Status status_, const std::string& statusStr_, const std::string& errorMessage_)
{
    if (status != status_ || statusStr != statusStr_ || errorMessage != errorMessage_)
    {
        status = status_;
        statusStr = statusStr_;
        errorMessage = errorMessage_;
        NotifyStatusChanged();
    }
}

void Package::NotifyStatusChanged()
{
    for (PackageObserver* observer : observers)
    {
        observer->StatusChanged(this);
    }
}

void Package::SetComponent(Node* component_)
{
    if (component != component_)
    {
        component = component_;
        NotifyComponentChanged();
    }
}

void Package::NotifyComponentChanged()
{
    for (PackageObserver* observer : observers)
    {
        observer->ComponentChanged(this);
    }
}

void Package::SetFile(File* file_)
{
    if (file != file_)
    {
        file = file_;
        NotifyFileChanged();
    }
}

void Package::NotifyFileChanged()
{
    for (PackageObserver* observer : observers)
    {
        observer->FileChanged(this);
    }
}

void Package::NotifyFileIndexChanged()
{
    for (PackageObserver* observer : observers)
    {
        observer->FileIndexChanged(this);
    }
}

void Package::NotifyFileContentPositionChanged()
{
    for (PackageObserver* observer : observers)
    {
        observer->FileContentPositionChanged(this);
    }
}

void Package::NotifyStreamPositionChanged()
{
    for (PackageObserver* observer : observers)
    {
        observer->StreamPositionChanged(this);
    }
}

void Package::AddObserver(PackageObserver* observer)
{
    if (std::find(observers.begin(), observers.end(), observer) == observers.end())
    {
        observers.push_back(observer);
    }
}

void Package::RemoveObserver(PackageObserver* observer)
{
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

const std::string& Package::GetSourceRootDir() const 
{ 
    if (!SourceRootDir().empty())
    {
        return SourceRootDir();
    }
    else
    {
        throw std::runtime_error("source root directory not set");
    }
}

const std::string& Package::GetTargetRootDir() const
{ 
    if (!TargetRootDir().empty())
    {
        return TargetRootDir();
    }
    else
    {
        throw std::runtime_error("target root directory not set");
    }
}

void Package::SetVersion(const std::string& version_)
{
    version = version_;
}

int Package::MajorVersion() const
{
    if (!Version().empty())
    {
        std::vector<std::string> v = Split(Version(), '.');
        if (v.size() > 0)
        {
            return boost::lexical_cast<int>(v[0]);
        }
    }
    return -1;
}

int Package::MinorVersion() const
{
    if (!Version().empty())
    {
        std::vector<std::string> v = Split(Version(), '.');
        if (v.size() > 1)
        {
            return boost::lexical_cast<int>(v[1]);
        }
    }
    return -1;
}

int Package::Build() const
{
    if (!Version().empty())
    {
        std::vector<std::string> v = Split(Version(), '.');
        if (v.size() > 2)
        {
            return boost::lexical_cast<int>(v[2]);
        }
    }
    return -1;
}

int Package::BinaryVersion() const
{
    if (!Version().empty())
    {
        std::vector<std::string> v = Split(Version(), '.');
        int major = MajorVersion();
        if (major != -1)
        {
            int minor = MinorVersion();
            if (minor == -1)
            {
                minor = 0;
            }
            int build = Build();
            if (build == -1)
            {
                build = 0;
            }
            return
                static_cast<int>(static_cast<uint8_t>(major)) << static_cast<int>(3 * 8) |
                static_cast<int>(static_cast<uint8_t>(minor)) << static_cast<int>(2 * 8) |
                static_cast<int>(static_cast<uint16_t>(build));
        }
    }
    return -1;
}

void Package::SetId(const boost::uuids::uuid& id_)
{
    id = id_;
}

std::string Package::UninstallString() const
{
    if (includeUninstaller)
    {
        return MakeNativePath(Path::Combine(targetRootDir, "uninstall.exe"));
    }
    return std::string();
}

void Package::SetPreinstallComponent(Component* preinstallComponent_)
{
    preinstallComponent.reset(preinstallComponent_);
    preinstallComponent->SetParent(this);
}

Component* Package::GetPreinstallComponent() const
{
    return preinstallComponent.get();
}

void Package::SetInstallationComponent(Component* installationComponent_)
{
    installationComponent.reset(installationComponent_);
    installationComponent->SetParent(this);
}

Component* Package::GetInstallationComponent() const
{
    return installationComponent.get();
}

void Package::AddComponent(Component* component)
{
    component->SetParent(this);
    components.push_back(std::unique_ptr<Component>(component));
}

void Package::SetEnvironment(Environment* environment_)
{
    environment.reset(environment_);
    environment->SetParent(this);
}

void Package::SetLinks(Links* links_)
{
    links.reset(links_);
    links->SetParent(this);
}

void Package::WriteIndex(const std::string& filePath)
{
    Streams streams;
    streams.Add(new FileStream(filePath, OpenMode::write | OpenMode::binary));
    streams.Add(new BufferedStream(streams.Back()));
    BinaryStreamWriter writer(streams.Back());
    WriteIndex(writer);
}

void Package::ReadIndex(const std::string& filePath)
{
    Streams streams;
    streams.Add(new FileStream(filePath, OpenMode::read | OpenMode::binary));
    streams.Add(new BufferedStream(streams.Back()));
    BinaryStreamReader reader(streams.Back());
    ReadIndex(reader);
}

void Package::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(sourceRootDir);
    writer.Write(targetRootDir);
    writer.Write(uint8_t(compression));
    writer.Write(version);
    writer.Write(appName);
    writer.Write(publisher);
    writer.Write(iconFilePath);
    writer.Write(id);
    writer.Write(includeUninstaller);
    int32_t numComponents = components.size();
    writer.Write(numComponents);
    for (int32_t i = 0; i < numComponents; ++i)
    {
        Component* component = components[i].get();
        wingpackage::WriteIndex(component, writer);
    }
    bool hasEnvironment = environment != nullptr;
    writer.Write(hasEnvironment);
    if (hasEnvironment)
    {
        environment->WriteIndex(writer);
    }
    bool hasLinks = links != nullptr;
    writer.Write(hasLinks);
    if (hasLinks)
    {
        links->WriteIndex(writer);
    }
    int32_t numUninstallCommands = uninstallCommands.size();
    writer.Write(numUninstallCommands);
    for (int32_t i = 0; i < numUninstallCommands; ++i)
    {
        writer.Write(uninstallCommands[i]);
    }
}

void Package::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    SetComponent(this);
    CheckInterrupted();
    sourceRootDir = reader.ReadUtf8String();
    std::string packageTargetRootDir = reader.ReadUtf8String();
    if (targetRootDir.empty())
    {
        targetRootDir = packageTargetRootDir;
    }
    compression = Compression(reader.ReadByte());
    version = reader.ReadUtf8String();
    appName = reader.ReadUtf8String();
    publisher = reader.ReadUtf8String();
    iconFilePath = reader.ReadUtf8String();
    reader.ReadUuid(id);
    includeUninstaller = reader.ReadBool();
    int32_t numComponents = reader.ReadInt();
    for (int32_t i = 0; i < numComponents; ++i)
    {
        Component* component = BeginReadComponent(reader);
        AddComponent(component);
        component->ReadIndex(reader);
    }
    bool hasEnvironment = reader.ReadBool();
    if (hasEnvironment)
    {
        SetEnvironment(new Environment());
        environment->ReadIndex(reader);
    }
    bool hasLinks = reader.ReadBool();
    if (hasLinks)
    {
        SetLinks(new Links());
        links->ReadIndex(reader);
    }
    int32_t numUninstallCommands = reader.ReadInt();
    for (int32_t i = 0; i < numUninstallCommands; ++i)
    {
        std::string uninstallCommand = reader.ReadUtf8String();
        uninstallCommands.push_back(uninstallCommand);
    }
}

void Package::WriteData(BinaryStreamWriter& writer)
{
    for (const auto& component : components)
    {
        component->WriteData(writer);
    }
}

void Package::ReadData(BinaryStreamReader& reader)
{
    SetComponent(this);
    CheckInterrupted();
    for (const auto& component : components)
    {
        component->ReadData(reader);
    }
}

sngxml::dom::Element* Package::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"package");
    element->SetAttribute(U"name", ToUtf32(Name()));
    element->SetAttribute(U"appName", ToUtf32(appName));
    element->SetAttribute(U"publisher", ToUtf32(publisher));
    element->SetAttribute(U"iconFilePath", ToUtf32(iconFilePath));
    element->SetAttribute(U"sourceRootDir", ToUtf32(sourceRootDir));
    element->SetAttribute(U"targetRootDir", ToUtf32(targetRootDir));
    element->SetAttribute(U"compression", ToUtf32(CompressionStr(compression)));
    element->SetAttribute(U"version", ToUtf32(version));
    element->SetAttribute(U"id", ToUtf32(boost::lexical_cast<std::string>(id)));
    element->SetAttribute(U"includeUninstaller", ToUtf32(ToString(includeUninstaller)));
    for (const auto& component : components)
    {
        sngxml::dom::Element* child = component->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    if (environment != nullptr)
    {
        sngxml::dom::Element* child = environment->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    if (links != nullptr)
    {
        sngxml::dom::Element* child = links->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    sngxml::dom::Element* variablesElement = variables.ToXml();
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(variablesElement));
    return element;
}

std::unique_ptr<sngxml::dom::Document> Package::XmlDoc() const
{
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    doc->AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml()));
    return doc;
}

void Package::WriteIndexToXmlFile(const std::string& xmlFilePath)
{
    std::ofstream file(xmlFilePath);
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    std::unique_ptr<sngxml::dom::Document> doc = XmlDoc();
    doc->Write(formatter);
}

std::unique_ptr<sngxml::dom::Document> Package::InfoDoc() const
{
    sngxml::dom::Element* rootElement = new sngxml::dom::Element(U"packageInfo");
    rootElement->SetAttribute(U"appName", ToUtf32(appName));
    rootElement->SetAttribute(U"appVersion", ToUtf32(version));
    rootElement->SetAttribute(U"publisher", ToUtf32(publisher));
    rootElement->SetAttribute(U"iconFilePath", ToUtf32(iconFilePath));
    rootElement->SetAttribute(U"compression", ToUtf32(CompressionStr(compression)));
    std::string installDirName = Path::GetFileName(targetRootDir);
    rootElement->SetAttribute(U"installDirName", ToUtf32(installDirName));
    std::string defaultContainingDirPath = Path::GetDirectoryName(GetFullPath(targetRootDir));
    rootElement->SetAttribute(U"defaultContainingDirPath", ToUtf32(defaultContainingDirPath));
    rootElement->SetAttribute(U"uncompressedPackageSize", ToUtf32(std::to_string(size)));
    rootElement->SetAttribute(U"includeUninstaller", ToUtf32(ToString(includeUninstaller)));
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    doc->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement));
    return doc;
}

void Package::WriteInfoXmlFile(const std::string& xmlFilePath)
{
    std::ofstream file(xmlFilePath);
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    std::unique_ptr<sngxml::dom::Document> doc = InfoDoc();
    doc->Write(formatter);
}

void Package::Create(const std::string& filePath, Content content)
{
    if (content != Content::none)
    {
        Streams streams = GetWriteStreams(filePath);
        if (streams.Count() > 0)
        {
            includeFileContent = false;
            Stream* uncompressedStream = streams.Get(0);
            fileContentSize = 0;
            fileContentPos = 0;
            BinaryStreamWriter uncompressedStreamWriter(*uncompressedStream);
            uncompressedStreamWriter.Write(std::uint8_t(compression));
            uncompressedStreamWriter.Write(targetRootDir);
            if ((content & Content::preinstall) != Content::none)
            {
                bool hasPreinstallComponent = preinstallComponent != nullptr;
                uncompressedStreamWriter.Write(hasPreinstallComponent);
                if (hasPreinstallComponent)
                {
                    preinstallComponent->Write(streams);
                }
            }
            includeFileContent = true;
            BinaryStreamWriter writer(streams.Back());
            if ((content & Content::index) != Content::none)
            {
                WriteIndex(writer);
            }
            if ((content & Content::data) != Content::none)
            {
                WriteData(writer);
            }
            size = writer.Position();
            SetComponent(nullptr);
            SetFile(nullptr);
            SetStatus(Status::succeeded, "writing succeeded", std::string());
        }
    }
}

std::string Package::ExpandPath(const std::string& path) const
{
    return variables.ExpandPath(path);
}

void Package::Install(DataSource dataSource, const std::string& filePath, uint8_t* data, int64_t size, Content content)
{
    ResetAction();
    try
    {
        if (content != Content::none)
        {
            Streams streams = GetReadBaseStream(dataSource, filePath, data, size);
            if (streams.Count() > 0)
            {
                includeFileContent = false;
                Stream* uncompressedStream = streams.Get(0);
                BinaryStreamReader uncompressedStreamReader(*uncompressedStream);
                Compression packageCompression = static_cast<Compression>(uncompressedStreamReader.ReadByte());
                std::string packageTargetRootDir = uncompressedStreamReader.ReadUtf8String();
                if (targetRootDir.empty())
                {
                    SetTargetRootDir(packageTargetRootDir);
                }
                AddReadCompressionStreams(streams, packageCompression);
                if ((content & Content::preinstall) != Content::none)
                {
                    bool hasPreinstallComponent = uncompressedStreamReader.ReadBool();
                    if (hasPreinstallComponent)
                    {
                        SetPreinstallDir(GetFullPath(Path::Combine(GetTargetRootDir(), boost::lexical_cast<std::string>(boost::uuids::random_generator()()))));
                        SetStatus(Status::running, "checking prerequisites...", std::string());
                        SetPreinstallComponent(new PreinstallComponent());
                        preinstallComponent->Read(streams);
                        preinstallComponent->RunCommands();
                    }
                }
                fileContentSize = 0;
                fileContentPos = 0;
                includeFileContent = true;
                stream = &streams.Back();
                stream->AddObserver(&streamObserver);
                BinaryStreamReader reader(*stream);
                if ((content & Content::index) != Content::none)
                {
                    SetStatus(Status::running, "reading package index...", std::string());
                    ReadIndex(reader);
                }
                if ((content & Content::data) != Content::none)
                {
                    SetStatus(Status::running, "copying files...", std::string());
                    ReadData(reader);
                }
                if (environment)
                {
                    SetStatus(Status::running, "creating environment variables...", std::string());
                    environment->Install();
                }
                if (links)
                {
                    SetStatus(Status::running, "creating links...", std::string());
                    links->Install();
                }
                if (includeUninstaller)
                {
                    SetStatus(Status::running, "writing uninstall information file...", std::string());
                    std::string uninstallBinFilePath = GetFullPath(Path::Combine(GetTargetRootDir(), "uninstall.bin"));
                    WriteIndex(uninstallBinFilePath);
                }
                if (installationComponent != nullptr)
                {
                    SetStatus(Status::running, "creating installation registry information...", std::string());
                    installationComponent->CreateInstallationInfo();
                }
                SetComponent(nullptr);
                SetFile(nullptr);
                SetStatus(Status::succeeded, "installation succeeded", std::string());
                stream->RemoveObserver(&streamObserver);
            }
        }
    }
    catch (const AbortException&)
    {
        SetStatus(Status::aborted, "installation aborted", std::string());
    }
    catch (const std::exception& ex)
    {
        SetStatus(Status::failed, "installation failed", ex.what());
    }
}

void Package::Uninstall()
{
    ResetAction();
    try
    {
        Node::Uninstall();
        SetStatus(Status::running, "running uninstall commands...", std::string());
        RunUninstallCommands();
        if (links)
        {
            SetStatus(Status::running, "removing links...", std::string());
            links->Uninstall();
        }
        if (environment)
        {
            SetStatus(Status::running, "removing environment variables...", std::string());
            environment->Uninstall();
        }
        for (const auto& component : components)
        {
            SetStatus(Status::running, "removing files...", std::string());
            component->Uninstall();
        }
        if (installationComponent)
        {
            SetStatus(Status::running, "removing installation information from registry...", std::string());
            installationComponent->RemoveInstallationInfo();
        }
        SetComponent(nullptr);
        SetFile(nullptr);
        SetStatus(Status::succeeded, "uninstallation succceeded", std::string());
    }
    catch (const std::exception& ex)
    {
        SetStatus(Status::failed, "uninstallation failed", ex.what());
    }
}

void Package::RunUninstallCommands()
{
    int n = uninstallCommands.size();
    for (int i = 0; i < n; ++i)
    {
        RunUninstallCommand(uninstallCommands[i]);
    }
}

void Package::RunUninstallCommand(const std::string& uninstallCommand)
{
    try
    {
        Process process(uninstallCommand, Process::Redirections::none);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("uninstall action '" + uninstallCommand + "' returned exit code " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        LogError(ex.what());
    }
}

void Package::LogError(const std::string& error)
{
    for (PackageObserver* observer : observers)
    {
        observer->LogError(this, error);
    }
}

void Package::IncrementFileCount()
{
    ++fileCount;
}

void Package::IncrementFileIndex()
{
    ++fileIndex;
    NotifyFileIndexChanged();
}

void Package::IncrementFileContentSize(int64_t size)
{
    if (!includeFileContent) return;
    fileContentSize += size;
}

void Package::IncrementFileContentPosition(int64_t amount)
{
    if (!includeFileContent) return;
    fileContentPos += amount;
    NotifyFileContentPositionChanged();
}

Streams Package::GetReadBaseStream(DataSource dataSource, const std::string& filePath, uint8_t* data, int64_t size)
{
    Streams streams;
    if (dataSource == DataSource::memory)
    {
        streams.Add(new MemoryStream(data, size));
    }
    else if (dataSource == DataSource::file)
    {
        streams.Add(new FileStream(filePath, OpenMode::read | OpenMode::binary));
    }
    return streams;
}

void Package::AddReadCompressionStreams(Streams& streams, Compression comp)
{
    switch (comp)
    {
        case Compression::none:
        {
            break;
        }
        case Compression::deflate:
        {
            streams.Add(new DeflateStream(CompressionMode::decompress, streams.Back()));
            streams.Add(new BufferedStream(streams.Back()));
            break;
        }
        case Compression::bzip2:
        {
            streams.Add(new BZip2Stream(CompressionMode::decompress, streams.Back()));
            streams.Add(new BufferedStream(streams.Back()));
            break;
        }
    }
}

Streams Package::GetWriteStreams(const std::string& filePath)
{
    Streams streams;
    streams.Add(new FileStream(filePath, OpenMode::write | OpenMode::binary));
    switch (compression)
    {
        case Compression::none:
        {
            streams.Add(new BufferedStream(streams.Back()));
            break;
        }
        case Compression::deflate:
        {
            streams.Add(new BufferedStream(streams.Back()));
            streams.Add(new DeflateStream(CompressionMode::compress, streams.Back()));
            streams.Add(new BufferedStream(streams.Back()));
            break;
        }
        case Compression::bzip2:
        {
            streams.Add(new BufferedStream(streams.Back()));
            streams.Add(new BZip2Stream(CompressionMode::compress, streams.Back()));
            streams.Add(new BufferedStream(streams.Back()));
            break;
        }
    }
    return streams;
}

void Package::ResetAction()
{
    actionEvent.Reset();
    action = Action::continueAction;
    interrupted = false;
}

void Package::Interrupt()
{
    actionEvent.Reset();
    interrupted = true;
}

void Package::CheckInterrupted()
{
    if (interrupted)
    {
        Action action = GetAction();
        switch (action)
        {
            case Action::continueAction:
            {
                interrupted = false;
                return;
            }
            case Action::abortAction:
            {
                interrupted = false;
                throw AbortException();
            }
        }
        interrupted = false;
    }
}

Action Package::GetAction()
{
    actionEvent.WaitFor();
    return action;
}

void Package::SetAction(Action action_)
{
    action = action_;
    actionEvent.Set();
}

} } // namespace wingstall::wingpackage
