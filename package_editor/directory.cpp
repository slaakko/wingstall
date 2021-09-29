// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/directory.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/configuration.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::util;
using namespace soulng::unicode;

Directory::Directory(const std::string& name_) : Node(NodeKind::directory, name_)
{
    rules.reset(new Rules());
    rules->SetParent(this);
    content.reset(new Content());
    content->SetParent(this);
}

Directory::Directory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::directory, std::string())
{
    rules.reset(new Rules());
    rules->SetParent(this);
    content.reset(new Content());
    content->SetParent(this);
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw PackageXMLException("'directory' element has no 'name' attribute", packageXMLFilePath, element);
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
                    rules->AddRule(rule);
                }
            }
        }
    }
}

sngxml::dom::Element* Directory::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"directory");
    element->SetAttribute(U"name", ToUtf32(Name()));
    rules->AddElements(element);
    return element;
}

TreeViewNode* Directory::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name());
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("folder.closed.bitmap"));
        node->SetExpandedImageIndex(imageList->GetImageIndex("folder.opened.bitmap"));
    }
    node->AddChild(rules->ToTreeViewNode(view));
    node->AddChild(content->ToTreeViewNode(view));
    return node;
}

Control* Directory::CreateView(ImageList* imageList)
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
    int nameColumnWidthValue = 400;
    View& view = GetConfiguredViewSettings().GetView(ViewName());
    ColumnWidth& nameColumnWidth = view.GetColumnWidth("Name");
    if (nameColumnWidth.IsDefined())
    {
        nameColumnWidthValue = nameColumnWidth.Get();
    }
    else
    {
        nameColumnWidth.Set(nameColumnWidthValue);
    }
    listView->AddColumn("Name", nameColumnWidthValue);
    ListViewItem* rulesItem = listView->AddItem();
    rules->SetData(rulesItem, imageList);
    ListViewItem* contentItem = listView->AddItem();
    content->SetData(contentItem, imageList);
    return listView.release();
}

std::string Directory::DirectoryPath() const
{
    std::string sourceRootDir = "C:/";
    Package* package = GetPackage();
    if (package && package->GetProperties())
    {
        sourceRootDir = package->GetProperties()->SourceRootDir();
    }
    return GetFullPath(Path::Combine(sourceRootDir, Name()));
}

int Directory::RuleCount() const
{
    return rules->RuleCount();
}

Rule* Directory::GetRule(int index) const
{
    return rules->GetRule(index);
}

Rule* Directory::GetRule(const std::string& name) const
{
    return rules->GetRule(name);
}

Node* Directory::GetRuleContainerNode()
{
    return rules.get();
}

} } // wingstall::package_editor
