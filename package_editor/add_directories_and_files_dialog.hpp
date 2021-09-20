// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
#include <package_editor/component.hpp>
#include <wing/Window.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class AddDirectoriesAndFilesDialog : public Window
{
public:
    AddDirectoriesAndFilesDialog(wingstall::package_editor::Component* component_);
private:
    wingstall::package_editor::Component* component;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
