// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/rule.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/action.hpp>
#include <package_editor/rule_dialog.hpp>
#include <package_editor/configuration.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;
using namespace soulng::util;

Rules::Rules() : Node(NodeKind::rules, "Rules")
{
}

void Rules::AddElements(sngxml::dom::Element* directoryElement)
{
    for (const auto& rule : rules)
    {
        directoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rule->ToXml()));
    }
}

void Rules::AddRule(Rule* rule)
{
    rule->SetParent(this);
    rules.push_back(std::unique_ptr<Rule>(rule));
    TreeViewNode* rulesTreeViewNode = GetTreeViewNode();
    if (rulesTreeViewNode)
    {
        TreeView* treeView = rulesTreeViewNode->GetTreeView();
        if (treeView)
        {
            TreeViewNode* ruleTreeViewNode = rule->ToTreeViewNode(treeView);
            rulesTreeViewNode->AddChild(ruleTreeViewNode);
            treeView->SetSelectedNode(rulesTreeViewNode);
        }
    }
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
    listView->SetData(this);
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
        listView->ColumnWidthChanged().AddHandler(mainWindow, &MainWindow::ListViewColumnWidthChanged);
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);

    int kindColumnWidthValue = 200; 
    View& view = GetConfiguredViewSettings().GetView(ViewName());
    ColumnWidth& kindColumnWidth = view.GetColumnWidth("Kind");
    if (kindColumnWidth.IsDefined())
    {
        kindColumnWidthValue = kindColumnWidth.Get();
    }
    else
    {
        kindColumnWidth.Set(kindColumnWidthValue);
    }
    listView->AddColumn("Kind", kindColumnWidthValue);
    int nameColumnWidthValue = 200;
    ColumnWidth& nameColumnWidth = view.GetColumnWidth("Name/Pattern");
    if (nameColumnWidth.IsDefined())
    {
        nameColumnWidthValue = nameColumnWidth.Get();
    }
    else
    {
        nameColumnWidth.Set(nameColumnWidthValue);
    }
    listView->AddColumn("Name/Pattern", nameColumnWidthValue);
    int cascadeColumnWidthValue = 200;
    ColumnWidth& cascadeColumnWidth = view.GetColumnWidth("Cascade");
    if (cascadeColumnWidth.IsDefined())
    {
        cascadeColumnWidthValue = cascadeColumnWidth.Get();
    }
    else
    {
        cascadeColumnWidth.Set(cascadeColumnWidthValue);
    }
    listView->AddColumn("Cascade", cascadeColumnWidthValue);
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

void Rules::AddNew(NodeKind kind)
{
    if (kind == NodeKind::rule)
    {
        MainWindow* mainWindow = GetMainWindow();
        if (mainWindow)
        {
            RuleDialog ruleDialog("Add New Rule");
            if (ruleDialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                Package* package = GetPackage();
                if (package)
                {
                    package->SetDirty();
                }
                Rule* rule = new Rule();
                ruleDialog.GetData(rule);
                AddRule(rule);
                OpenAndExpand();
            }
        }
    }
}

void Rules::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addNewRuleMenuItem(new MenuItem("Add New Rule"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewRuleMenuItem.get(), this, NodeKind::rule)));
    contextMenu->AddMenuItem(addNewRuleMenuItem.release());
    std::unique_ptr<MenuItem> addVSRulesMenuItem(new MenuItem("Add Visual C++ Source Directory Rules"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddVSRulesAction(addVSRulesMenuItem.get(), this)));
    contextMenu->AddMenuItem(addVSRulesMenuItem.release());
}

bool Rules::CanClear() const
{
    return !rules.empty();
}

void Rules::Clear()
{
    if (!rules.empty())
    {
        TreeViewNode* rulesTreeViewNode = GetTreeViewNode();
        if (rulesTreeViewNode)
        {
            TreeView* treeView = rulesTreeViewNode->GetTreeView();
            if (treeView)
            {
                rulesTreeViewNode->RemoveChildren();
            }
        }
        rules.clear();
        Parent()->OpenAndExpand();
        Package* package = GetPackage();
        if (package)
        {
            package->SetDirty();
        }
    }
}

int Rules::RuleCount() const
{
    return rules.size();
}

Rule* Rules::GetRule(int index) const
{
    return rules[index].get();
}

Rule* Rules::GetRule(const std::string& name) const
{
    for (const auto& rule : rules)
    {
        if (rule->Value() == name) return rule.get();
    }
    return nullptr;
}

Rule::Rule() : Node(NodeKind::rule, std::string()), ruleKind(), pathKind(), cascade(false), compiled(false)
{
}

Rule::Rule(const std::string& namePattern_, RuleKind ruleKind_, PathKind pathKind_) : Node(NodeKind::rule, std::string()), ruleKind(ruleKind_), pathKind(pathKind_), cascade(false), compiled(false)
{
    if (ruleKind == RuleKind::exclude)
    {
        SetName("Exclude");
    }
    else if (ruleKind == RuleKind::include)
    {
        SetName("Include");
    }
    if (pathKind == PathKind::dir)
    {
        SetName(Name() + " Directory");
    }
    else if (pathKind == PathKind::file)
    {
        SetName(Name() + " File");
    }
    SetValue(namePattern_);
}

Rule::Rule(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::rule, std::string()), ruleKind(), pathKind(), cascade(false), compiled(false)
{
    if (element->Name() == U"exclude")
    {
        ruleKind = RuleKind::exclude;
        SetName("Exclude");
    }
    else if (element->Name() == U"include")
    {
        ruleKind = RuleKind::include;
        SetName("Include");
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
        SetValue(ToUtf8(dirAttr));
    }
    else
    {
        std::u32string fileAttr = element->GetAttribute(U"file");
        if (!fileAttr.empty())
        {
            pathKind = PathKind::file;
            SetName(Name() + " File");
            SetValue(ToUtf8(fileAttr));
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

sngxml::dom::Element* Rule::ToXml() const
{
    std::u32string name;
    if (ruleKind == RuleKind::exclude)
    {
        name = U"exclude";
    }
    else if (ruleKind == RuleKind::include)
    {
        name = U"include";
    }
    sngxml::dom::Element* element = new sngxml::dom::Element(name);
    if (pathKind == PathKind::dir)
    {
        element->SetAttribute(U"dir", ToUtf32(Value()));
    }
    else if (pathKind == PathKind::file)
    {
        element->SetAttribute(U"file", ToUtf32(Value()));
    }
    if (cascade)
    {
        element->SetAttribute(U"cascade", U"true");
    }
    for (const auto& rule : rules)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(rule->ToXml()));
    }
    return element;
}

void Rule::SetValue(const std::string& value_)
{
    value = value_;
}

std::string Rule::Text() const
{
    std::string text;
    if (ruleKind == RuleKind::exclude)
    {
        text.append("Exclude ");
    }
    else if (ruleKind == RuleKind::include)
    {
        text.append("Include ");
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
    if (ruleKind == RuleKind::exclude)
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
    else if (ruleKind == RuleKind::include)
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
    TreeViewNode* thisViewNode = GetTreeViewNode();
    if (thisViewNode)
    {
        TreeView* treeView = thisViewNode->GetTreeView();
        if (treeView)
        {
            TreeViewNode* ruleTreeViewNode = rule->ToTreeViewNode(treeView);
            thisViewNode->AddChild(ruleTreeViewNode);
            treeView->SetSelectedNode(thisViewNode);
        }
    }
}

void Rule::RemoveRules()
{
    rules.clear();
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
    listView->SetData(this);
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
        listView->ColumnWidthChanged().AddHandler(mainWindow, &MainWindow::ListViewColumnWidthChanged);
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    int kindColumnWidthValue = 200;
    View& view = GetConfiguredViewSettings().GetView(ViewName());
    ColumnWidth& kindColumnWidth = view.GetColumnWidth("Kind");
    if (kindColumnWidth.IsDefined())
    {
        kindColumnWidthValue = kindColumnWidth.Get();
    }
    else
    {
        kindColumnWidth.Set(kindColumnWidthValue);
    }
    listView->AddColumn("Kind", kindColumnWidthValue);
    int nameColumnWidthValue = 200;
    ColumnWidth& nameColumnWidth = view.GetColumnWidth("Name/Pattern");
    if (nameColumnWidth.IsDefined())
    {
        nameColumnWidthValue = nameColumnWidth.Get();
    }
    else
    {
        nameColumnWidth.Set(nameColumnWidthValue);
    }
    listView->AddColumn("Name/Pattern", nameColumnWidthValue);

    int cascadeColumnWidthValue = 200;
    ColumnWidth& cascadeColumnWidth = view.GetColumnWidth("Cascade");
    if (cascadeColumnWidth.IsDefined())
    {
        cascadeColumnWidthValue = cascadeColumnWidth.Get();
    }
    else
    {
        cascadeColumnWidth.Set(cascadeColumnWidthValue);
    }
    listView->AddColumn("Cascade", cascadeColumnWidthValue);
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

bool Rule::Leaf() const
{
    return rules.empty();
}

bool Rule::CanAdd() const
{
    if (ruleKind == RuleKind::include && pathKind == PathKind::dir)
    {
        return true;
    }
    return false;
}

void Rule::AddNew(NodeKind kind)
{
    if (kind == NodeKind::rule)
    {
        MainWindow* mainWindow = GetMainWindow();
        if (mainWindow)
        {
            RuleDialog ruleDialog("Add New Rule");
            if (ruleDialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                Package* package = GetPackage();
                if (package)
                {
                    package->SetDirty();
                }
                Rule* rule = new Rule();
                ruleDialog.GetData(rule);
                AddRule(rule);
                OpenAndExpand();
            }
        }
    }
}

void Rule::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addNewRuleMenuItem(new MenuItem("Add New Rule"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewRuleMenuItem.get(), this, NodeKind::rule)));
    contextMenu->AddMenuItem(addNewRuleMenuItem.release());
}

void Rule::Edit()
{
    RuleDialog dialog("Edit Rule");
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        dialog.SetData(this);
        if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
        {
            Package* package = GetPackage();
            if (package)
            {
                package->SetDirty();
            }
            dialog.GetData(this);
            compiled = false;
            Node* parent = Parent();
            if (parent)
            {
                parent->OpenAndExpand();
            }
            TreeViewNode* ruleTreeViewNode = GetTreeViewNode();
            if (ruleTreeViewNode)
            {
                ruleTreeViewNode->SetText(Text());
            }
        }
    }
}

int Rule::RuleCount() const
{
    return rules.size();
}

Rule* Rule::GetRule(int index) const
{
    return rules[index].get();
}

Rule* Rule::GetRule(const std::string& name) const
{
    for (const auto& rule : rules)
    {
        if (rule->Value() == name) return rule.get();
    }
    return nullptr;
}

bool Rule::Matches(const std::u32string& name) 
{
    if (!compiled)
    {
        Package* package = GetPackage();
        if (package)
        {
            nfa = soulng::rex::CompileFilePattern(package->GetContext(), ToUtf32(Value()));
            compiled = true;
        }
    }
    if (compiled)
    {
        return soulng::rex::PatternMatch(name, nfa);
    }
    return false;
}

Rule* MakeExcludeDebugDirRule()
{
    Rule* rule = new Rule("Debug", RuleKind::exclude, PathKind::dir);
    rule->SetCascade(true);
    return rule;
}

Rule* MakeExcludeReleaseDirRule()
{
    Rule* rule = new Rule("Release", RuleKind::exclude, PathKind::dir);
    rule->SetCascade(true);
    return rule;
}

Rule* MakeExcludeX64DirRule()
{
    Rule* rule = new Rule("x64", RuleKind::exclude, PathKind::dir);
    rule->SetCascade(true);
    return rule;
}

Rule* MakeExcludeVSDirRule()
{
    Rule* rule = new Rule(".vs", RuleKind::exclude, PathKind::dir);
    rule->SetCascade(true);
    return rule;
}

} } // wingstall::package_editor
