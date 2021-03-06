// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_CHECK_BOX_INCLUDED
#define WING_CHECK_BOX_INCLUDED
#include <wing/Button.hpp>

namespace wing {

using CheckedChangedEvent = Event;

struct WING_API CheckBoxCreateParams
{
    CheckBoxCreateParams();
    CheckBoxCreateParams& Defaults();
    CheckBoxCreateParams& WindowClassName(const std::string& windowClassName_);
    CheckBoxCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CheckBoxCreateParams& WindowStyle(int windowStyle_);
    CheckBoxCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CheckBoxCreateParams& BackgroundColor(const Color& backgroundColor_);
    CheckBoxCreateParams& Text(const std::string& text_);
    CheckBoxCreateParams& Location(Point location_);
    CheckBoxCreateParams& SetSize(Size size_);
    CheckBoxCreateParams& SetAnchors(Anchors anchors_);
    CheckBoxCreateParams& SetDock(Dock dock_);
    CheckBoxCreateParams& AutoSize(bool autoSize_);
    ControlCreateParams controlCreateParams;
    bool autoSize;
};

enum class CheckBoxFlags : int
{
    none = 0, checked = 1 << 0, autoSize = 1 << 1, autoSized = 1 << 2
};

WING_API inline CheckBoxFlags operator|(CheckBoxFlags left, CheckBoxFlags right)
{
    return CheckBoxFlags(int(left) | int(right));
}

WING_API inline CheckBoxFlags operator&(CheckBoxFlags left, CheckBoxFlags right)
{
    return CheckBoxFlags(int(left) & int(right));
}

WING_API inline CheckBoxFlags operator~(CheckBoxFlags flags)
{
    return CheckBoxFlags(~int(flags));
}

class WING_API CheckBox : public ButtonBase
{
public:
    CheckBox(CheckBoxCreateParams& createParams);
    bool Checked() const { return (flags & CheckBoxFlags::checked) != CheckBoxFlags::none; }
    void SetChecked(bool checked);
    CheckedChangedEvent& CheckedChanged() { return checkedChanged; }
protected:
    void OnCreated() override;
    void OnClick() override;
    void OnTextChanged() override;
    virtual void OnCheckedChanged();
private:
    void DoAutoSize();
    Size GetCheckSize();
    Size GetBorderSize();
    void SetCheckedFlag();
    void ResetCheckedFlag();
    CheckBoxFlags flags;
    CheckedChangedEvent checkedChanged;
};

} // wing

#endif // WING_CHECK_BOX_INCLUDED
