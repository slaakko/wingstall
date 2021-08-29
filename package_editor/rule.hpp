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
    TreeViewNode* ToTreeViewNode(TreeView* view);
private:
    std::vector<std::unique_ptr<Rule>> rules;
};

enum class RuleKind
{
    include, exclude
};

enum class PathKind
{
    dir, file
};

class Rule : public Node
{
public:
    Rule(RuleKind ruleKind_, PathKind pathKind_);
    Rule(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    std::string Text() const;
    TreeViewNode* ToTreeViewNode(TreeView* view);
    RuleKind GetRuleKind() const { return ruleKind; }
    PathKind GetPathKind() const { return pathKind; }
    bool Cascade() const { return cascade; }
    void SetCascade(bool cascade_) { cascade = cascade_; }
    void AddRule(Rule* rule);
private:
    std::string ImageName() const;
    RuleKind ruleKind;
    PathKind pathKind;
    bool cascade;
    std::unique_ptr<Rules> rules;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_RULE_INCLUDED
