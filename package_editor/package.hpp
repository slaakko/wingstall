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
#include <soulng/rex/Context.hpp>
#include <boost/uuid/uuid.hpp>

namespace wingstall { namespace package_editor {

class PackageContentView;
class PackageExplorer;

class Properties : public Node
{
public:
    Properties();
    void SetAttributes(sngxml::dom::Element* packageElement);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    void SetSourceRootDir(const std::string& sourceRootDir_);
    const std::string& SourceRootDir() const { return sourceRootDir; }
    void SetTargetRootDir(const std::string& targetRootDir_);
    const std::string& TargetRootDir() const { return targetRootDir; }
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
    std::string ImageName() const override { return "package.properties.bitmap"; }
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
    Package(const std::string& packageXMLFilePath);
    Package(const std::string& packageXMLFilePath, sngxml::dom::Element* root);
    bool CanSave() const override;
    void Save() override;
    bool CanBuild() const override;
    void Build() override;
    bool CanClose() const override { return true; }
    void Close() override;
    bool IsDirty() const { return dirty; }
    void SetDirty();
    bool Building() const { return building; }
    void ResetBuilding();
    sngxml::dom::Element* ToXml() const;
    std::string PathComponentName() const override { return "Package"; }
    const std::string& FilePath() const { return filePath; }
    std::string BinDirectoryPath() const;
    Properties* GetProperties() const { return properties.get(); }
    Components* GetComponents() const { return components.get(); }
    Environment* GetEnvironment() const { return environment.get(); }
    Links* GetLinks() const { return links.get(); }
    EngineVariables* GetEngineVariables() const { return engineVariables.get(); }
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList)  override;
    Package* GetPackage() const override { return const_cast<Package*>(this); }
    PackageContentView* View() const { return view; }
    void SetView(PackageContentView* view_) { view = view_; }
    PackageExplorer* Explorer() const { return explorer; }
    void SetExplorer(PackageExplorer* explorer_) { explorer = explorer_; }
    soulng::rex::Context& GetContext() { return context; }
private:
    std::string filePath;
    std::unique_ptr<Properties> properties;
    std::unique_ptr<Components> components;
    std::unique_ptr<Environment> environment;
    std::unique_ptr<Links> links;
    std::unique_ptr<EngineVariables> engineVariables;
    PackageContentView* view;
    PackageExplorer* explorer;
    soulng::rex::Context context;
    bool dirty;
    bool building;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PACKAGE_INCLUDED
