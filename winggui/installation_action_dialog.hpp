// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGGUI_INSTALLATION_ACTION_DIALOG_INCLUDED
#define WINGSTALL_WINGGUI_INSTALLATION_ACTION_DIALOG_INCLUDED
#include <winggui/api.hpp>
#include <wing/Window.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>

namespace wingstall { namespace winggui {

using namespace cmajor::wing;

class InstallationActionDialog : public Window
{
public:
    InstallationActionDialog();
private:
    Button* abortButton;
    Button* rollbackButton;
    Button* continueButton;
};

} } // namespace wingstall::wingui

#endif // WINGSTALL_WINGGUI_INSTALLATION_ACTION_DIALOG_INCLUDED
