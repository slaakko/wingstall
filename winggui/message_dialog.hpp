// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGGUI_MESSAGE_DIALOG_INCLUDED
#define WINGSTALL_WINGGUI_MESSAGE_DIALOG_INCLUDED
#include <winggui/api.hpp>
#include <wing/Window.hpp>
#include <wing/Button.hpp>
#include <wing/Label.hpp>

namespace wingstall { namespace winggui {

using namespace wing;

enum class MessageKind
{
    info, error
};

class MessageDialog : public Window
{
public:
    MessageDialog(MessageKind messageKind, const std::string& messageText);
private:
    Button* okButton;
};

} } // namespace wingstall::wingui

#endif // WINGSTALL_WINGGUI_MESSAGE_DIALOG_INCLUDED
