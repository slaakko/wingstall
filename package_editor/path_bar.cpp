// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/path_bar.hpp>
#include <wing/ImageList.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

Color DefaultPathBarBackgroundColor()
{
    return Color::White;
}

Color DefaultPathBarFrameColor()
{
    return Color(204, 206, 219);
}

Color DefaultDirectoryPathViewBackgroundColor()
{
    return Color::White;
}

Color DefaultParentPathSelectorBackgroundColor()
{
    return Color::White;
}

Color DefaultParentPathSelectorMouseOverColor()
{
    return Color(230, 243, 255);
}

Color DefaultParentPathSelectorMouseClickColor()
{
    return Color(204, 232, 255);
}

Color DefaultPathDividerBackgroundColor()
{
    return Color::White;
}

Color DefaultPathDividerLineColor()
{
    return Color(204, 206, 219);
}

std::string DefaultDirectoryPathViewFontFamilyName()
{
    return "Segoe UI";
}

float DefaultDirectoryPathViewFontSize()
{
    return 9.0f;
}

DirectoryPathViewCreateParams::DirectoryPathViewCreateParams()
{
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(DefaultDirectoryPathViewBackgroundColor()).
        WindowClassName("wingstall.package_editor.directory_path_view").SetSize(Size(100, 0)).SetDock(Dock::left);
    fontFamilyName = DefaultDirectoryPathViewFontFamilyName();
    fontSize = DefaultDirectoryPathViewFontSize();
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::Defaults()
{
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

DirectoryPathViewCreateParams& DirectoryPathViewCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return *this;
}

ParentPathSelectorCreateParams::ParentPathSelectorCreateParams(ImageList* imageList_) : controlCreateParams(), imageList(imageList_)
{
    controlCreateParams.BackgroundColor(DefaultParentPathSelectorBackgroundColor()).WindowClassBackgroundColor(COLOR_WINDOW).WindowClassName("wingstall.package_editor.parent_path_selector").
        SetSize(Size(20, 0)).SetDock(Dock::left);
    mouseOverColor = DefaultParentPathSelectorMouseOverColor();
    mouseClickColor = DefaultParentPathSelectorMouseClickColor();
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::Defaults()
{
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::MouseOverColor(const Color& mouseOverColor_)
{
    mouseOverColor = mouseOverColor_;
    return *this;
}

ParentPathSelectorCreateParams& ParentPathSelectorCreateParams::MouseClickColor(const Color& mouseClickColor_)
{
    mouseClickColor = mouseClickColor_;
    return *this;
}

PathDividerCreateParams::PathDividerCreateParams()
{
    controlCreateParams.BackgroundColor(DefaultPathDividerBackgroundColor()).WindowClassBackgroundColor(COLOR_WINDOW).WindowClassName("wingstall.package_editor.path_divider").
        SetSize(Size(1, 0)).SetDock(Dock::left);
    penColor = DefaultPathDividerLineColor();
}

PathDividerCreateParams& PathDividerCreateParams::Defaults()
{
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

PathDividerCreateParams& PathDividerCreateParams::PenColor(const Color& penColor_)
{
    penColor = penColor_;
    return *this;
}

PathBarCreateParams::PathBarCreateParams(ImageList* imageList_) : controlCreateParams(), imageList(imageList_), parentPathSelectorCreateParams(imageList)
{
    controlCreateParams.WindowClassName("wingstall.package_editor.path_bar").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(DefaultPathBarBackgroundColor()).
        WindowStyle(DefaultChildWindowStyle());
    frameColor = DefaultPathBarFrameColor();
}

PathBarCreateParams& PathBarCreateParams::Defaults()
{
    return *this;
}

PathBarCreateParams& PathBarCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

PathBarCreateParams& PathBarCreateParams::FrameColor(const Color& frameColor_)
{
    frameColor = frameColor_;
    return *this;
}

PathBar::PathBar(PathBarCreateParams& createParams)  : ContainerControl(createParams.controlCreateParams)
{
    std::unique_ptr<DirectoryPathView> directoryPathViewPtr(new DirectoryPathView(createParams.directoryPathViewCreateParams));
    directoryPathView = directoryPathViewPtr.get();
    AddChild(directoryPathViewPtr.release());
    std::unique_ptr<PathDivider> pathDividerPtr1(new PathDivider(createParams.pathDividerCreateParams));
    pathDivider1 = pathDividerPtr1.get();
    AddChild(pathDividerPtr1.release());
    std::unique_ptr<ParentPathSelector> parentPathSelectorPtr(new ParentPathSelector(createParams.parentPathSelectorCreateParams));
    parentPathSelector = parentPathSelectorPtr.get();
    AddChild(parentPathSelectorPtr.release());
    Invalidate();
    std::unique_ptr<PathDivider> pathDividerPtr2(new PathDivider(createParams.pathDividerCreateParams));
    pathDivider2 = pathDividerPtr2.get();
    AddChild(pathDividerPtr2.release());
    std::unique_ptr<PathView> pathViewPtr(new PathView(PathViewCreateParams().SetDock(Dock::left)));
    pathView = pathViewPtr.get();
    AddChild(pathViewPtr.release());
}

void PathBar::SetDirectoryPath(const std::string& directoryPath)
{
    directoryPathView->SetDirectoryPath(directoryPath);
    Invalidate();
}

void PathBar::SetDirectoryPathViewWidth(int width)
{
    Size sz = directoryPathView->GetSize();
    sz.Width = width + 4;
    directoryPathView->SetSize(sz);
    Size pathDivider1Size = pathDivider1->GetSize();
    sz.Width = sz.Width + pathDivider1Size.Width;
    sz.Width = sz.Width + 32;
    Size pathDivider2Size = pathDivider2->GetSize();
    sz.Width = sz.Width + pathDivider2Size.Width;
    Size pvSize = pathView->GetSize();
    sz.Width = sz.Width + pvSize.Width;
    sz.Height = std::max(sz.Height, pvSize.Height);
    SetSize(sz);
}

void PathBar::SetPathViewMaxWidth(int pathViewMaxWidth)
{
    Size sz = directoryPathView->GetSize();
    directoryPathView->SetSize(sz);
    Size pathDivider1Size = pathDivider1->GetSize();
    sz.Width = sz.Width + pathDivider1Size.Width;
    sz.Width = sz.Width + 32;
    Size pathDivider2Size = pathDivider2->GetSize();
    sz.Width = sz.Width + pathDivider2Size.Width;
    pathView->SetMaxWidth(pathViewMaxWidth);
    Size pvSize = pathView->GetSize();
    sz.Width = sz.Width + pvSize.Width;
    sz.Height = std::max(sz.Height, pvSize.Height);
    SetSize(sz);
}

void PathBar::SetCurrentNode(Node* currentNode)
{
    pathView->Clear();
    while (currentNode)
    {
        pathView->PushPathComponent(currentNode->PathComponentName(), currentNode);
        currentNode = currentNode->Parent();
    }
}

void PathBar::OnPaint(PaintEventArgs& args)
{
    if (directoryPathView)
    {
        directoryPathView->Invalidate();
    }
    if (pathDivider1)
    {
        pathDivider1->Invalidate();
    }
    if (parentPathSelector)
    {
        parentPathSelector->Invalidate();
    }
    if (pathDivider2)
    {
        pathDivider2->Invalidate();
    }
    if (pathView)
    {
        pathView->Invalidate();
    }
    Control::OnPaint(args);
}

void PathBar::OnSizeChanged()
{
    ContainerControl::OnSizeChanged();
    DockChildren();
}

DirectoryPathView::DirectoryPathView(DirectoryPathViewCreateParams& createParams) :
    Control(createParams.controlCreateParams), directoryPath(), blackBrush(Color::Black)
{
    std::u16string fontFamilyName = ToUtf16(createParams.fontFamilyName); 
    SetFont(wing::Font(wing::FontFamily((const WCHAR*)fontFamilyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
}

void DirectoryPathView::SetDirectoryPath(const std::string& directoryPath_)
{
    directoryPath = directoryPath_;
    Invalidate();
}

void DirectoryPathView::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    PointF origin(4, 6);
    DrawString(args.graphics, directoryPath, GetFont(), origin, blackBrush);
}

ParentPathSelector::ParentPathSelector(ParentPathSelectorCreateParams& createParams) :
    Control(createParams.controlCreateParams), 
    state(ParentPathSelectorState::idle),
    mouseOverBrush(createParams.mouseOverColor), 
    mouseClickBrush(createParams.mouseClickColor)
{
    imageList = createParams.imageList;
}

void ParentPathSelector::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    if (state == ParentPathSelectorState::mouseOver)
    {
        Point loc(0, 0);
        Rect rect(loc, GetSize());
        args.graphics.FillRectangle(&mouseOverBrush, rect);
    }
    else if (state == ParentPathSelectorState::clicked)
    {
        Point loc(0, 0);
        Rect rect(loc, GetSize());
        args.graphics.FillRectangle(&mouseClickBrush, rect);
    }
    Point loc;
    Size sz(GetSize());
    Rect rect(loc, sz);
    if (sz.Width > 0 && sz.Height > 0)
    {
        Bitmap* bitmap = imageList->GetImage(imageList->GetImageIndex("up.arrow.bitmap"));
        Padding padding(2, 7, 2, 7);
        PointF imageLoc(padding.left, padding.top);
        RectF r(imageLoc, SizeF(bitmap->GetWidth() + padding.Horizontal(), bitmap->GetHeight() + padding.Vertical()));
        Gdiplus::ImageAttributes attributes;
        attributes.SetColorKey(DefaultBitmapTransparentColor(), DefaultBitmapTransparentColor());
        CheckGraphicsStatus(args.graphics.DrawImage(bitmap, r, 0, 0, bitmap->GetWidth() + padding.Horizontal(), bitmap->GetHeight() + padding.Vertical(), Unit::UnitPixel, &attributes));
    }
}

void ParentPathSelector::SetState(ParentPathSelectorState state_)
{
    if (state != state_)
    {
        state = state_;
        Invalidate();
    }
}

void ParentPathSelector::OnMouseEnter()
{
    Control::OnMouseEnter();
    SetState(ParentPathSelectorState::mouseOver);
}

void ParentPathSelector::OnMouseLeave()
{
    Control::OnMouseLeave();
    SetState(ParentPathSelectorState::idle);
}

void ParentPathSelector::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    SetState(ParentPathSelectorState::clicked);
}

void ParentPathSelector::OnMouseUp(MouseEventArgs& args)
{
    Control::OnMouseUp(args);
    SetState(ParentPathSelectorState::mouseOver);
}

PathDivider::PathDivider(PathDividerCreateParams& createParams) :
    Control(createParams.controlCreateParams), pen(createParams.penColor)
{
}

void PathDivider::OnPaint(PaintEventArgs& args)
{
    PointF start(0, 0);
    Size sz = GetSize();
    PointF end(0, sz.Height);
    args.graphics.DrawLine(&pen, start, end);
}

} } // wingstall::package_editor
