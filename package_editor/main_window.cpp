// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/main_window.hpp>
#include <wing/Dialog.hpp>
#include <wing/LogView.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <wing/SplitContainer.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;
using namespace soulng::util;

std::string WingstallPackagesDir()
{
    std::string packagesDir = GetFullPath(Path::Combine(WingstallRoot(), "packages"));
    boost::filesystem::create_directories(packagesDir);
    return packagesDir;
}

MainWindow::MainWindow() : Window(WindowCreateParams().Text("Wingstall Package Editor").WindowClassName("wingstall.package_editor.main_window")),
    newPackageMenuItem(nullptr),
    openPackageMenuItem(nullptr),
    closePackageMenuItem(nullptr),
    exitMenuItem(nullptr),
    packageExplorer(nullptr),
    packageContentView(nullptr),
    logView(nullptr),
    showingDialog(false)
{
    std::unique_ptr<MenuBar> menuBar(new MenuBar());

    std::unique_ptr<MenuItem> fileMenuItem(new MenuItem("&File"));

    std::unique_ptr<MenuItem> newPackageMenuItemPtr(new MenuItem("&New Package..."));
    newPackageMenuItem = newPackageMenuItemPtr.get();
    newPackageMenuItem->SetShortcut(Keys::controlModifier | Keys::n);
    newPackageMenuItem->Click().AddHandler(this, &MainWindow::NewPackageClick);
    fileMenuItem->AddMenuItem(newPackageMenuItemPtr.release());

    std::unique_ptr<MenuItem> openPackageMenuItemPtr(new MenuItem("&Open Package..."));
    openPackageMenuItem = openPackageMenuItemPtr.get();
    openPackageMenuItem->SetShortcut(Keys::controlModifier | Keys::o);
    openPackageMenuItem->Click().AddHandler(this, &MainWindow::OpenPackageClick);
    fileMenuItem->AddMenuItem(openPackageMenuItemPtr.release());

    std::unique_ptr<MenuItem> closePackageMenuItemPtr(new MenuItem("&Close Package"));
    closePackageMenuItem = closePackageMenuItemPtr.get();
    closePackageMenuItem->Click().AddHandler(this, &MainWindow::ClosePackageClick);
    fileMenuItem->AddMenuItem(closePackageMenuItemPtr.release());

    std::unique_ptr<MenuItem> exitMenuItemPtr(new MenuItem("E&xit"));
    exitMenuItem = exitMenuItemPtr.get();
    exitMenuItem->Click().AddHandler(this, &MainWindow::ExitClick);
    fileMenuItem->AddMenuItem(exitMenuItemPtr.release());

    menuBar->AddMenuItem(fileMenuItem.release());

    AddChild(menuBar.release());

    std::unique_ptr<SplitContainer> verticalSplitContainer(new SplitContainer(SplitContainerCreateParams(SplitterOrientation::vertical).SetDock(Dock::fill)));

    std::unique_ptr<SplitContainer> horizontalSplitContainer(new SplitContainer(SplitContainerCreateParams(SplitterOrientation::horizontal).SetDock(Dock::fill).SplitterDistance(500)));
    std::unique_ptr<PackageExplorer> packageExplorerPtr(new PackageExplorer(PackageExplorerCreateParams().SetDock(Dock::fill)));
    packageExplorer = packageExplorerPtr.get();
    packageExplorer->SetMainWindow(this);
    horizontalSplitContainer->Pane1Container()->AddChild(packageExplorerPtr.release());

    std::unique_ptr<PackageContentView> packageContentViewPtr(new PackageContentView(PackageContentViewCreateParams().SetDock(Dock::fill)));
    packageContentView = packageContentViewPtr.get();
    packageContentView->SetMainWindow(this);
    horizontalSplitContainer->Pane2Container()->AddChild(packageContentViewPtr.release());

    packageExplorer->SetContentView(packageContentView);
    
    verticalSplitContainer->Pane1Container()->AddChild(horizontalSplitContainer.release());

    std::unique_ptr<LogView> logViewPtr(new LogView(TextViewCreateParams().Defaults()));
    logView = logViewPtr.get();
    logView->SetFlag(ControlFlags::scrollSubject);
    logView->SetDoubleBuffered();
    std::unique_ptr<Control> paddedLogView(new PaddedControl(PaddedControlCreateParams(logViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableLogView(new ScrollableControl(ScrollableControlCreateParams(paddedLogView.release()).SetDock(Dock::fill)));
    Application::SetLogView(logView);
    verticalSplitContainer->Pane2Container()->AddChild(scrollableLogView.release());

    AddChild(verticalSplitContainer.release());

    imageList.AddImage("package.bitmap");
    imageList.AddImage("components.bitmap");
    imageList.AddImage("component.bitmap");
    imageList.AddImage("component.bitmap");
    imageList.AddImage("add.folder.bitmap");
    imageList.AddImage("add.file.bitmap");
    imageList.AddImage("delete.folder.bitmap");
    imageList.AddImage("delete.folder.cascade.bitmap");
    imageList.AddImage("delete.file.bitmap");
    imageList.AddImage("delete.file.cascade.bitmap");
    imageList.AddImage("folder.closed.bitmap");
    imageList.AddImage("folder.opened.bitmap");
    imageList.AddImage("file.bitmap");
    imageList.AddImage("rules.bitmap");
    imageList.AddImage("document.collection.bitmap");
    imageList.AddImage("package.properties.bitmap");
    imageList.AddImage("environment.bitmap");
    imageList.AddImage("environment.var.bitmap");
    imageList.AddImage("path.directory.bitmap");
    imageList.AddImage("links.bitmap");
    imageList.AddImage("linked.folder.closed.bitmap");
    imageList.AddImage("shortcut.bitmap");
    imageList.AddImage("engine.variables.bitmap");
    imageList.AddImage("engine.variable.bitmap");
    imageList.AddImage("xml.file.bitmap");

    packageExplorer->SetImageList(&imageList);
    packageContentView->SetImageList(&imageList);

}

MainWindow::~MainWindow()
{
    packageContentView->ViewContent(nullptr);
}

void MainWindow::AddListViewEventHandlers(ListView* listView)
{
    listView->ItemClick().AddHandler(this, &MainWindow::ListViewItemClick);
    listView->ItemRightClick().AddHandler(this, &MainWindow::ListViewItemRightClick);
    listView->ItemDoubleClick().AddHandler(this, &MainWindow::ListViewItemDoubleClick);
}

void MainWindow::AddTreeViewEventHandlers(TreeView* treeView)
{
    treeView->NodeClick().AddHandler(packageExplorer, &PackageExplorer::TreeViewNodeClick);
}

void MainWindow::ClearClickActions()
{
    clickActions.clear();
}

void MainWindow::OnKeyDown(KeyEventArgs& args)
{
    try
    {
        Window::OnKeyDown(args);
        if (!args.handled)
        {
            switch (args.key)
            {
                case Keys::escape:
                {
                    RemoveContextMenu();
                    args.handled = true;
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::MouseUpNotification(MouseEventArgs& args)
{
    try
    {
        if (args.buttons == MouseButtons::lbutton)
        {
            if (!showingDialog)
            {
                RemoveContextMenu();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ListViewItemClick(ListViewItemEventArgs& args)
{
    if (args.item)
    {
        ListView* view = args.item->View();
        if (view)
        {
            view->SetSelectedItem(args.item);
        }
    }
}

void MainWindow::ListViewItemRightClick(ListViewItemEventArgs& args)
{
    if (args.item)
    {
        ListView* view = args.view;
        if (view)
        {
            view->SetSelectedItem(args.item);
            void* data = args.item->Data();
            if (data)
            {
                Node* node = static_cast<Node*>(data);
                ClearClickActions();
                std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
                node->AddMenuItems(contextMenu.get(), clickActions, MenuItemsKind::allMenuItems);
                if (contextMenu->HasMenuItems())
                {
                    Point screenLoc = view->ClientToScreen(args.location);
                    ShowContextMenu(contextMenu.release(), screenLoc);
                }
            }
        }
    }
    else
    {
        Node* node = packageExplorer->SelectedNode();
        if (node)
        {
            ClearClickActions();
            std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
            node->AddMenuItems(contextMenu.get(), clickActions, MenuItemsKind::newMenuItems);
            if (contextMenu->HasMenuItems())
            {
                ListView* view = args.view;
                if (view)
                {
                    Point screenLoc = view->ClientToScreen(args.location);
                    ShowContextMenu(contextMenu.release(), screenLoc);
                }
            }
        }
    }
}

void MainWindow::ListViewItemDoubleClick(ListViewItemEventArgs& args)
{
    if (args.item && args.item->Data())
    {
        Node* node = static_cast<Node*>(args.item->Data());
        node->Open();
    }
}

void MainWindow::NewPackageClick()
{
    // todo
    try
    {

    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OpenPackageClick()
{
    try
    {
        bool cancel = false;
        CancelArgs cancelArgs(cancel);
        ExitView().Fire(cancelArgs);
        if (cancelArgs.cancel)
        {
            return;
        }
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Package Files (*.package.xml)", "*.package.xml"));
        descriptionFilterPairs.push_back(std::make_pair("XML Files (*.xml)", "*.xml"));
        descriptionFilterPairs.push_back(std::make_pair("All Files (*.*)", "*.*"));
        std::string initialDirectory = WingstallPackagesDir();
        std::string filePath;
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "xml", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            ClosePackageClick();
            std::string packageXMLFilePath = GetFullPath(filePath);
            std::unique_ptr<sngxml::dom::Document> packageDoc(sngxml::dom::ReadDocument(packageXMLFilePath));
            package.reset(new Package(packageXMLFilePath, packageDoc->DocumentElement()));
            package->SetView(packageContentView);
            package->SetExplorer(packageExplorer);
            packageExplorer->SetPackage(package.get());
            if (packageExplorer->SelectedNode())
            {
                packageExplorer->SelectedNode()->Open();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ClosePackageClick()
{
    
    try
    {
        // todo
        package.reset();
        packageExplorer->SetPackage(package.get());
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ExitClick()
{
    // todo
    try
    {
        Close();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

} } // wingstall::package_editor
