// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
#include <package_editor/package.hpp>
#include <package_editor/package_explorer.hpp>
#include <package_editor/package_content_view.hpp>
#include <package_editor/tool_bar.hpp>
#include <package_editor/build.hpp>
#include <wing/ImageList.hpp>
#include <wing/Menu.hpp>
#include <wing/Window.hpp>
#include <wing/PathView.hpp>
#include <wing/StatusBar.hpp>
#include <wing/ProgressBar.hpp>
#include <thread>

namespace wingstall { namespace package_editor {

using namespace wing;

class PathBar;

using ExitViewEvent = EventWithArgs<CancelArgs>;

std::string WingstallPackagesDir();

class MainWindow : public Window
{
public:
    MainWindow();
    ~MainWindow();
    PathBar* GetPathBar() const { return pathBar; }
    ImageList* GetImageList() const { return const_cast<ImageList*>(&imageList); }
    void AddListViewEventHandlers(ListView* listView);
    void AddTreeViewEventHandlers(TreeView* treeView);
    void ClearClickActions();
    std::vector<std::unique_ptr<ClickAction>>& ClickActions() { return clickActions; }
    ExitViewEvent& ExitView() { return exitView; }
    void ShowPackageFilePathStatusItems();
    void HidePackageFilePathStatusItems();
    void EnableSave();
    void DisableSave();
    void ShowPackageBuildProgressStatusItems();
    void HidePackageBuildProgressStatusItems();
    void StartBuild();
    void CancelBuildClick();
    void EndBuild();
    void ClosePackageClick();
    Package* GetPackage() const { return package.get(); }
    LogView* GetLog() const { return logView; }
    void SetBuildProgressPercent(int percent);
protected:
    void OnKeyDown(KeyEventArgs& args) override;
    void MouseUpNotification(MouseEventArgs& args) override;
    void OnWindowClosing(CancelArgs& args) override;
private:
    void NewPackageClick();
    void OpenPackageClick();
    void SavePackageClick();
    void BuildPackageClick();
    void OpenBinDirectoryClick();
    void ExitClick();
    void EditConfigurationClick();
    void HomepageClick();
    void LocalDocumentationClick();
    void AboutClick();
    void SetCommandStatus();
    void PackageExplorerSizeChanged();
    void PackageContentViewSizeChanged();
    void PathComponentSelected(PathComponentArgs& args);
    void ListViewItemClick(ListViewItemEventArgs& args);
    void ListViewItemRightClick(ListViewItemEventArgs& args);
    void ListViewItemDoubleClick(ListViewItemEventArgs& args);
    void ParentPathSelectorClick();
    ToolButton* saveToolButton;
    ToolButton* buildToolButton;
    ToolButton* cancelBuildToolButton;
    ToolButton* openBinDirectoryToolButton;
    MenuItem* newPackageMenuItem;
    MenuItem* openPackageMenuItem;
    MenuItem* savePackageMenuItem;
    MenuItem* buildPackageMenuItem;
    MenuItem* cancelBuildMenuItem;
    MenuItem* closePackageMenuItem;
    MenuItem* exitMenuItem;
    MenuItem* editConfigurationMenuItem;
    MenuItem* openBinDirectoryMenuItem;
    MenuItem* homepageMenuItem;
    MenuItem* localDocumentationMenuItem;
    MenuItem* aboutMenuItem;
    StatusBarTextItem* packageFilePathLabelStatusBarItem;
    StatusBarTextItem* packageFilePathStatusBarItem;
    StatusBarTextItem* packageBuildProgressLabelStatusBarItem;
    StatusBarControlItem* buildProgressStatusBarItem;
    ProgressBar* buildProgressBar;
    StatusBarTextItem* packageBuildProgressPerceStatusBarItem;
    ExitViewEvent exitView;
    std::unique_ptr<Package> package;
    PathBar* pathBar;
    PackageExplorer* packageExplorer;
    PackageContentView* packageContentView;
    LogView* logView;
    ImageList imageList;
    bool showingDialog;
    std::vector<std::unique_ptr<ClickAction>> clickActions;
    bool canceled;
    std::unique_ptr<BuildTask> buildTask;
    std::unique_ptr<std::thread> buildThread;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
