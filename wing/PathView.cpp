// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/PathView.hpp>
#include <soulng/util/Unicode.hpp>

#undef max
#undef min

namespace wing {

using namespace soulng::unicode;

Color DefaultPathViewTextColor()
{
    return Color::Black;
}

Color DefaultPathViewTickColor()
{
    return Color::Black;
}

Color DefaultPathViewMouseOverColor()
{
    return Color(230, 243, 255);
}

Color DefaultPathViewMouseClickColor()
{
    return Color(204, 232, 255);
}

Padding DefaultPathViewPathComponentPadding()
{
    return Padding(2, 6, 2, 6);
}

Padding DefaultPathViewTickPadding()
{
    return Padding(4, 6, 4, 6);
}

std::string DefaultPathViewFontFamilyName()
{
    return "Segoe UI";
}

float DefaultPathViewFontSize()
{
    return 9.0f;
}

float DefaultPathViewTickSizePercent()
{
    return 40.0f;
}

PathViewCreateParams::PathViewCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("wing.PathView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    controlCreateParams.BackgroundColor(Color::White);
    fontFamilyName = DefaultPathViewFontFamilyName();
    fontSize = DefaultPathViewFontSize();
    textColor = DefaultPathViewTextColor();
    tickSizePercent = DefaultPathViewTickSizePercent();
    tickColor = DefaultPathViewTickColor();
    pathComponentPadding = DefaultPathViewPathComponentPadding(); 
    tickPadding = DefaultPathViewTickPadding();
    mouseOverColor = DefaultPathViewMouseOverColor();
    mouseClickColor = DefaultPathViewMouseClickColor();
}

PathViewCreateParams& PathViewCreateParams::Defaults()
{
    return *this;
}

PathViewCreateParams& PathViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

PathViewCreateParams& PathViewCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::TextColor(const Color& textColor_)
{
    textColor = textColor_;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::TickSizePercent(float tickSizePercent_)
{
    tickSizePercent = tickSizePercent_;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::TickColor(const Color& tickColor_)
{
    tickColor = tickColor_;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::PathComponentPadding(const Padding& padding)
{
    pathComponentPadding = padding;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::TickPadding(const Padding& padding)
{
    tickPadding = padding;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::MouseOverColor(const Color& mouseOverColor_)
{
    mouseOverColor = mouseOverColor_;
    return *this;
}

PathViewCreateParams& PathViewCreateParams::MouseClickColor(const Color& mouseClickColor_)
{
    mouseClickColor = mouseClickColor_;
    return *this;
}

PathView::PathView(PathViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    pathComponents(this), 
    tickSizePercent(createParams.tickSizePercent), 
    textBrush(createParams.textColor), 
    tickBrush(createParams.tickColor),
    mouseOverBrush(createParams.mouseOverColor),
    mouseClickBrush(createParams.mouseClickColor),
    pathComponentPadding(createParams.pathComponentPadding),
    tickPadding(createParams.tickPadding),
    maxWidth(0),
    textHeight(0),
    tickHeight(0), 
    tickWidth(0),
    mouseDownComponent(nullptr),
    mouseOverComponent(nullptr)
{
    std::u16string fontFamilyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)fontFamilyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    sqrt3per2 = std::sqrt(3.0) / 2;
    SetSize(Size(10, 10));
    Invalidate();
}

void PathView::OnPaint(PaintEventArgs& args)
{
    Measure(args.graphics);
    Component* startChild = nullptr;
    if (maxWidth > 0)
    {
        PointF origin(0, 0);
        int width = 0;
        Component* child = pathComponents.LastChild();
        bool first = true;
        while (child)
        {
            if (child->IsPathComponent())
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    width = width + tickWidth + tickPadding.Horizontal();
                }
                PathComponent* pathComponent = static_cast<PathComponent*>(child);
                Size sz = pathComponent->GetSize();
                if (width + sz.Width > maxWidth)
                {
                    break;
                }
                width = width + sz.Width;
                startChild = child;
            }
            child = child->PrevSibling();
        }
    }
    else
    {
        startChild = pathComponents.FirstChild();
    }
    args.graphics.Clear(BackgroundColor());
    PointF origin(0, 0);
    Component* child = startChild;
    bool first = true;
    while (child)
    {
        if (child->IsPathComponent())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                DrawTick(args.graphics, origin);
            }
            PathComponent* pathComponent = static_cast<PathComponent*>(child);
            pathComponent->SetLocation(Point(origin.X, origin.Y));
            pathComponent->Draw(args.graphics);
            Size sz = pathComponent->GetSize();
            origin.X = origin.X + sz.Width;
        }
        child = child->NextSibling();
    }
}

void PathView::OnMouseDown(MouseEventArgs& args)
{
    PathComponent* pathComponent = PathComponentAt(args.location);
    if (pathComponent)
    {
        mouseDownComponent = pathComponent;
        mouseDownComponent->SetState(PathComponentState::mouseClick);
    }
    else
    {
        mouseDownComponent = nullptr;
    }
}

void PathView::OnMouseUp(MouseEventArgs& args)
{
    PathComponent* pathComponent = PathComponentAt(args.location);
    if (pathComponent)
    {
        pathComponent->SetState(PathComponentState::idle);
        if (pathComponent == mouseDownComponent)
        {
            PathComponentArgs args(pathComponent);
            OnPathComponentSelected(args);
        }
    }
    mouseDownComponent = nullptr;
}

void PathView::OnMouseMove(MouseEventArgs& args)
{
    if (mouseOverComponent)
    {
        mouseOverComponent->SetState(PathComponentState::idle);
        mouseOverComponent = nullptr;
    }
    PathComponent* pathComponent = PathComponentAt(args.location);
    if (pathComponent)
    {
        pathComponent->SetState(PathComponentState::mouseOver);
        mouseOverComponent = pathComponent;
    }
}

void PathView::OnMouseEnter()
{
    mouseDownComponent = nullptr;
    mouseOverComponent = nullptr;
}

void PathView::OnMouseLeave()
{
    if (mouseDownComponent)
    {
        mouseDownComponent->SetState(PathComponentState::idle);
        mouseDownComponent = nullptr;
    }
    if (mouseOverComponent)
    {
        mouseOverComponent->SetState(PathComponentState::idle);
        mouseOverComponent = nullptr;
    }
}

void PathView::OnPathComponentSelected(PathComponentArgs& args)
{
    pathComponentSelected.Fire(args);
}

void PathView::DrawTick(Graphics& graphics, PointF& origin)
{
    Gdiplus::SmoothingMode prevMode = graphics.GetSmoothingMode();
    CheckGraphicsStatus(graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality));
    PointF p0(tickPadding.left + origin.X, tickPadding.top + origin.Y + (textHeight - tickHeight) / 2.0f);
    PointF p1(p0.X, p0.Y + tickHeight);
    PointF p2(p0.X + tickWidth, p0.Y + tickHeight / 2.0f);
    std::vector<PointF> points;
    points.push_back(p0);
    points.push_back(p1);
    points.push_back(p2);
    CheckGraphicsStatus(graphics.FillPolygon(&tickBrush, points.data(), 3));
    origin.X = origin.X + tickWidth + tickPadding.Horizontal();
    graphics.SetSmoothingMode(prevMode);
}

void PathView::Measure(Graphics& graphics)
{
    Component* child = pathComponents.FirstChild();
    while (child)
    {
        if (child->IsPathComponent())
        {
            PathComponent* pathComponent = static_cast<PathComponent*>(child);
            pathComponent->Measure(graphics);
        }
        child = child->NextSibling();
    }
    if (textHeight > 0)
    {
        tickHeight = (tickSizePercent / 100.0f) * textHeight;
        tickWidth = sqrt3per2 * tickHeight;
    }
}

void PathView::Clear()
{
    mouseDownComponent = nullptr;
    mouseOverComponent = nullptr;
    if (!pathComponents.IsEmpty())
    {
        while (!pathComponents.IsEmpty())
        {
            pathComponents.RemoveChild(pathComponents.FirstChild());
        }
        Invalidate();
    }
}

void PathView::SetTextHeight(float textHeight_)
{
    textHeight = std::max(textHeight, textHeight_);
}

void PathView::SetMaxWidth(int maxWidth_)
{
    if (maxWidth != maxWidth_)
    {
        maxWidth = maxWidth_;
        Size sz = GetSize();
        sz.Width = maxWidth;
        SetSize(sz);
        Invalidate();
    }
}

void PathView::AddPathComponent(const std::string& pathComponentName, void* data)
{
    PathComponent* pathComponent = new PathComponent(this, pathComponentName, data);
    pathComponents.AddChild(pathComponent);
    Invalidate();
}

void PathView::PushPathComponent(const std::string& pathComponentName, void* data)
{
    PathComponent* pathComponent = new PathComponent(this, pathComponentName, data);
    if (!pathComponents.IsEmpty())
    {
        pathComponents.InsertBefore(pathComponent, pathComponents.FirstChild());
    }
    else
    {
        pathComponents.AddChild(pathComponent);
    }
    Invalidate();
}

PathComponent* PathView::PathComponentAt(const Point& location) const
{
    Component* child = pathComponents.FirstChild();
    while (child)
    {
        if (child->IsPathComponent())
        {
            PathComponent* pathComponent = static_cast<PathComponent*>(child);
            Rect r(pathComponent->Location(), pathComponent->GetSize());
            if (r.Contains(location))
            {
                return pathComponent;
            }
        }
        child = child->NextSibling();
    }
    return nullptr;
}

PathComponent::PathComponent(PathView* view_, const std::string& name_, void* data_) : Component(), view(view_), name(name_), location(), size(), data(data_), state(PathComponentState::idle)
{
}

void PathComponent::SetLocation(const Point& location_)
{
    location = location_;
}

void PathComponent::SetState(PathComponentState state_)
{
    if (state != state_)
    {
        state = state_;
        view->Invalidate();
    }
}

void PathComponent::Draw(Graphics& graphics)
{
    if (state == PathComponentState::mouseOver)
    {
        Rect rect(location, GetSize());
        graphics.FillRectangle(&view->MouseOverBrush(), rect);
    }
    else if (state == PathComponentState::mouseClick)
    {
        Rect rect(location, GetSize());
        graphics.FillRectangle(&view->MouseClickBrush(), rect);
    }
    Padding componentPadding = view->GetPathComponentPadding();
    PointF origin(location.X, location.Y);
    origin.X = origin.X + componentPadding.left;
    origin.Y = origin.Y + componentPadding.top;
    DrawString(graphics, name, view->GetFont(), origin, view->TextBrush());
}

void PathComponent::Measure(Graphics& graphics)
{
    Padding componentPadding = view->GetPathComponentPadding();
    PointF origin(0, 0);
    StringFormat stringFormat;
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    RectF r = MeasureString(graphics, name, view->GetFont(), origin, stringFormat);
    view->SetTextHeight(r.Height);
    size = Size(componentPadding.Horizontal() + r.Width, componentPadding.Vertical() + r.Height);
}

} // wing
