// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_DIRECTORY_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_DIRECTORY_INCLUDED
#include <package_editor/content.hpp>

namespace wingstall { namespace package_editor {

class Directory : public Node
{
public:
    Directory(const std::string& name_);
    Directory(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    std::string ImageName() const override { return "folder.closed.bitmap"; }
    std::string ViewName() const override { return "directory_view"; }
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string DirectoryPath() const override;
    int RuleCount() const override;
    Rule* GetRule(int index) const override;
    Rule* GetRule(const std::string& name) const override;
    Node* GetRuleContainerNode() override;
private:
    std::unique_ptr<Rules> rules;
    std::unique_ptr<Content> content;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_DIRECTORY_INCLUDED
