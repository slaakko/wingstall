// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_BUTTON_INCLUDED
#define WING_BUTTON_INCLUDED 
#include <wing/Window.hpp>

namespace wing {

class WING_API ButtonBase : public Control
{
public:
    ButtonBase(ControlCreateParams& createParams);
    void DoClick();
};

class WING_API Button : public ButtonBase
{
public:
    Button(ControlCreateParams& createParams);
    bool IsButton() const override { return true; }
    bool IsDefault() const { return isDefault; }
    void SetDefault();
    void ResetDefault();
    DialogResult GetDialogResult() const { return dialogResult; }
    void SetDialogResult(DialogResult dialogResult_) { dialogResult = dialogResult_; }
protected:
    void OnClick() override;
    void OnCreated() override;
    void OnGotFocus() override;
    void OnLostFocus() override;
private:
    bool isDefault;
    DialogResult dialogResult;
    void SetDefaultButtonStyle();
    void ResetDefaultButtonStyle();
};

} // wing

#endif // WING_BUTTON_INCLUDED
