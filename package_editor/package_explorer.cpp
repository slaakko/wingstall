// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/package_explorer.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <stdexcept>

namespace wingstall { namespace package_editor {

Color DefaultPackageExplorerBackgroundColor()
{
    return Color::White;
}

Color DefaultPackageExplorerFrameColor()
{
    return Color(204, 206, 219);
}

Padding PackageExplorerNodeImagePadding()
{
    return Padding(2, 2, 2, 2);
}

PackageExplorerCreateParams::PackageExplorerCreateParams()
{
    controlCreateParams.WindowClassName("wingstall.pacakge_editor.package_explorer");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultPackageExplorerBackgroundColor());
    treeViewCreateParams = TreeViewCreateParams().NodeIndentPercent(100).NodeImagePadding(PackageExplorerNodeImagePadding());
    frameColor = DefaultPackageExplorerFrameColor();
}

PackageExplorerCreateParams& PackageExplorerCreateParams::Defaults()
{
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return* this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return* this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

PackageExplorerCreateParams& PackageExplorerCreateParams::FrameColor(const Color& frameColor_)
{
    frameColor = frameColor_;
    return *this;
}

PackageExplorer::PackageExplorer(PackageExplorerCreateParams& createParams) : Control(createParams.controlCreateParams), package(nullptr), treeView(nullptr), child(nullptr), container(this)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(createParams.treeViewCreateParams));
    treeView = treeViewPtr.get();
    treeView->SetDoubleBuffered();
    treeView->AddImage("package.bitmap");
    treeView->AddImage("components.bitmap");
    treeView->AddImage("component.bitmap");
    treeView->AddImage("component.bitmap");
    treeView->AddImage("add.folder.bitmap");
    treeView->AddImage("add.file.bitmap");
    treeView->AddImage("delete.folder.bitmap");
    treeView->AddImage("delete.file.bitmap");
    treeView->AddImage("folder.closed.bitmap");
    treeView->AddImage("folder.opened.bitmap");
    treeView->AddImage("file.bitmap");
    treeView->AddImage("rules.bitmap");
    treeView->AddImage("document.collection.bitmap");
    treeView->AddImage("package.properties.bitmap");
    treeView->AddImage("environment.bitmap");
    treeView->AddImage("links.bitmap");
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> borderedTreeView(new BorderedControl(BorderedControlCreateParams(paddedTreeView.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(createParams.frameColor)));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(borderedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    container.AddChild(scrollableTreeView.release());
    Invalidate();
}

void PackageExplorer::SetPackage(Package* package_)
{
    package = package_;
    if (package)
    {
        treeView->SetRoot(package->ToTreeViewNode(treeView));
        package->GetTreeViewNode()->Expand();
        package->GetComponents()->GetTreeViewNode()->Expand();
    }
    else
    {
        treeView->SetRoot(nullptr);
    }
    Invalidate();
}

void PackageExplorer::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void PackageExplorer::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void PackageExplorer::SetChildPos()
{
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

void PackageExplorer::OnPaint(PaintEventArgs& args)
{
    try
    {
        treeView->Invalidate();
        child->Invalidate();
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

} } // wingstall::package_editor
