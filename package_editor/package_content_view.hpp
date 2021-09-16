// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PACKAGE_CONTENT_VIEW_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PACKAGE_CONTENT_VIEW_INCLUDED
#include <package_editor/package.hpp>
#include <wing/ContainerControl.hpp>

namespace wingstall { namespace package_editor {

class EmptyView : public Control
{
public:
    EmptyView();
protected:
    void OnPaint(PaintEventArgs& args) override;
};

Color DefaultPackageContentViewBackgroundColor();
Color DefaultPackageContentViewFrameColor();

struct PackageContentViewCreateParams
{
    PackageContentViewCreateParams();
    PackageContentViewCreateParams& Defaults();
    PackageContentViewCreateParams& WindowClassName(const std::string& windowClassName_);
    PackageContentViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    PackageContentViewCreateParams& WindowStyle(int windowStyle_);
    PackageContentViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    PackageContentViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    PackageContentViewCreateParams& Text(const std::string& text_);
    PackageContentViewCreateParams& Location(Point location_);
    PackageContentViewCreateParams& SetSize(Size size_);
    PackageContentViewCreateParams& SetAnchors(Anchors anchors_);
    PackageContentViewCreateParams& SetDock(Dock dock_);
    PackageContentViewCreateParams& FrameColor(const Color& frameColor_);
    ControlCreateParams controlCreateParams;
    Color frameColor;
};

class PackageContentView : public ContainerControl
{
public:
    PackageContentView(PackageContentViewCreateParams& createParams);
    void SetImageList(ImageList* imageList_) { imageList = imageList_; }
    void ViewContent(Node* node);
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    Control* MakeFramedControl(Control* child_);
    Control* framedChild;
    Control* child;
    Color frameColor;
    ImageList* imageList;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PACKAGE_CONTENT_VIEW_INCLUDED
