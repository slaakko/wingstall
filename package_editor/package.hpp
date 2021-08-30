// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED
#include <package_editor/component.hpp>
#include <package_editor/environment.hpp>
#include <package_editor/links.hpp>
#include <package_editor/engine_variable.hpp>
#include <wingpackage/component.hpp>
#include <sngxml/dom/Element.hpp>
#include <boost/uuid/uuid.hpp>

namespace wingstall { namespace package_editor {

class Properties : public Node
{
public:
    Properties();
    TreeViewNode* ToTreeViewNode(TreeView* view);
    const std::string& TargetRootDir() const { return targetRootDir; }
    void SetSourceRootDir(const std::string& sourceRootDir_);
    const std::string& SourceRootDir() const { return sourceRootDir; }
    void SetTargetRootDir(const std::string& targetRootDir_);
    const std::string& AppName() const { return appName; }
    void SetAppName(const std::string& appName_);
    const std::string& Publisher() const { return publisher; }
    void SetPublisher(const std::string& publisher_);
    const std::string& Version() const { return version; }
    void SetVersion(const std::string& version_);
    wingstall::wingpackage::Compression GetCompression() const { return compression; }
    void SetCompression(wingstall::wingpackage::Compression compression_);
    const std::string& IconFilePath() const { return iconFilePath; }
    void SetIconFilePath(const std::string& iconFilePath_);
    bool IncludeUninstaller() const { return includeUninstaller; }
    void SetIncludeUninstaller(bool includeUninstaller_);
    const boost::uuids::uuid& Id() const { return id; }
    void SetId(const boost::uuids::uuid& id_);
private:
    std::string sourceRootDir;
    std::string targetRootDir;
    std::string appName;
    std::string publisher;
    std::string version;
    wingstall::wingpackage::Compression compression;
    std::string iconFilePath;
    bool includeUninstaller;
    boost::uuids::uuid id;
};

class Package : public Node
{
public:
    Package();
    Package(const std::string& packageXMLFilePath, sngxml::dom::Element* root);
    const std::string& FilePath() const { return filePath; }
    Properties* GetProperties() const { return properties.get(); }
    Components* GetComponents() const { return components.get(); }
    Environment* GetEnvironment() const { return environment.get(); }
    Links* GetLinks() const { return links.get(); }
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Package* GetPackage() const override { return const_cast<Package*>(this); }
private:
    std::string filePath;
    std::unique_ptr<Properties> properties;
    std::unique_ptr<Components> components;
    std::unique_ptr<Environment> environment;
    std::unique_ptr<Links> links;
    std::unique_ptr<EngineVariables> engineVariables;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED