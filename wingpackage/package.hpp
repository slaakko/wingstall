// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_PACKAGE_INCLUDED
#define WINGSTALL_WINGPACKAGE_PACKAGE_INCLUDED
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

enum class Compression : uint8_t
{
    none, deflate, bzip2
};

std::string CompressionStr(Compression compression);
Compression ParseCompressionStr(const std::string& compressionStr);

enum class DataSource : uint8_t
{
    file, memory
};

enum class Content : uint8_t
{
    none = 0,
    index = 1 << 0,
    data = 1 << 1,
    all = index | data
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
    abortAction, rollbackAction, continueAction
};

class AbortException : public std::runtime_error
{
public:
    AbortException();
};

class RollbackException : public std::runtime_error
{
public:
    RollbackException();
};

enum class Status : int 
{
    idle, running, succeeded, failed, aborted, rollbacked
};

class PackageObserver
{
public:
    virtual ~PackageObserver();
    virtual void StatusChanged(Package* package) {}
    virtual void ComponentChanged(Package* package) {}
    virtual void FileChanged(Package* package) {}
    virtual void StreamPositionChanged(Package* package) {}
    virtual void LogError(Package* package, const std::string& error) {}
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
    Stream* GetStream() const { return stream; }
    void NotifyStreamPositionChanged();
    void AddObserver(PackageObserver* observer);
    void RemoveObserver(PackageObserver* observer);
    const std::string& GetSourceRootDir() const override;
    const std::string& SourceRootDir() const { return sourceRootDir; }
    void SetSourceRootDir(const std::string& sourceRootDir_) { sourceRootDir = sourceRootDir_; }
    const std::string& GetTargetRootDir() const override;
    const std::string& TargetRootDir() const { return targetRootDir; }
    void SetTargetRootDir(const std::string& targetRootDir_) { targetRootDir = targetRootDir_; }
    Compression GetCompression() const { return compression; }
    void SetCompression(Compression compression_) { compression = compression_; }
    const std::string& Version() const { return version; }
    void SetVersion(const std::string& version_);
    const std::string& AppName() const { return appName; }
    void SetAppName(const std::string& appName_) { appName = appName_; }
    const boost::uuids::uuid& Id() const { return id; }
    void SetId(const boost::uuids::uuid& id_);
    const std::vector<std::unique_ptr<Component>>& Components() const { return components; }
    void AddComponent(Component* component);
    Environment* GetEnvironment() const { return environment.get(); }
    void SetEnvironment(Environment* environment_);
    Links* GetLinks() const { return links.get(); }
    void SetLinks(Links* links_);
    Variables& GetVariables() { return variables; }
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
    void Install(Compression comp, const std::string& filePath, Content content);
    void Install(Compression comp, uint8_t* data, int64_t size, Content content);
    void Uninstall();
    void Rollback();
    void ResetAction();
    void Interrupt();
    void CheckInterrupted();
    Action GetAction();
    void SetAction(Action action_);
    void LogError(const std::string& error);
private:
    void NotifyStatusChanged();
    void NotifyComponentChanged();
    void NotifyFileChanged();
    Status status;
    std::string statusStr;
    std::string errorMessage;
    Node* component;
    File* file;
    Stream* stream;
    std::vector<PackageObserver*> observers;
    std::string sourceRootDir;
    std::string targetRootDir;
    Compression compression;
    std::string version;
    std::string appName;
    boost::uuids::uuid id;
    std::vector<std::unique_ptr<Component>> components;
    std::unique_ptr<Environment> environment;
    std::unique_ptr<Links> links;
    Variables variables;
    PackageStreamObserver streamObserver;
    int64_t size;
    bool interrupted;
    Action action;
    cmajor::wing::ManualResetEvent actionEvent;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_PACKAGE_INCLUDED
