// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/main_window.hpp>
#include <package_editor/new_package_dialog.hpp>
#include <package_editor/path_bar.hpp>
#include <package_editor/about_dialog.hpp>
#include <package_editor/edit_configuration_dialog.hpp>
#include <package_editor/configuration.hpp>
#include <wingstall_config/config.hpp>
#include <wingstall_config/build.props.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/Dialog.hpp>
#include <wing/LogView.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <wing/MessageBox.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>

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
    showingDialog(false),
    canceled(false),
    setMaximizedSplitterDistance(false)
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

    std::unique_ptr<MenuItem> savePackageMenuItemPtr(new MenuItem("&Save Package"));
    savePackageMenuItem = savePackageMenuItemPtr.get();
    savePackageMenuItem->SetShortcut(Keys::controlModifier | Keys::s);
    savePackageMenuItem->Click().AddHandler(this, &MainWindow::SavePackageClick);
    savePackageMenuItem->Disable();
    fileMenuItem->AddMenuItem(savePackageMenuItemPtr.release());

    std::unique_ptr<MenuItem> closePackageMenuItemPtr(new MenuItem("&Close Package"));
    closePackageMenuItem = closePackageMenuItemPtr.get();
    closePackageMenuItem->Click().AddHandler(this, &MainWindow::ClosePackageClick);
    closePackageMenuItem->Disable();
    fileMenuItem->AddMenuItem(closePackageMenuItemPtr.release());

    std::unique_ptr<MenuItem> exitMenuItemPtr(new MenuItem("E&xit"));
    exitMenuItem = exitMenuItemPtr.get();
    exitMenuItem->Click().AddHandler(this, &MainWindow::ExitClick);
    fileMenuItem->AddMenuItem(exitMenuItemPtr.release());

    menuBar->AddMenuItem(fileMenuItem.release());

    std::unique_ptr<MenuItem> editMenuItem(new MenuItem("&Edit"));

    std::unique_ptr<MenuItem> editConfigurationMenuItemPtr(new MenuItem("&Configuration..."));
    editConfigurationMenuItem = editConfigurationMenuItemPtr.get();
    editConfigurationMenuItem->Click().AddHandler(this, &MainWindow::EditConfigurationClick);
    editMenuItem->AddMenuItem(editConfigurationMenuItemPtr.release());

    std::unique_ptr<MenuItem> resetMenuItemPtr(new MenuItem("&Reset UI values to defaults"));
    resetMenuItem = resetMenuItemPtr.get();
    resetMenuItem->Click().AddHandler(this, &MainWindow::ResetConfigClick);
    editMenuItem->AddMenuItem(resetMenuItemPtr.release());

    menuBar->AddMenuItem(editMenuItem.release());

    std::unique_ptr<MenuItem> buildMenuItem(new MenuItem("&Build"));

    std::unique_ptr<MenuItem> buildPackageMenuItemPtr(new MenuItem("&Build Package"));
    buildPackageMenuItem = buildPackageMenuItemPtr.get();
    buildPackageMenuItem->SetShortcut(Keys::f5);
    buildPackageMenuItem->Click().AddHandler(this, &MainWindow::BuildPackageClick);
    buildPackageMenuItem->Disable();
    buildMenuItem->AddMenuItem(buildPackageMenuItemPtr.release());

    std::unique_ptr<MenuItem> openBinDirectoryMenuItemPtr(new MenuItem("&Open Bin Directory"));
    openBinDirectoryMenuItem = openBinDirectoryMenuItemPtr.get();
    openBinDirectoryMenuItem->SetShortcut(Keys::f7);
    openBinDirectoryMenuItem->Click().AddHandler(this, &MainWindow::OpenBinDirectoryClick);
    openBinDirectoryMenuItem->Disable();
    buildMenuItem->AddMenuItem(openBinDirectoryMenuItemPtr.release());

    std::unique_ptr<MenuItem> cancelBuildMenuItemPtr(new MenuItem("&Cancel Build"));
    cancelBuildMenuItem = cancelBuildMenuItemPtr.get();
    cancelBuildMenuItem->SetShortcut(Keys::controlModifier | Keys::f4);
    cancelBuildMenuItem->Click().AddHandler(this, &MainWindow::CancelBuildClick);
    cancelBuildMenuItem->Disable();
    buildMenuItem->AddMenuItem(cancelBuildMenuItemPtr.release());

    menuBar->AddMenuItem(buildMenuItem.release());

    std::unique_ptr<MenuItem> helpMenuItem(new MenuItem("&Help"));

    std::unique_ptr<MenuItem> homepageMenuItemPtr(new MenuItem("&Homepage"));
    homepageMenuItem = homepageMenuItemPtr.get();
    homepageMenuItem->Click().AddHandler(this, &MainWindow::HomepageClick);
    helpMenuItem->AddMenuItem(homepageMenuItemPtr.release());

    std::unique_ptr<MenuItem> localDocumentationMenuItemPtr(new MenuItem("&Local Documentation"));
    localDocumentationMenuItem = localDocumentationMenuItemPtr.get();
    localDocumentationMenuItem->Click().AddHandler(this, &MainWindow::LocalDocumentationClick);
    helpMenuItem->AddMenuItem(localDocumentationMenuItemPtr.release());

    std::unique_ptr<MenuItem> aboutMenuItemPtr(new MenuItem("&About..."));
    aboutMenuItem = aboutMenuItemPtr.get();
    aboutMenuItem->Click().AddHandler(this, &MainWindow::AboutClick);
    helpMenuItem->AddMenuItem(aboutMenuItemPtr.release());

    menuBar->AddMenuItem(helpMenuItem.release());

    AddChild(menuBar.release());

    std::unique_ptr<ToolBar> toolBarPtr(MakeToolBar());
    ToolBar* toolBar = toolBarPtr.get();
    std::unique_ptr<Control> borderedToolBar(new BorderedControl(BorderedControlCreateParams(toolBarPtr.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(DefaultToolBarBorderColor()).FocusedSingleBorderColor(DefaultToolBarBorderColor()).SetSize(toolBar->GetSize()).SetDock(Dock::top)));

    std::unique_ptr<ToolButton> saveToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("save.bitmap").SetPadding(Padding(8, 8, 8, 8)).SetToolTip("Save Package (Ctrl+S)")));
    saveToolButton = saveToolButtonPtr.get();
    saveToolButton->Click().AddHandler(this, &MainWindow::SavePackageClick);
    saveToolButton->Disable();
    toolBar->AddToolButton(saveToolButtonPtr.release());

    std::unique_ptr<ToolButton> buildToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("build.bitmap").SetPadding(Padding(6, 6, 6, 6)).SetToolTip("Build Package (F5)")));
    buildToolButton = buildToolButtonPtr.get();
    buildToolButton->Click().AddHandler(this, &MainWindow::BuildPackageClick);
    buildToolButton->Disable();
    toolBar->AddToolButton(buildToolButtonPtr.release());

    std::unique_ptr<ToolButton> cancelBuildToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cancel.build.bitmap").SetPadding(Padding(6, 6, 6, 6)).SetToolTip("Cancel Build (Ctrl+F4)")));
    cancelBuildToolButton = cancelBuildToolButtonPtr.get();
    cancelBuildToolButton->Click().AddHandler(this, &MainWindow::CancelBuildClick);
    cancelBuildToolButton->Disable();
    toolBar->AddToolButton(cancelBuildToolButtonPtr.release());

    std::unique_ptr<ToolButton> openBinDirectoryToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("folder.opened.bitmap").SetPadding(Padding(6, 6, 6, 6)).SetToolTip("Open Bin Directory (F7)")));
    openBinDirectoryToolButton = openBinDirectoryToolButtonPtr.get();
    openBinDirectoryToolButton->Click().AddHandler(this, &MainWindow::OpenBinDirectoryClick);
    openBinDirectoryToolButton->Disable();
    toolBar->AddToolButton(openBinDirectoryToolButtonPtr.release());

    AddChild(borderedToolBar.release());

    std::unique_ptr<PathBar> pathBarPtr(new PathBar(PathBarCreateParams(&imageList).Defaults()));
    pathBar = pathBarPtr.get();
    pathBar->GetParentPathSelector()->Click().AddHandler(this, &MainWindow::ParentPathSelectorClick);
    pathBar->GetPathView()->PathComponentSelected().AddHandler(this, &MainWindow::PathComponentSelected);
    std::unique_ptr<BorderedControl> borderedPathBar(new BorderedControl(BorderedControlCreateParams(pathBarPtr.release()).SetSize(Size(0, 32)).SetDock(Dock::top).
        NormalSingleBorderColor(DefaultPathBarFrameColor())));
    AddChild(borderedPathBar.release());

    std::unique_ptr<SplitContainer> verticalSplitContainerPtr(new SplitContainer(SplitContainerCreateParams(SplitterOrientation::vertical).SetDock(Dock::fill)));
    verticalSplitContainer = verticalSplitContainerPtr.get();

    std::unique_ptr<SplitContainer> horizontalSplitContainerPtr(new SplitContainer(SplitContainerCreateParams(SplitterOrientation::horizontal).SetDock(Dock::fill).SplitterDistance(500)));
    horizontalSplitContainer = horizontalSplitContainerPtr.get();
    std::unique_ptr<PackageExplorer> packageExplorerPtr(new PackageExplorer(PackageExplorerCreateParams().SetDock(Dock::fill)));
    packageExplorer = packageExplorerPtr.get();
    packageExplorer->SetMainWindow(this);
    horizontalSplitContainer->Pane1Container()->AddChild(packageExplorerPtr.release());
    packageExplorer->SizeChanged().AddHandler(this, &MainWindow::PackageExplorerSizeChanged);

    std::unique_ptr<PackageContentView> packageContentViewPtr(new PackageContentView(PackageContentViewCreateParams().SetDock(Dock::fill)));
    packageContentView = packageContentViewPtr.get();
    packageContentView->SetMainWindow(this);
    packageContentView->SizeChanged().AddHandler(this, &MainWindow::PackageContentViewSizeChanged);
    horizontalSplitContainer->Pane2Container()->AddChild(packageContentViewPtr.release());

    packageExplorer->SetContentView(packageContentView);
    
    verticalSplitContainer->Pane1Container()->AddChild(horizontalSplitContainerPtr.release());

    std::unique_ptr<LogView> logViewPtr(new LogView(TextViewCreateParams().Defaults()));
    logView = logViewPtr.get();
    logView->SetFlag(ControlFlags::scrollSubject);
    logView->SetDoubleBuffered();
    std::unique_ptr<Control> paddedLogView(new PaddedControl(PaddedControlCreateParams(logViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableLogView(new ScrollableControl(ScrollableControlCreateParams(paddedLogView.release()).SetDock(Dock::fill)));
    Application::SetLogView(logView);
    verticalSplitContainer->Pane2Container()->AddChild(scrollableLogView.release());

    AddChild(verticalSplitContainerPtr.release());

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
    imageList.AddDisabledImage("folder.closed.bitmap");
    imageList.AddImage("folder.opened.bitmap");
    imageList.AddImage("file.bitmap");
    imageList.AddDisabledImage("file.bitmap");
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
    imageList.AddImage("up.arrow.bitmap");

    packageExplorer->SetImageList(&imageList);
    packageContentView->SetImageList(&imageList);

    std::unique_ptr<StatusBar> statusBarPtr(new StatusBar(StatusBarCreateParams().Defaults()));
    StatusBar* statusBar = statusBarPtr.get();
    statusBar->SetDoubleBuffered();

    std::unique_ptr<StatusBarTextItem> packageFilePathLabelStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().Text("File Path:")));
    packageFilePathLabelStatusBarItem = packageFilePathLabelStatusBarItemPtr.get();
    packageFilePathLabelStatusBarItem->SetVisible(false);
    statusBar->AddItem(packageFilePathLabelStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> packageFilePathStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().BorderStyle(StatusBarItemBorderStyle::sunken)));
    packageFilePathStatusBarItem = packageFilePathStatusBarItemPtr.get();
    packageFilePathStatusBarItem->SetVisible(false);
    statusBar->AddItem(packageFilePathStatusBarItemPtr.release());

    std::unique_ptr<StatusBarTextItem> packageBuildProgressLabelStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().Text("Build Progress:")));
    packageBuildProgressLabelStatusBarItem = packageBuildProgressLabelStatusBarItemPtr.get();
    packageBuildProgressLabelStatusBarItem->SetVisible(false);
    statusBar->AddItem(packageBuildProgressLabelStatusBarItemPtr.release());
    std::unique_ptr<ProgressBar> progressBarPtr(new ProgressBar(ProgressBarCreateParams().Defaults()));
    buildProgressBar = progressBarPtr.get();
    buildProgressBar->SetDoubleBuffered();
    buildProgressBar->SetSize(Size(300, 24));
    buildProgressBar->Hide();
    std::unique_ptr<StatusBarControlItem> buildProgressStatusBarItemPtr(new StatusBarControlItem(StatusBarControlItemCreateParams(progressBarPtr.release()).Defaults()));
    buildProgressStatusBarItem = buildProgressStatusBarItemPtr.get();
    buildProgressStatusBarItem->SetVisible(false);
    statusBar->AddItem(buildProgressStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> packageBuildProgressPerceStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().BorderStyle(StatusBarItemBorderStyle::sunken).MaxTextLength(3)));
    packageBuildProgressPerceStatusBarItem = packageBuildProgressPerceStatusBarItemPtr.get();
    packageBuildProgressPerceStatusBarItem->SetVisible(false);
    statusBar->AddItem(packageBuildProgressPerceStatusBarItemPtr.release());

    AddChild(statusBarPtr.release());

    SetCommandStatus();

    LoadConfigurationSettings();

    SetTimer(configurationSaveTimerId, configurationSavePeriod);
}

MainWindow::~MainWindow()
{
    if (IsConfigurationChanged())
    {
        SaveConfiguration();
    }
    if (buildThread)
    {
        if (buildThread->joinable())
        {
            buildThread->join();
        }
    }
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

void MainWindow::ShowPackageFilePathStatusItems()
{
    packageFilePathLabelStatusBarItem->SetVisible(true);
    packageFilePathStatusBarItem->SetVisible(true);
}

void MainWindow::HidePackageFilePathStatusItems()
{
    packageFilePathLabelStatusBarItem->SetVisible(false);
    packageFilePathLabelStatusBarItem->SetVisible(false);
}

void MainWindow::EnableSave()
{
    saveToolButton->Enable();
    savePackageMenuItem->Enable();
}

void MainWindow::DisableSave()
{
    saveToolButton->Disable();
    savePackageMenuItem->Disable();
}

void MainWindow::ShowPackageBuildProgressStatusItems()
{
    packageBuildProgressLabelStatusBarItem->SetVisible(true);
    buildProgressStatusBarItem->SetVisible(true);
    buildProgressBar->Show();
    packageBuildProgressPerceStatusBarItem->SetVisible(true);
}

void MainWindow::HidePackageBuildProgressStatusItems()
{
    packageBuildProgressLabelStatusBarItem->SetVisible(false);
    buildProgressStatusBarItem->SetVisible(false);
    buildProgressBar->Hide();
    packageBuildProgressPerceStatusBarItem->SetVisible(false);
}

void MainWindow::StartBuild()
{
    if (buildThread)
    {
        if (buildThread->joinable())
        {
            buildThread->join();
        }
    }
    if (package->IsDirty())
    {
        package->Save();
    }
    ShowPackageBuildProgressStatusItems();
    buildTask.reset(new BuildTask(this));
    buildThread.reset(new std::thread(RunBuildTask, buildTask.get()));
}

void MainWindow::CancelBuildClick()
{
    if (buildTask)
    {
        buildTask->Cancel();
    }
}

void MainWindow::EndBuild()
{
    HidePackageBuildProgressStatusItems();
    package->ResetBuilding();
    SetCommandStatus();
}

void MainWindow::OnTimer(TimerEventArgs& args)
{
    if (args.timerId == configurationSaveTimerId)
    {
        SetWindowSettings();
        if (IsConfigurationChanged())
        {
            SaveConfiguration();
        }
    }
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

void MainWindow::OnWindowStateChanged()
{
    Window::OnWindowStateChanged();
    if (GetWindowState() == WindowState::normal)
    {
        if (IsNormalWindowSettingsDefined())
        {
            SetLocation(GetConfiguredWindowLocation());
            SetSize(GetConfiguredWindowSize());
            horizontalSplitContainer->SetSplitterDistance(GetConfiguredNormalHorizontalSplitterDistance());
            verticalSplitContainer->SetSplitterDistance(GetConfiguredNormalVerticalSplitterDistance());
        }
    }
    else if (GetWindowState() == WindowState::maximized)
    {
        setMaximizedSplitterDistance = true;
    }
}

void MainWindow::OnSizeChanged()
{
    Window::OnSizeChanged();
    if (GetWindowState() == WindowState::maximized && setMaximizedSplitterDistance)
    {
        setMaximizedSplitterDistance = false;
        if (IsMaximizedWindowSettingsDefined())
        {
            horizontalSplitContainer->SetSplitterDistance(GetConfiguredMaximizedHorizontalSplitterDistance());
            verticalSplitContainer->SetSplitterDistance(GetConfiguredMaximizedVerticalSplitterDistance());
        }
    }
}

void MainWindow::OnWindowClosing(CancelArgs& args)
{
    ClosePackageClick();
    if (canceled)
    {
        canceled = false;
        args.cancel = true;
    }
}

void MainWindow::PackageExplorerSizeChanged()
{
    Size packageExplorerSize = packageExplorer->GetSize();
    pathBar->SetDirectoryPathViewWidth(packageExplorerSize.Width);
}

void MainWindow::PackageContentViewSizeChanged()
{
    Size packageContentViewSize = packageContentView->GetSize();
    pathBar->SetPathViewMaxWidth(packageContentViewSize.Width);
}

void MainWindow::PathComponentSelected(PathComponentArgs& args)
{
    PathComponent* pathComponent = args.pathComponent;
    if (pathComponent)
    {
        void* data = pathComponent->Data();
        if (data)
        {
            Node* node = static_cast<Node*>(data);
            node->Open();
        }
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
                    view->TranslateContentLocation(args.location);
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
                    view->TranslateContentLocation(args.location); 
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
        node->ExecuteDefaultAction();
    }
}

void MainWindow::ParentPathSelectorClick()
{
    Node* selectedNode = packageExplorer->SelectedNode();
    if (selectedNode)
    {
        Node* parent = selectedNode->Parent();
        if (parent)
        {
            parent->Open();
        }
    }
}

void MainWindow::LoadConfigurationSettings() 
{
    LoadConfiguration();
    SetWindowState(GetConfiguredWindowState());
    if (GetWindowState() == WindowState::normal)
    {
        if (IsNormalWindowSettingsDefined())
        {
            SetLocation(GetConfiguredWindowLocation());
            SetSize(GetConfiguredWindowSize());
            horizontalSplitContainer->SetSplitterDistance(GetConfiguredNormalHorizontalSplitterDistance());
            verticalSplitContainer->SetSplitterDistance(GetConfiguredNormalVerticalSplitterDistance());
        }
    }
    else if (GetWindowState() == WindowState::maximized)
    {
        setMaximizedSplitterDistance = true;
    }
}

void MainWindow::SetWindowSettings()
{
    WindowState windowState = GetWindowState();
    wingstall::package_editor::SetWindowState(windowState);
    if (windowState == WindowState::normal)
    {
        Point location = Location();
        SetWindowLocation(location);
        Size size = GetSize();
        SetWindowSize(size);
        int horizontalSplitterDistance = horizontalSplitContainer->SplitterDistance();
        SetNormalHorizontalSplitterDistance(horizontalSplitterDistance);
        int verticalSplitterDistance = verticalSplitContainer->SplitterDistance();
        SetNormalVerticalSplitterDistance(verticalSplitterDistance);
        SetNormalWindowSettingsDefined();
    }
    else if (windowState == WindowState::maximized)
    {
        int horizontalSplitterDistance = horizontalSplitContainer->SplitterDistance();
        SetMaximizedHorizontalSplitterDistance(horizontalSplitterDistance);
        int verticalSplitterDistance = verticalSplitContainer->SplitterDistance();
        SetMaximizedVerticalSplitterDistance(verticalSplitterDistance);
        SetMaximizedWindowSettingsDefined();
    }
}

void MainWindow::NewPackageClick()
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
        NewPackageDialog dialog;
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            ClosePackageClick();
            if (canceled)
            {
                canceled = false;
                return;
            }
            package.reset(new Package(dialog.GetPackageFilePath()));
            packageFilePathStatusBarItem->SetText(dialog.GetPackageFilePath());
            ShowPackageFilePathStatusItems();
            package->SetName(dialog.GetPackageName());
            package->SetContentView(packageContentView);
            package->SetExplorer(packageExplorer);
            packageExplorer->SetPackage(package.get());
            package->Open();
            package->GetProperties()->SetSourceRootDir(GetFullPath("C:/"));
            package->GetProperties()->SetTargetRootDir("C:/" + dialog.GetPackageName());
            package->GetProperties()->SetAppName(dialog.GetPackageName());
            package->GetProperties()->SetVersion("1.0");
            package->GetProperties()->SetId(boost::uuids::random_generator()());
            package->GetEngineVariables()->Fetch();
            package->SetDirty();
            SetCommandStatus();
        }
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
            if (canceled)
            {
                canceled = false;
                return;
            }
            std::string packageXMLFilePath = GetFullPath(filePath);
            std::unique_ptr<sngxml::dom::Document> packageDoc(sngxml::dom::ReadDocument(packageXMLFilePath));
            package.reset(new Package(packageXMLFilePath, packageDoc->DocumentElement()));
            packageFilePathStatusBarItem->SetText(packageXMLFilePath);
            ShowPackageFilePathStatusItems();
            package->SetContentView(packageContentView);
            package->SetExplorer(packageExplorer);
            packageExplorer->SetPackage(package.get());
            package->Open();
            SetCommandStatus();
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
        bool cancel = false;
        CancelArgs cancelArgs(cancel);
        ExitView().Fire(cancelArgs);
        if (cancelArgs.cancel)
        {
            canceled = true;
            return;
        }
        if (package)
        {
            if (package->IsDirty())
            {
                MessageBoxResult result = wing::MessageBox::Show("Save package?", "Package '" + package->Name() + "' has been modified", this, MB_YESNOCANCEL);
                if (result == MessageBoxResult::cancel)
                {
                    canceled = true;
                    return;
                }
                else if (result == MessageBoxResult::yes)
                {
                    package->Save();
                }
            }
        }
        package.reset();
        packageFilePathStatusBarItem->SetText(std::string());
        HidePackageFilePathStatusItems();
        pathBar->SetCurrentNode(nullptr);
        pathBar->SetDirectoryPath(std::string());
        packageExplorer->SetPackage(package.get());
        packageContentView->ViewContent(nullptr);
        SetCommandStatus();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
    canceled = false;
}

void MainWindow::SetBuildProgressPercent(int percent)
{
    buildProgressBar->SetProgressPercent(percent);
    packageBuildProgressPerceStatusBarItem->SetText(std::to_string(percent) + "%");
}

void MainWindow::ListViewColumnWidthChanged(ListViewColumnEventArgs& args)
{
    ListView* listView = args.view;
    void* data = listView->GetData();
    if (data)
    {
        Node* node = static_cast<Node*>(data);
        ListViewColumn* column = args.column;
        if (column)
        {
            View& view = GetConfiguredViewSettings().GetView(node->ViewName());
            ColumnWidth& columnWidth = view.GetColumnWidth(column->Name());
            columnWidth.Set(column->Width());
        }
    }
}

void MainWindow::SavePackageClick()
{
    try
    {
        if (package)
        {
            package->Save();
        }
        SetCommandStatus();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::BuildPackageClick()
{
    try
    {
        package->Build();
        SetCommandStatus();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OpenBinDirectoryClick()
{
    try
    {
        std::string binDirectoryFilePath = package->BinDirectoryPath();
#pragma warning(disable:4311)
#pragma warning(disable:4302)
        if (reinterpret_cast<int>(ShellExecuteA(Handle(), "open", binDirectoryFilePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL)) < 32)
        {
            throw std::runtime_error("shell execute failed");
        }
#pragma warning(default:4311)
#pragma warning(default:4302)

    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ExitClick()
{
    try
    {
        ClosePackageClick();
        if (canceled)
        {
            canceled = false;
            return;
        }
        Close();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::EditConfigurationClick()
{
    try
    {
        std::unique_ptr<sngxml::dom::Document> configDoc = wingstall::config::ConfigurationDocument();
        std::string boostIncludeDir = MakeNativePath(wingstall::config::BoostIncludeDir(configDoc.get()));
        std::string boostLibDir = MakeNativePath(wingstall::config::BoostLibDir(configDoc.get()));
        std::string vcVarsPath = MakeNativePath(wingstall::config::VCVarsFilePath(configDoc.get()));
        EditConfigurationDialog dialog(boostIncludeDir, boostLibDir, vcVarsPath);
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            dialog.GetData(boostIncludeDir, boostLibDir, vcVarsPath);
            wingstall::config::SetBoostIncludeDir(configDoc.get(), boostIncludeDir);
            wingstall::config::SetBoostLibDir(configDoc.get(), boostLibDir);
            wingstall::config::SetVCVarsFilePath(configDoc.get(), vcVarsPath);
            wingstall::config::SaveConfiguration(configDoc.get());
            wingstall::config::MakeBuildPropsFile(boostIncludeDir, boostLibDir);
            ShowMessageBox(Handle(), "Information", "Configuration saved to '" + GetFullPath(wingstall::config::WingstallConfigDir()) + "' directory");
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::HomepageClick()
{
    try
    {
        std::string homePage = "http://slaakko.github.io/wingstall/";
        ShellExecuteA(Handle(), "open", homePage.c_str(), nullptr, nullptr, SW_SHOW);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::LocalDocumentationClick()
{
    try
    {
        std::string indexFilePath = GetFullPath(Path::Combine(WingstallRoot(), "doc/index.html"));
        ShellExecuteA(Handle(), "open", indexFilePath.c_str(), nullptr, nullptr, SW_SHOW);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::AboutClick()
{
    try
    {
        AboutDialog dialog;
        dialog.ShowDialog(*this);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ResetConfigClick()
{
    try
    {
        ResetConfiguration();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::SetCommandStatus()
{
    if (package)
    {
        if (package->IsDirty())
        {
            EnableSave();
        }
        else
        {
            DisableSave();
        }
        closePackageMenuItem->Enable();
        buildPackageMenuItem->Enable();
        buildToolButton->Enable();
        if (boost::filesystem::exists(package->BinDirectoryPath()))
        {
            openBinDirectoryMenuItem->Enable();
            openBinDirectoryToolButton->Enable();
        }
        pathBar->Show();
        if (package->Building())
        {
            buildPackageMenuItem->Disable();
            cancelBuildMenuItem->Enable();
            cancelBuildToolButton->Enable();
            buildToolButton->Disable();
            closePackageMenuItem->Disable();
            openBinDirectoryMenuItem->Disable();
            savePackageMenuItem->Disable();
            saveToolButton->Disable();
            newPackageMenuItem->Disable();
            openPackageMenuItem->Disable();
            exitMenuItem->Disable();
        }
        else
        {
            buildPackageMenuItem->Enable();
            buildToolButton->Enable();
            cancelBuildMenuItem->Disable();
            cancelBuildToolButton->Disable();
            newPackageMenuItem->Enable();
            openPackageMenuItem->Enable();
            exitMenuItem->Enable();
        }
    }
    else
    {
        savePackageMenuItem->Disable();
        saveToolButton->Disable();
        closePackageMenuItem->Disable();
        buildPackageMenuItem->Disable();
        buildToolButton->Disable();
        cancelBuildMenuItem->Disable();
        cancelBuildToolButton->Disable();
        openBinDirectoryMenuItem->Disable();
        pathBar->Hide();
    }
}

} } // wingstall::package_editor
