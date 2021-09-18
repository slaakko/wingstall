// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PACKAGE_EXPLORER_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PACKAGE_EXPLORER_INCLUDED
#include <package_editor/package.hpp>
#include <wing/ContainerControl.hpp>
#include <wing/TreeView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class PackageContentView;

Color DefaultPackageExplorerBackgroundColor();
Color DefaultPackageExplorerFrameColor();
Padding PackageExplorerNodeImagePadding();

struct PackageExplorerCreateParams
{
    PackageExplorerCreateParams();
    PackageExplorerCreateParams& Defaults();
    PackageExplorerCreateParams& WindowClassName(const std::string& windowClassName_);
    PackageExplorerCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    PackageExplorerCreateParams& WindowStyle(int windowStyle_);
    PackageExplorerCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    PackageExplorerCreateParams& BackgroundColor(const Color& backgroundColor_);
    PackageExplorerCreateParams& Text(const std::string& text_);
    PackageExplorerCreateParams& Location(Point location_);
    PackageExplorerCreateParams& SetSize(Size size_);
    PackageExplorerCreateParams& SetAnchors(Anchors anchors_);
    PackageExplorerCreateParams& SetDock(Dock dock_);
    PackageExplorerCreateParams& FrameColor(const Color& frameColor_);
    ControlCreateParams controlCreateParams;
    TreeViewCreateParams treeViewCreateParams;
    Color frameColor;
};

class PackageExplorer : public ContainerControl
{
public:
    PackageExplorer(PackageExplorerCreateParams& createParams);
    void SetPackage(Package* package_);
    void SetImageList(ImageList* imageList);
    void SetContentView(PackageContentView* contentView_);
    void Open(Node* node);
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void TreeViewNodeClick(TreeViewNodeClickEventArgs& args);
    void TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    void MakeView();
    TreeViewCreateParams treeViewCreateParams;
    ImageList* imageList;
    Color frameColor;
    Package* package;
    TreeView* treeView;
    Control* child;
    PackageContentView* contentView;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PACKAGE_EXPLORER_INCLUDED
