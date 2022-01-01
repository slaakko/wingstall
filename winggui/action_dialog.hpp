// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGGUI_ACTION_DIALOG_INCLUDED
#define WINGSTALL_WINGGUI_ACTION_DIALOG_INCLUDED
#include <winggui/api.hpp>
#include <wing/Window.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>

namespace wingstall { namespace winggui {

using namespace wing;

enum class ActionDialogKind
{
    installationAction, uninstallationAction
};

class ActionDialog : public Window
{
public:
    ActionDialog(ActionDialogKind kind);
private:
    Button* abortButton;
    Button* continueButton;
};

} } // namespace wingstall::wingui

#endif // WINGSTALL_WINGGUI_ACTION_DIALOG_INCLUDED
