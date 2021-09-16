// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/engine_variable.hpp>
#include <package_editor/package.hpp>
#include <wing/ImageList.hpp>
#include <wing/Shell.hpp>

namespace wingstall { namespace package_editor {

EngineVariables::EngineVariables() : Node(NodeKind::engineVariables, "Engine Variables")
{
    std::unique_ptr<EngineVariable> targetRootDir(new TargetRootDirVariable());
    targetRootDir->SetParent(this);
    engineVariables.push_back(std::move(targetRootDir));
    std::unique_ptr<EngineVariable> appName(new AppNameVariable());
    appName->SetParent(this);
    engineVariables.push_back(std::move(appName));
    std::unique_ptr<EngineVariable> appVersion(new AppVersionVariable());
    appVersion->SetParent(this);
    engineVariables.push_back(std::move(appVersion));
    std::unique_ptr<EngineVariable> publisher(new PublisherVariable());
    publisher->SetParent(this);
    engineVariables.push_back(std::move(publisher));
    std::unique_ptr<EngineVariable> startMenuProgramsFolder(new StartMenuProgramsFolderVariable());
    startMenuProgramsFolder->SetParent(this);
    engineVariables.push_back(std::move(startMenuProgramsFolder));
    std::unique_ptr<EngineVariable> desktopFolder(new DesktopFolderVariable());
    desktopFolder->SetParent(this);
    engineVariables.push_back(std::move(desktopFolder));
    std::unique_ptr<EngineVariable> programFilesDir(new ProgramFilesDirVariable());
    programFilesDir->SetParent(this);
    engineVariables.push_back(std::move(programFilesDir));
}

TreeViewNode* EngineVariables::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Engine Variables");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("engine.variables.bitmap"));
    }
    for (const auto& engineVariable : engineVariables)
    {
        node->AddChild(engineVariable->ToTreeViewNode(view));
    }
    return node;
}

Control* EngineVariables::CreateView(ImageList* imageList)
{
    std::unique_ptr<ListView> listView(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Name", 200);
    listView->AddColumn("Value", 200);
    for (const auto& engineVariable : engineVariables)
    {
        ListViewItem* item = listView->AddItem();
        engineVariable->SetData(item, imageList);
    }
    return listView.release();

}

EngineVariable::EngineVariable() : Node(NodeKind::engineVariable, std::string())
{
}

TreeViewNode* EngineVariable::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name() + "=" + Value());
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("engine.variable.bitmap"));
    }
    return node;
}

void EngineVariable::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(1, Value());
}

TargetRootDirVariable::TargetRootDirVariable()
{
    SetName("TARGET_ROOT_DIR");
}

std::string TargetRootDirVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->GetProperties()->TargetRootDir();
    }
    else
    {
        return std::string();
    }
}

AppNameVariable::AppNameVariable()
{
    SetName("APP_NAME");
}

std::string AppNameVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->GetProperties()->AppName();
    }
    else
    {
        return std::string();
    }
}

AppVersionVariable::AppVersionVariable()
{
    SetName("APP_VERSION");
}

std::string AppVersionVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->GetProperties()->Version();
    }
    else
    {
        return std::string();
    }
}

PublisherVariable::PublisherVariable()
{
    SetName("PUBLISHER");
}

std::string PublisherVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->GetProperties()->Publisher();
    }
    else
    {
        return std::string();
    }
}

StartMenuProgramsFolderVariable::StartMenuProgramsFolderVariable()
{
    SetName("START_MENU_PROGRAMS_FOLDER");
}

std::string StartMenuProgramsFolderVariable::Value() const
{
    try
    {
        return GetStartMenuProgramsFolderPath();
    }
    catch (const std::exception& ex)
    {
        return "could not get start menu programs folder path: " + std::string(ex.what());
    }
}

DesktopFolderVariable::DesktopFolderVariable()
{
    SetName("DESKTOP_FOLDER");
}

std::string DesktopFolderVariable::Value() const
{
    try
    {
        return GetDesktopFolderPath();
    }
    catch (const std::exception& ex)
    {
        return "could not get desktop folder path : " + std::string(ex.what());
    }
}

ProgramFilesDirVariable::ProgramFilesDirVariable()
{
    SetName("PROGRAM_FILES_DIR");
}

std::string ProgramFilesDirVariable::Value() const
{
    try
    {
        return GetProgramFilesDirectoryPath();
    }
    catch (const std::exception& ex)
    {
        return "could not get program files folder path: " + std::string(ex.what());
    }
}

} } // wingstall::package_editor
