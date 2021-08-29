// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED
#include <package_editor/component.hpp>
#include <package_editor/environment.hpp>
#include <package_editor/links.hpp>
#include <wingpackage/component.hpp>
#include <sngxml/dom/Element.hpp>
#include <boost/uuid/uuid.hpp>

namespace wingstall { namespace package_editor {

class Properties : public Node
{
public:
    Properties();
    TreeViewNode* ToTreeViewNode(TreeView* view);
    void SetSourceRootDir(const std::string& sourceRootDir_);
    void SetTargetRootDir(const std::string& targetRootDir_);
    void SetAppName(const std::string& appName_);
    void SetPublisher(const std::string& publisher_);
    void SetVersion(const std::string& version_);
    void SetCompression(wingstall::wingpackage::Compression compression_);
    void SetIconFilePath(const std::string& iconFilePath_);
    void SetIncludeUninstaller(bool includeUninstaller_);
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
private:
    std::string filePath;
    std::unique_ptr<Properties> properties;
    std::unique_ptr<Components> components;
    std::unique_ptr<Environment> environment;
    std::unique_ptr<Links> links;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED
