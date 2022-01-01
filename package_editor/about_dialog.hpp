// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ABOUT_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ABOUT_DIALOG_INCLUDED
#include <wing/Window.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

std::string WingstallVersionStr();

class AboutDialog : public Window
{
public:
    AboutDialog();
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ABOUT_DIALOG_INCLUDED
