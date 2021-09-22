// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED
#include <package_editor/component.hpp>
#include <wing/Button.hpp>
#include <wing/Window.hpp>
#include <wing/ListView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

Color DefaultAddDirectoriesAndFilesDialogListViewBorderColor();
void GetDirectoriesAndFiles(const std::string& dirPath, std::vector<std::u32string>& directories, std::vector<std::u32string>& files);
std::vector<std::u32string> Filter(const std::vector<std::u32string>& names, const std::vector<std::u32string>& exclude);
std::vector<std::u32string> Merge(const std::vector<std::u32string>& left, const std::vector<std::u32string>& right);

struct DialogListViewItemData
{
    enum Kind
    {
        directory, file
    };
    DialogListViewItemData(Kind kind_, int index_) : kind(kind_), index(index_) {}
    Kind kind;
    int index;
};

class AddDirectoriesAndFilesDialog : public Window
{
public:
    AddDirectoriesAndFilesDialog(wingstall::package_editor::Component* component_);
    void GetSelectedDirectoriesAndFiles(std::vector<std::u32string>& selectedDirectories, std::vector<std::u32string>& selectedFiles) const;
private:
    void AddDirectoriesAndFiles();
    void AddListViewEventHandlers();
    void ListViewItemClick(ListViewItemEventArgs& args);
    void ListViewItemDoubleClick(ListViewItemEventArgs& args);
    wingstall::package_editor::Component* component;
    std::vector<std::u32string> directoryNames;
    std::vector<std::u32string> fileNames;
    std::vector<std::unique_ptr<DialogListViewItemData>> data;
    ImageList* imageList;
    ListView* listView;
    Color borderColor;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ADD_DIRECTORIES_AND_FILES_DIALOG_INCLUDED