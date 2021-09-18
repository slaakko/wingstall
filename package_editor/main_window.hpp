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

class MainWindow : public Window
{
public:
    MainWindow();
    void AddListViewEventHandlers(ListView* listView);
private:
    MenuItem* newPackageMenuItem;
    MenuItem* openPackageMenuItem;
    MenuItem* closePackageMenuItem;
    MenuItem* exitMenuItem;
    std::unique_ptr<Package> package;
    PackageExplorer* packageExplorer;
    PackageContentView* packageContentView;
    LogView* logView;
    ImageList imageList;
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
