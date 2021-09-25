// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/package_explorer.hpp>
#include <package_editor/package_content_view.hpp>
#include <package_editor/path_bar.hpp>
#include <package_editor/main_window.hpp>
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
    controlCreateParams.WindowClassName("wingstall.package_editor.package_explorer");
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

PackageExplorer::PackageExplorer(PackageExplorerCreateParams& createParams) : 
    ContainerControl(createParams.controlCreateParams), 
    mainWindow(nullptr),
    package(nullptr), 
    treeView(nullptr), 
    child(nullptr), 
    contentView(nullptr), 
    imageList(nullptr),
    treeViewCreateParams(createParams.treeViewCreateParams),
    frameColor(createParams.frameColor)
{
    MakeView();
}

void PackageExplorer::MakeView()
{
    if (child)
    {
        RemoveChild(child);
        child = nullptr;
        treeView = nullptr;
    }
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(treeViewCreateParams));
    treeView = treeViewPtr.get();
    treeView->SetFlag(ControlFlags::scrollSubject);
    treeView->SetDoubleBuffered();
    treeView->SetImageList(imageList);
    treeView->NodeClick().AddHandler(this, &PackageExplorer::TreeViewNodeClick);
    treeView->NodeDoubleClick().AddHandler(this, &PackageExplorer::TreeViewNodeDoubleClick);
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> borderedTreeView(new BorderedControl(BorderedControlCreateParams(paddedTreeView.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(frameColor)));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(borderedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    AddChild(scrollableTreeView.release());
    Invalidate();
}

void PackageExplorer::SetPackage(Package* package_)
{
    MakeView();
    package = package_;
    if (package)
    {
        treeView->SetRoot(package->ToTreeViewNode(treeView));
        package->GetTreeViewNode()->Expand();
        package->GetComponents()->GetTreeViewNode()->Expand();
    }
    Invalidate();
}

void PackageExplorer::SetImageList(ImageList* imageList_)
{
    imageList = imageList_;
    if (treeView)
    {
        treeView->SetImageList(imageList);
    }
}

void PackageExplorer::SetContentView(PackageContentView* contentView_)
{
    contentView = contentView_;
}

void PackageExplorer::TreeViewNodeClick(TreeViewNodeClickEventArgs& args)
{
    try
    {
        void* data = args.node->Data();
        if (data)
        {
            Node* node = static_cast<Node*>(data);
            if (args.buttons == MouseButtons::lbutton)
            {
                contentView->ViewContent(node);
                node->SetCurrentPathNode();
            }
            else if (args.buttons == MouseButtons::rbutton)
            {
                if (mainWindow)
                {
                    mainWindow->ClearClickActions();
                    std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
                    node->AddMenuItems(contextMenu.get(), mainWindow->ClickActions(), static_cast<MenuItemsKind>(MenuItemsKind::allMenuItems | MenuItemsKind::treeView));
                    if (contextMenu->HasMenuItems())
                    {
                        treeView->TranslateContentLocation(args.location);
                        Point screenLoc = treeView->ClientToScreen(args.location);
                        mainWindow->ShowContextMenu(contextMenu.release(), screenLoc);
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void PackageExplorer::TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args)
{
    try
    {
        TreeViewNode* treeViewNode = args.node;
        if (treeViewNode)
        {
            Node* node = nullptr;
            if (treeViewNode->Data())
            {
                node = static_cast<Node*>(treeViewNode->Data());
            }
            if (node && node->Leaf())
            {
                node->ExecuteDefaultAction();
            }
            else
            {
                if (treeViewNode->State() == TreeViewNodeState::collapsed)
                {
                    treeViewNode->ExpandAll();
                }
                else if (treeViewNode->State() == TreeViewNodeState::expanded)
                {
                    treeViewNode->CollapseAll();
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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

void PackageExplorer::Open(Node* node)
{
    TreeViewNode* treeViewNode = node->GetTreeViewNode();
    if (treeViewNode)
    {
        TreeViewNode* parentNode = treeViewNode->Parent();
        while (parentNode)
        {
            parentNode->Expand();
            parentNode = parentNode->Parent();
        }
        TreeView* treeView = treeViewNode->GetTreeView();
        if (treeView)
        {
            treeView->SetSelectedNode(treeViewNode);
        }
    }
}

Node* PackageExplorer::SelectedNode()
{
    if (treeView)
    {
        TreeViewNode* selectedNode = treeView->SelectedNode();
        if (selectedNode)
        {
            void* data = selectedNode->Data();
            if (data)
            {
                Node* node = static_cast<Node*>(data);
                return node;
            }
        }
    }
    return nullptr;
}

} } // wingstall::package_editor
