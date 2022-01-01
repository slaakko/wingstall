// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_PACKAGE_INCLUDED
#define WINGSTALL_WINGPACKAGE_PACKAGE_INCLUDED
#include <wingpackage/component.hpp>
#include <wingpackage/variable.hpp>
#include <wing/ManualResetEvent.hpp>
#include <sngxml/dom/Document.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;

class Component;
class File;
class PathMatcher;
class Environment;
class Links;
class Variables;

enum class DataSource : uint8_t
{
    file, memory
};

enum class Content : uint8_t
{
    none = 0,
    preinstall = 1 << 0,
    index = 1 << 1,
    data = 1 << 2,
    all = preinstall | index | data
};

inline Content operator|(Content left, Content right)
{
    return Content(uint8_t(left) | uint8_t(right));
}

inline Content operator&(Content left, Content right)
{
    return Content(uint8_t(left) & uint8_t(right));
}

inline Content operator~(Content operand)
{
    return Content(~uint8_t(operand));
}

enum class Action : int
{
    abortAction, continueAction
};

class AbortException : public std::runtime_error
{
public:
    AbortException();
};

enum class Status : int 
{
    idle, running, succeeded, failed, aborted
};

class PackageObserver
{
public:
    virtual ~PackageObserver();
    virtual void StatusChanged(Package* package) {}
    virtual void ComponentChanged(Package* package) {}
    virtual void FileChanged(Package* package) {}
    virtual void FileIndexChanged(Package* package) {}
    virtual void StreamPositionChanged(Package* package) {}
    virtual void LogError(Package* package, const std::string& error) {}
    virtual void FileContentPositionChanged(Package* package) {}
};

class PackageStreamObserver : public StreamObserver
{
public:
    PackageStreamObserver(Package* package_);
    void PositionChanged(Stream* stream) override;
private:
    Package* package;
};

class Package : public Node
{
public:
    Package();
    Package(const std::string& name_);
    Package(PathMatcher& pathMatcher, sngxml::dom::Document* doc);
    Package* GetPackage() const override { return const_cast<Package*>(this); }
    Status GetStatus() const { return status; }
    const std::string& GetStatusStr() const { return statusStr; }
    const std::string& GetErrorMessage() const { return errorMessage; }
    void SetStatus(Status status_, const std::string& statusStr_, const std::string& errorMessage_);
    Node* GetComponent() const { return component; }
    void SetComponent(Node* component_);
    File* GetFile() const { return file; }
    void SetFile(File* file_);
    void NotifyStreamPositionChanged();
    int64_t GetStreamPosition() const;
    void AddObserver(PackageObserver* observer);
    void RemoveObserver(PackageObserver* observer);
    const std::string& GetSourceRootDir() const override;
    const std::string& SourceRootDir() const { return sourceRootDir; }
    void SetSourceRootDir(const std::string& sourceRootDir_) { sourceRootDir = sourceRootDir_; }
    const std::string& PreinstallDir() const { return preinstallDir; }
    void SetPreinstallDir(const std::string& preinstallDir_) { preinstallDir = preinstallDir_; }
    const std::string& GetTargetRootDir() const override;
    const std::string& TargetRootDir() const { return targetRootDir; }
    void SetTargetRootDir(const std::string& targetRootDir_) { targetRootDir = targetRootDir_; }
    Compression GetCompression() const { return compression; }
    void SetCompression(Compression compression_) { compression = compression_; }
    const std::string& Version() const { return version; }
    void SetVersion(const std::string& version_);
    int MajorVersion() const;
    int MinorVersion() const;
    int Build() const;
    int BinaryVersion() const;
    const std::string& AppName() const { return appName; }
    void SetAppName(const std::string& appName_) { appName = appName_; }
    const std::string& Publisher() const { return publisher; }
    void SetPublisher(const std::string& publisher_) { publisher = publisher_; }
    const std::string& IconFilePath() const { return iconFilePath; }
    void SetIconFilePath(const std::string& iconFilePath_) { iconFilePath = iconFilePath_; }
    const boost::uuids::uuid& Id() const { return id; }
    void SetId(const boost::uuids::uuid& id_);
    std::string UninstallString() const;
    void SetPreinstallComponent(Component* preinstallComponent_);
    Component* GetPreinstallComponent() const;
    void SetInstallationComponent(Component* installationComponent_);
    Component* GetInstallationComponent() const;
    const std::vector<std::unique_ptr<Component>>& Components() const { return components; }
    void AddComponent(Component* component);
    Environment* GetEnvironment() const { return environment.get(); }
    void SetEnvironment(Environment* environment_);
    Links* GetLinks() const { return links.get(); }
    void SetLinks(Links* links_);
    Variables& GetVariables() { return variables; }
    void WriteIndex(const std::string& filePath);
    void ReadIndex(const std::string& filePath);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    sngxml::dom::Element* ToXml() const override;
    std::unique_ptr<sngxml::dom::Document> XmlDoc() const;
    std::unique_ptr<sngxml::dom::Document> InfoDoc() const;
    void WriteIndexToXmlFile(const std::string& xmlFilePath);
    void WriteInfoXmlFile(const std::string& xmlFilePath);
    void Create(const std::string& filePath, Content content);
    int64_t Size() const { return size; }
    std::string ExpandPath(const std::string& str) const;
    void Install(DataSource dataSource, const std::string& filePath, uint8_t* data, int64_t size, Content content);
    void Uninstall() override;
    void RunUninstallCommands();
    void RunUninstallCommand(const std::string& uninstallCommand);
    void ResetAction();
    void Interrupt();
    void CheckInterrupted();
    Action GetAction();
    void SetAction(Action action_);
    void LogError(const std::string& error);
    int FileCount() const { return fileCount; }
    void IncrementFileCount();
    int FileIndex() const { return  fileIndex; }
    void IncrementFileIndex();
    int64_t FileContentSize() const { return fileContentSize; }
    void IncrementFileContentSize(int64_t size);
    int64_t FileContentPosition() const { return fileContentPos; }
    void IncrementFileContentPosition(int64_t amount);
private:
    Streams GetReadBaseStream(DataSource dataSource, const std::string& filePath, uint8_t* data, int64_t size);
    void AddReadCompressionStreams(Streams& streams, Compression comp);
    Streams GetWriteStreams(const std::string& filePath);
    void NotifyStatusChanged();
    void NotifyComponentChanged();
    void NotifyFileChanged();
    void NotifyFileIndexChanged();
    void NotifyFileContentPositionChanged();
    Status status;
    std::string statusStr;
    std::string errorMessage;
    Node* component;
    File* file;
    Stream* stream;
    std::vector<PackageObserver*> observers;
    std::string sourceRootDir;
    std::string targetRootDir;
    std::string preinstallDir;
    Compression compression;
    std::string version;
    std::string appName;
    std::string publisher;
    std::string iconFilePath;
    bool includeUninstaller;
    boost::uuids::uuid id;
    std::vector<std::string> uninstallCommands;
    std::unique_ptr<Component> preinstallComponent;
    std::unique_ptr<Component> installationComponent;
    std::vector<std::unique_ptr<Component>> components;
    std::unique_ptr<Environment> environment;
    std::unique_ptr<Links> links;
    Variables variables;
    PackageStreamObserver streamObserver;
    int64_t size;
    int64_t uncompressedSize;
    int64_t streamStartPosition;
    bool interrupted;
    Action action;
    wing::ManualResetEvent actionEvent;
    int fileCount;
    int fileIndex;
    bool includeFileContent;
    int64_t fileContentSize;
    int64_t fileContentPos;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_PACKAGE_INCLUDED
