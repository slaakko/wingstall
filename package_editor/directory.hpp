// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_DIRECTORY_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_DIRECTORY_INCLUDED
#include <package_editor/rule.hpp>

namespace wingstall { namespace package_editor {

class Content;

class Directory : public Node
{
public:
    Directory();
    Directory(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
private:
    std::unique_ptr<Rules> rules;
    std::unique_ptr<Content> content;
};

class Content : public Node
{
public:
    Content();
    TreeViewNode* ToTreeViewNode(TreeView* view);
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_DIRECTORY_INCLUDED
