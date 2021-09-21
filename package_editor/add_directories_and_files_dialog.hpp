// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
#include <package_editor/component.hpp>
#include <wing/Window.hpp>
#include <wing/ListView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

Color DefaultAddDirectoriesAndFilesDialogListViewBorderColor();
void GetDirectoriesAndFiles(const std::string& dirPath, std::vector<std::u32string>& directories, std::vector<std::u32string>& files);
std::vector<std::u32string> Filter(const std::vector<std::u32string>& names, const std::vector<std::u32string>& exclude);
std::vector<std::u32string> Merge(const std::vector<std::u32string>& left, const std::vector<std::u32string>& right);

class AddDirectoriesAndFilesDialog : public Window
{
public:
    AddDirectoriesAndFilesDialog(wingstall::package_editor::Component* component_);
private:
    void AddDirectoriesAndFiles();
    void AddListViewEventHandlers();
    void ListViewItemClick(ListViewItemEventArgs& args);
    void ListViewItemRightClick(ListViewItemEventArgs& args);
    void ListViewItemDoubleClick(ListViewItemEventArgs& args);
    wingstall::package_editor::Component* component;
    ImageList* imageList;
    ListView* listView;
    Color borderColor;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
