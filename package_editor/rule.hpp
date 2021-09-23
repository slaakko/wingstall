// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_RULE_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_RULE_INCLUDED
#include <package_editor/node.hpp>
#include <sngxml/dom/Element.hpp>
#include <wing/TreeView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class Rule;

class Rules : public Node
{
public:
    Rules();
    void AddRule(Rule* rule);
    std::string ImageName() const override { return "rules.bitmap"; }
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
    bool CanAdd() const override { return true; }
    void AddNew(NodeKind kind) override;
    void AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions) override;
private:
    std::vector<std::unique_ptr<Rule>> rules;
};

enum class RuleKind
{
    exclude, include
};

enum class PathKind
{
    dir, file
};

class Rule : public Node
{
public:
    Rule();
    Rule(const std::string& namePattern_, RuleKind ruleKind_, PathKind pathKind_);
    Rule(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    std::string Text() const;
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    RuleKind GetRuleKind() const { return ruleKind; }
    void SetRuleKind(RuleKind ruleKind_) { ruleKind = ruleKind_; }
    PathKind GetPathKind() const { return pathKind; }
    void SetPathKind(PathKind pathKind_) { pathKind = pathKind_; }
    bool Cascade() const { return cascade; }
    void SetCascade(bool cascade_) { cascade = cascade_; }
    void AddRule(Rule* rule);
    void RemoveRules();
    std::string ImageName() const override;
    std::string CascadeStr() const;
    const std::string& Value() const { return value; }
    void SetValue(const std::string& value_);
    void SetData(ListViewItem* item, ImageList* imageList) override;
    int Count() const override;
    int IndexOf(const Node* child) const override;
    Node* GetNode(int index) const override;
    std::unique_ptr<Node> RemoveChild(int index) override;
    void InsertBefore(int index, std::unique_ptr<Node>&& child);
    void InsertAfter(int index, std::unique_ptr<Node>&& child);
    bool CanAdd() const override;
    void AddNew(NodeKind kind) override;
    void AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions) override;
    bool CanEdit() const override { return true; }
    void Edit() override;
private:
    RuleKind ruleKind;
    PathKind pathKind;
    bool cascade;
    std::vector<std::unique_ptr<Rule>> rules;
    std::string value;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_RULE_INCLUDED
