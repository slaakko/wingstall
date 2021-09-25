// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/path_bar.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/ScrollableControl.hpp>

namespace wingstall { namespace package_editor {

Color DefaultPathBarBackgroundColor()
{
    return Color::White;
}

Color DefaultPathBarFrameColor()
{
    return Color(204, 206, 219);
}

PathBarCreateParams::PathBarCreateParams(ImageList* imageList_) : controlCreateParams(), imageList(imageList_)
{
    controlCreateParams.WindowClassName("wingstall.package_editor.directory_content_view").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(DefaultPathBarBackgroundColor()).
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
    std::unique_ptr<DirectoryPathView> directoryPathViewPtr(new DirectoryPathView());
    directoryPathView = directoryPathViewPtr.get();
    AddChild(directoryPathViewPtr.release());
}

void PathBar::SetDirectoryPath(const std::string& directoryPath)
{
    directoryPathView->SetDirectoryPath(directoryPath);
    Invalidate();
}

void PathBar::OnPaint(PaintEventArgs& args)
{
    Size sz = GetSize();
    if (directoryPathView)
    {
        directoryPathView->Measure(args.graphics);
        directoryPathView->Invalidate();
        sz = BorderedSize(PaddedSize(directoryPathView->GetMeasuredSize(), DefaultPadding()), BorderStyle::single);
    }
    SetSize(sz);
    Control::OnPaint(args);
}

DirectoryPathView::DirectoryPathView() : 
    Control(ControlCreateParams().WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White).WindowClassName("wingstall.package_editor.directory_path_view").SetSize(Size(100, 0)).SetDock(Dock::left)),
    directoryPath(), blackBrush(Color::Black)
{
    std::u16string fontFamilyName = u"Segoe UI";
    SetFont(wing::Font(wing::FontFamily((const WCHAR*)fontFamilyName.c_str()), 9.0, FontStyle::FontStyleRegular, Unit::UnitPoint));
}

void DirectoryPathView::SetDirectoryPath(const std::string& directoryPath_)
{
    directoryPath = directoryPath_;
    Invalidate();
}

void DirectoryPathView::Measure(Graphics& graphics)
{
    PointF origin;
    StringFormat stringFormat;
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    std::string str = directoryPath;
    if (str.empty())
    {
        str = "C:/directory_path";
    }
    RectF r = MeasureString(graphics, str, GetFont(), origin, stringFormat);
    measuredSize = Size(r.Width + 0.5f, r.Height + 0.5f);
    SetSize(measuredSize);
}

void DirectoryPathView::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    PointF origin(0, 0);
    DrawString(args.graphics, directoryPath, GetFont(), origin, blackBrush);
}

} } // wingstall::package_editor
