// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/rule.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;
using namespace soulng::util;


Rules::Rules() : Node(NodeKind::rules, "Rules")
{
}

void Rules::AddRule(Rule* rule)
{
    rule->SetParent(this);
    rules.push_back(std::unique_ptr<Rule>(rule));
}

TreeViewNode* Rules::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Rules");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("rules.bitmap"));
    }
    for (const auto& rule : rules)
    {
        node->AddChild(rule->ToTreeViewNode(view));
    }
    return node;
}

Control* Rules::CreateView(ImageList* imageList)
{
    std::unique_ptr<ListView> listView(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Kind", 200);
    listView->AddColumn("Name/Pattern", 200);
    listView->AddColumn("Cascade", 200);
    for (const auto& rule : rules)
    {
        ListViewItem* item = listView->AddItem();
        rule->SetData(item, imageList);
    }
    return listView.release();
}

int Rules::Count() const
{
    return rules.size();
}

int Rules::IndexOf(const Node* child) const
{
    if (child->Kind() == NodeKind::rule)
    {
        const Rule* rule = static_cast<const Rule*>(child);
        int n = rules.size();
        for (int i = 0; i < n; ++i)
        {
            if (rules[i].get() == rule)
            {
                return i;
            }
        }
    }
    return -1;
}

Node* Rules::GetNode(int index) const
{
    if (index >= 0 && index < Count())
    {
        return rules[index].get();
    }
    return nullptr;
}

std::unique_ptr<Node> Rules::RemoveChild(int index)
{
    if (index >= 0 && index < Count())
    {
        Rule* rule = rules[index].release();
        rules.erase(rules.begin() + index);
        return std::unique_ptr<Node>(rule);
    }
    return std::unique_ptr<Node>();
}

void Rules::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::rule && index >= 0 && index < Count())
    {
        Rule* rule = static_cast<Rule*>(child.release());
        rules.insert(rules.begin() + index, std::unique_ptr<Rule>(rule));
    }
    else
    {
        child.reset();
    }
}

void Rules::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::rule && index >= 0 && index < Count())
    {
        Rule* rule = static_cast<Rule*>(child.release());
        rules.insert(rules.begin() + index + 1, std::unique_ptr<Rule>(rule));
    }
    else
    {
        child.reset();
    }
}

Rule::Rule(RuleKind ruleKind_, PathKind pathKind_) : Node(NodeKind::rule, std::string()), ruleKind(ruleKind_), pathKind(pathKind_), cascade(false)
{
}

Rule::Rule(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::rule, std::string()), ruleKind(), pathKind(), cascade(false)
{
    if (element->Name() == U"include")
    {
        ruleKind = RuleKind::include;
        SetName("Include");
    }
    else if (element->Name() == U"exclude")
    {
        ruleKind = RuleKind::exclude;
        SetName("Exclude");
    }
    else
    {
        throw PackageXMLException("unknown rule kind '" + ToUtf8(element->Name()) + "'", packageXMLFilePath, element);
    }
    std::u32string dirAttr = element->GetAttribute(U"dir");
    if (!dirAttr.empty())
    {
        pathKind = PathKind::dir;
        std::u32string fileAttr = element->GetAttribute(U"file");
        if (!fileAttr.empty())
        {
            throw PackageXMLException("rule '" + ToUtf8(element->Name()) + "' has both 'dir' and 'file' attributes", packageXMLFilePath, element);
        }
        SetName(Name() + " Directory");
        value = ToUtf8(dirAttr);
    }
    else
    {
        std::u32string fileAttr = element->GetAttribute(U"file");
        if (!fileAttr.empty())
        {
            pathKind = PathKind::file;
            SetName(Name() + " File");
            value = ToUtf8(fileAttr);
        }
        else
        {
            throw PackageXMLException("rule '" + ToUtf8(element->Name()) + "' must have either 'dir' or 'file' attributes", packageXMLFilePath, element);
        }
    }
    if (ruleKind == RuleKind::exclude)
    {
        std::u32string cascadeAttr = element->GetAttribute(U"cascade");
        if (!cascadeAttr.empty())
        {
            try
            {
                cascade = ParseBool(ToUtf8(cascadeAttr));
            }
            catch (const std::exception& ex)
            {
                throw PackageXMLException("rule '" + ToUtf8(element->Name()) + "' has invalid 'cascade' attribute value: " + std::string(ex.what()), packageXMLFilePath, element);
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> ruleObject = sngxml::xpath::Evaluate(U"*", element);
    if (ruleObject)
    {
        if (ruleObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(ruleObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Rule* rule = new Rule(packageXMLFilePath, element);
                    AddRule(rule);
                }
            }
        }
    }
}

std::string Rule::Text() const
{
    std::string text;
    if (ruleKind == RuleKind::include)
    {
        text.append("Include ");
    }
    else if (ruleKind == RuleKind::exclude)
    {
        text.append("Exclude ");
    }
    if (pathKind == PathKind::dir)
    {
        text.append("Directory ");
    }
    else if (pathKind == PathKind::file)
    {
        text.append("File ");
    }
    text.append("'").append(value).append("'");
    if (cascade)
    {
        text.append(" (cascade)");
    }
    return text;
}

std::string Rule::ImageName() const
{
    if (ruleKind == RuleKind::include)
    {
        if (pathKind == PathKind::dir)
        {
            return "add.folder.bitmap";
        }
        else if (pathKind == PathKind::file)
        {
            return "add.file.bitmap";
        }
    }
    else if (ruleKind == RuleKind::exclude)
    {
        if (pathKind == PathKind::dir)
        {
            if (cascade)
            {
                return "delete.folder.cascade.bitmap";
            }
            else
            {
                return "delete.folder.bitmap";
            }
        }
        else if (pathKind == PathKind::file)
        {
            if (cascade)
            {
                return "delete.file.cascade.bitmap";
            }
            else
            {
                return "delete.file.bitmap";
            }
        }
    }
    return std::string();
}

std::string Rule::CascadeStr() const
{
    if (cascade)
    {
        return "true";
    }
    else
    {
        return "false";
    }
}

TreeViewNode* Rule::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Text());
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex(ImageName()));
    }
    for (const auto& rule : rules)
    {
        node->AddChild(rule->ToTreeViewNode(view));
    }
    return node;
}

void Rule::AddRule(Rule* rule)
{
    rule->SetParent(this);
    rules.push_back(std::unique_ptr<Rule>(rule));
}

void Rule::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(1, Value());
    if (ruleKind == RuleKind::exclude)
    {
        item->SetColumnValue(2, CascadeStr());
    }
}

Control* Rule::CreateView(ImageList* imageList)
{
    std::unique_ptr<ListView> listView(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Kind", 200);
    listView->AddColumn("Name/Pattern", 200);
    listView->AddColumn("Cascade", 200);
    for (const auto& rule : rules)
    {
        ListViewItem* item = listView->AddItem();
        rule->SetData(item, imageList);
    }
    return listView.release();
}

int Rule::Count() const
{
    return rules.size();
}

int Rule::IndexOf(const Node* child) const
{
    if (child->Kind() == NodeKind::rule)
    {
        const Rule* rule = static_cast<const Rule*>(child);
        int n = rules.size();
        for (int i = 0; i < n; ++i)
        {
            if (rules[i].get() == rule)
            {
                return i;
            }
        }
    }
    return -1;
}

Node* Rule::GetNode(int index) const
{
    if (index >= 0 && index < Count())
    {
        return rules[index].get();
    }
    return nullptr;
}

std::unique_ptr<Node> Rule::RemoveChild(int index)
{
    if (index >= 0 && index < Count())
    {
        Rule* rule = rules[index].release();
        rules.erase(rules.begin() + index);
        return std::unique_ptr<Node>(rule);
    }
    return std::unique_ptr<Node>();
}

void Rule::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::rule && index >= 0 && index < Count())
    {
        Rule* rule = static_cast<Rule*>(child.release());
        rules.insert(rules.begin() + index, std::unique_ptr<Rule>(rule));
    }
    else
    {
        child.reset();
    }
}

void Rule::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::rule && index >= 0 && index < Count())
    {
        Rule* rule = static_cast<Rule*>(child.release());
        rules.insert(rules.begin() + index + 1, std::unique_ptr<Rule>(rule));
    }
    else
    {
        child.reset();
    }
}

} } // wingstall::package_editor
