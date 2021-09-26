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
#include <wing/ImageList.hpp>
#include <wing/Menu.hpp>
#include <wing/Window.hpp>
#include <wing/PathView.hpp>

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
protected:
    void OnKeyDown(KeyEventArgs& args) override;
    void MouseUpNotification(MouseEventArgs& args) override;
    void OnWindowClosing(CancelArgs& args) override;
private:
    void NewPackageClick();
    void OpenPackageClick();
    void ClosePackageClick();
    void SavePackageClick();
    void BuildPackageClick();
    void OpenBinFolderClick();
    void ExitClick();
    void EditSettingsClick();
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
    ToolButton* openBinFolderToolButton;
    MenuItem* newPackageMenuItem;
    MenuItem* openPackageMenuItem;
    MenuItem* savePackageMenuItem;
    MenuItem* buildPackageMenuItem;
    MenuItem* closePackageMenuItem;
    MenuItem* exitMenuItem;
    MenuItem* editSettingsMenuItem;
    MenuItem* openBinFolderMenuItem;
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
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
