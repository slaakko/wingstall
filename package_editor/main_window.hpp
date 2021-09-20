// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
#include <package_editor/package.hpp>
#include <package_editor/package_explorer.hpp>
#include <package_editor/package_content_view.hpp>
#include <wing/ImageList.hpp>
#include <wing/Menu.hpp>
#include <wing/Window.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

using ExitViewEvent = EventWithArgs<CancelArgs>;

class MainWindow : public Window
{
public:
    MainWindow();
    ~MainWindow();
    void AddListViewEventHandlers(ListView* listView);
    void AddTreeViewEventHandlers(TreeView* treeView);
    void ClearClickActions();
    std::vector<std::unique_ptr<ClickAction>>& ClickActions() { return clickActions; }
    ExitViewEvent& ExitView() { return exitView; }
protected:
    void OnKeyDown(KeyEventArgs& args) override;
    void MouseUpNotification(MouseEventArgs& args) override;
private:
    MenuItem* newPackageMenuItem;
    MenuItem* openPackageMenuItem;
    MenuItem* closePackageMenuItem;
    MenuItem* exitMenuItem;
    ExitViewEvent exitView;
    std::unique_ptr<Package> package;
    PackageExplorer* packageExplorer;
    PackageContentView* packageContentView;
    LogView* logView;
    ImageList imageList;
    bool showingDialog;
    std::vector<std::unique_ptr<ClickAction>> clickActions;
    void NewPackageClick();
    void OpenPackageClick();
    void ClosePackageClick();
    void ExitClick();
    void ListViewItemClick(ListViewItemEventArgs& args);
    void ListViewItemRightClick(ListViewItemEventArgs& args);
    void ListViewItemDoubleClick(ListViewItemEventArgs& args);
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_MAIN_WINDOW_INCLUDED
