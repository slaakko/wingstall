// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_PATH_VIEW_INCLUDED
#define WING_PATH_VIEW_INCLUDED
#include <wing/Container.hpp>
#include <wing/Control.hpp>

namespace wing {

WING_API Color DefaultPathViewTextColor();
WING_API Color DefaultPathViewTickColor();
WING_API Color DefaultPathViewMouseOverColor();
WING_API Color DefaultPathViewMouseClickColor();
WING_API Padding DefaultPathViewPathComponentPadding();
WING_API Padding DefaultPathViewTickPadding();
WING_API std::string DefaultPathViewFontFamilyName();
WING_API float DefaultPathViewFontSize();
WING_API float DefaultPathViewTickSizePercent();

class PathComponent;

struct WING_API PathComponentArgs
{
    PathComponentArgs(PathComponent* pathComponent_) : pathComponent(pathComponent_) {}
    PathComponent* pathComponent;
};

using PathComponentSelectedEvent = EventWithArgs<PathComponentArgs>;

struct WING_API PathViewCreateParams
{
    PathViewCreateParams();
    PathViewCreateParams& Defaults();
    PathViewCreateParams& WindowClassName(const std::string& windowClassName_);
    PathViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    PathViewCreateParams& WindowStyle(int windowStyle_);
    PathViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    PathViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    PathViewCreateParams& Text(const std::string& text_);
    PathViewCreateParams& Location(const Point& location_);
    PathViewCreateParams& SetSize(Size size_);
    PathViewCreateParams& SetAnchors(Anchors anchors_);
    PathViewCreateParams& SetDock(Dock dock_);
    PathViewCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    PathViewCreateParams& FontSize(float fontSize_);
    PathViewCreateParams& TextColor(const Color& textColor_);
    PathViewCreateParams& TickSizePercent(float tickSizePercent_);
    PathViewCreateParams& TickColor(const Color& tickColor_);
    PathViewCreateParams& PathComponentPadding(const Padding& padding);
    PathViewCreateParams& TickPadding(const Padding& padding);
    PathViewCreateParams& MouseOverColor(const Color& mouseOverColor_);
    PathViewCreateParams& MouseClickColor(const Color& mouseClickColor_);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color textColor;
    float tickSizePercent;
    Color tickColor;
    Padding pathComponentPadding;
    Padding tickPadding;
    Color mouseOverColor;
    Color mouseClickColor;
};

class WING_API PathView : public Control
{
public:
    PathView(PathViewCreateParams& createParams);
    void AddPathComponent(const std::string& pathComponentName, void* data);
    void PushPathComponent(const std::string& pathComponentName, void* data);
    PathComponent* PathComponentAt(const Point& location) const;
    void Clear();
    void SetTextHeight(float textHeight_);
    void SetMaxWidth(int maxWidth_);
    const Padding& GetPathComponentPadding() const { return pathComponentPadding; }
    const SolidBrush& TextBrush() const { return textBrush; }
    const SolidBrush& MouseOverBrush() const { return mouseOverBrush; }
    const SolidBrush& MouseClickBrush() const { return mouseClickBrush; }
    PathComponentSelectedEvent& PathComponentSelected() { return pathComponentSelected; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    virtual void OnPathComponentSelected(PathComponentArgs& args);
private:
    void Measure(Graphics& graphics);
    void DrawTick(Graphics& graphics, PointF& origin);
    Container pathComponents;
    float tickSizePercent;
    SolidBrush textBrush;
    SolidBrush tickBrush;
    SolidBrush mouseOverBrush;
    SolidBrush mouseClickBrush;
    Padding pathComponentPadding;
    Padding tickPadding;
    float textHeight;
    float tickHeight;
    float tickWidth;
    float sqrt3per2;
    int maxWidth;
    PathComponentSelectedEvent pathComponentSelected;
    PathComponent* mouseDownComponent;
    PathComponent* mouseOverComponent;
};

enum class PathComponentState : int
{
    idle, mouseOver, mouseClick
};

class WING_API PathComponent : public Component
{
public:
    PathComponent(PathView* view_, const std::string& name_, void* data_);
    bool IsPathComponent() const override { return true; }
    void Measure(Graphics& graphics);
    void Draw(Graphics& graphics);
    const std::string& Name() const { return name; }
    const Point& Location() const { return location; }
    void SetLocation(const Point& location_);
    const Size& GetSize() const { return size; }
    void* Data() const { return data; }
    void SetState(PathComponentState state_);
    PathComponentState State() const { return state; }
private:
    PathView* view;
    std::string name;
    Point location;
    Size size;
    void* data;
    PathComponentState state;
};

} // wing

#endif // WING_PATH_VIEW_INCLUDED
