// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/links.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

Links::Links() : Node(NodeKind::links, "Links")
{
    linkDirectories.reset(new LinkDirectories());
    shortcuts.reset(new Shortcuts());
}

Links::Links(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::links, "Links")
{
    linkDirectories.reset(new LinkDirectories());
    shortcuts.reset(new Shortcuts());
    std::unique_ptr<sngxml::xpath::XPathObject> linkDirectoryObject = sngxml::xpath::Evaluate(U"linkDirectory", element);
    if (linkDirectoryObject)
    {
        if (linkDirectoryObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(linkDirectoryObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    LinkDirectory* linkDirectory = new LinkDirectory(packageXMLFilePath, element);
                    linkDirectories->AddLinkDirectory(linkDirectory);
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> linkObject = sngxml::xpath::Evaluate(U"link", element);
    if (linkObject)
    {
        if (linkObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(linkObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Shortcut* shortcut = new Shortcut(packageXMLFilePath, element);
                    shortcuts->AddShortcut(shortcut);
                }
            }
        }
    }
}

TreeViewNode* Links::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Links");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("links.bitmap"));
    }
    node->AddChild(linkDirectories->ToTreeViewNode(view));
    node->AddChild(shortcuts->ToTreeViewNode(view));
    return node;
}

Control* Links::CreateView(ImageList* imageList)
{
    std::unique_ptr<ListView> listView(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Name", 200);
    ListViewItem* linkDirectoriesItem = listView->AddItem();
    linkDirectories->SetData(linkDirectoriesItem, imageList);
    ListViewItem* shortcutsItem = listView->AddItem();
    shortcuts->SetData(shortcutsItem, imageList);
    return listView.release();

}

LinkDirectories::LinkDirectories() : Node(NodeKind::linkDirectories, "Link Directories")
{
}

void LinkDirectories::AddLinkDirectory(LinkDirectory* linkDirectory)
{
    linkDirectory->SetParent(this);
    linkDirectories.push_back(std::unique_ptr<LinkDirectory>(linkDirectory));
}

TreeViewNode* LinkDirectories::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Link Directories");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("linked.folder.closed.bitmap"));
    }
    for (const auto& linkDirectory : linkDirectories)
    {
        node->AddChild(linkDirectory->ToTreeViewNode(view));
    }
    return node;
}

Control* LinkDirectories::CreateView(ImageList* imageList)
{
    std::unique_ptr<ListView> listView(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Path", 200);
    for (const auto& linkDirectory : linkDirectories)
    {
        ListViewItem* item = listView->AddItem();
        linkDirectory->SetData(item, imageList);
    }
    return listView.release();
}

LinkDirectory::LinkDirectory() : Node(NodeKind::linkDirectory, "Link Directory")
{
}

LinkDirectory::LinkDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::linkDirectory, "Link Directory")
{
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        path = ToUtf8(pathAttr);
    }
    else
    {
        throw PackageXMLException("'linkDirectory' element has no 'path' attribute", packageXMLFilePath, element);
    }
}

TreeViewNode* LinkDirectory::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(path);
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("linked.folder.closed.bitmap"));
    }
    return node;
}

void LinkDirectory::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(0, path);
}

Shortcuts::Shortcuts() : Node(NodeKind::shortcuts, "Shortcuts")
{
}

void Shortcuts::AddShortcut(Shortcut* shortcut)
{
    shortcut->SetParent(this);
    shortcuts.push_back(std::unique_ptr<Shortcut>(shortcut));
}

TreeViewNode* Shortcuts::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Shortcuts");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("shortcut.bitmap"));
    }
    for (const auto& shortcut : shortcuts)
    {
        node->AddChild(shortcut->ToTreeViewNode(view));
    }
    return node;
}

Control* Shortcuts::CreateView(ImageList* imageList)
{
    std::unique_ptr<ListView> listView(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Link File Path", 200);
    listView->AddColumn("Path", 200);
    for (const auto& shortcut : shortcuts)
    {
        ListViewItem* item = listView->AddItem();
        shortcut->SetData(item, imageList);
    }
    return listView.release();
}

Shortcut::Shortcut() : Node(NodeKind::shortcut, "Shortcut"), iconIndex(0)
{
}

Shortcut::Shortcut(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::shortcut, "Shortcut"), iconIndex(0)
{
    std::u32string linkFilePathAttr = element->GetAttribute(U"linkFilePath");
    if (!linkFilePathAttr.empty())
    {
        linkFilePath = ToUtf8(linkFilePathAttr);
    }
    else
    {
        throw PackageXMLException("'shortcut' element has no 'linkFilePath' attribute", packageXMLFilePath, element);
    }
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        path = ToUtf8(pathAttr);
    }
    else
    {
        throw PackageXMLException("'shortcut' element has no 'path' attribute", packageXMLFilePath, element);
    }
    std::u32string argumentsAttr = element->GetAttribute(U"arguments");
    if (!argumentsAttr.empty())
    {
        arguments = ToUtf8(argumentsAttr);
    }
    std::u32string workingDirectoryAttr = element->GetAttribute(U"workingDirectory");
    if (!workingDirectoryAttr.empty())
    {
        workingDirectory = ToUtf8(workingDirectoryAttr);
    }
    std::u32string descriptionAttr = element->GetAttribute(U"description");
    if (!descriptionAttr.empty())
    {
        description = ToUtf8(descriptionAttr);
    }
    std::u32string iconPathAttr = element->GetAttribute(U"iconPath");
    if (!iconPathAttr.empty())
    {
        iconPath = ToUtf8(iconPathAttr);
    }
    std::u32string iconIndexAttr = element->GetAttribute(U"iconIndex");
    if (!iconIndexAttr.empty())
    {
        iconIndex = boost::lexical_cast<int>(ToUtf8(iconIndexAttr));
    }
}

TreeViewNode* Shortcut::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(linkFilePath);
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("shortcut.bitmap"));
    }
    return node;
}

void Shortcut::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(0, linkFilePath);
    item->SetColumnValue(1, path);
}

} } // wingstall::package_editor

