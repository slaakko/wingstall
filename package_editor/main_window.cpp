// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/main_window.hpp>
#include <wing/Dialog.hpp>
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

MainWindow::MainWindow() : Window(WindowCreateParams().Text("Wingstall Package Editor").WindowClassName("wingstall.package_editor")),
    newPackageMenuItem(nullptr),
    openPackageMenuItem(nullptr),
    closePackageMenuItem(nullptr),
    exitMenuItem(nullptr),
    packageExplorer(nullptr)
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
    horizontalSplitContainer->Pane1Container()->AddChild(packageExplorerPtr.release());

    verticalSplitContainer->Pane1Container()->AddChild(horizontalSplitContainer.release());

    AddChild(verticalSplitContainer.release());
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
            packageExplorer->SetPackage(package.get());
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