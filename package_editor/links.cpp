// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/links.hpp>
#include <package_editor/link_directory_dialog.hpp>
#include <package_editor/shortcut_dialog.hpp>
#include <package_editor/action.hpp>
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
    linkDirectories->SetParent(this);
    shortcuts.reset(new Shortcuts());
    shortcuts->SetParent(this);
}

Links::Links(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::links, "Links")
{
    linkDirectories.reset(new LinkDirectories());
    linkDirectories->SetParent(this);
    shortcuts.reset(new Shortcuts());
    shortcuts->SetParent(this);
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
    listView->AddColumn("Path", 400);
    for (const auto& linkDirectory : linkDirectories)
    {
        ListViewItem* item = listView->AddItem();
        linkDirectory->SetData(item, imageList);
    }
    return listView.release();
}

int LinkDirectories::Count() const
{
    return linkDirectories.size();
}

int LinkDirectories::IndexOf(const Node* child) const
{
    if (child->Kind() == NodeKind::linkDirectory)
    {
        const LinkDirectory* linkDirectory = static_cast<const LinkDirectory*>(child);
        int n = linkDirectories.size();
        for (int i = 0; i < n; ++i)
        {
            if (linkDirectories[i].get() == linkDirectory)
            {
                return i;
            }
        }
    }
    return -1;
}

Node* LinkDirectories::GetNode(int index) const
{
    if (index >= 0 && index < Count())
    {
        return linkDirectories[index].get();
    }
    return nullptr;
}

bool LinkDirectories::HasNode(const std::string& name) const
{
    for (const auto& linkDirectory : linkDirectories)
    {
        if (linkDirectory->Path() == name) return true;
    }
    return false;
}

std::unique_ptr<Node> LinkDirectories::RemoveChild(int index)
{
    if (index >= 0 && index < Count())
    {
        LinkDirectory* linkDirectory = linkDirectories[index].release();
        linkDirectories.erase(linkDirectories.begin() + index);
        return std::unique_ptr<Node>(linkDirectory);
    }
    return std::unique_ptr<Node>();
}

void LinkDirectories::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::linkDirectory && index >= 0 && index < Count())
    {
        LinkDirectory* linkDirectory = static_cast<LinkDirectory*>(child.release());
        linkDirectories.insert(linkDirectories.begin() + index, std::unique_ptr<LinkDirectory>(linkDirectory));
    }
    else
    {
        child.reset();
    }
}

void LinkDirectories::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::linkDirectory && index >= 0 && index < Count())
    {
        LinkDirectory* linkDirectory = static_cast<LinkDirectory*>(child.release());
        linkDirectories.insert(linkDirectories.begin() + index + 1, std::unique_ptr<LinkDirectory>(linkDirectory));
    }
    else
    {
        child.reset();
    }
}

void LinkDirectories::AddNew(NodeKind kind)
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        if (kind == NodeKind::linkDirectory)
        {
            LinkDirectoryDialog dialog("Add New Link Directory");
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                std::unique_ptr<LinkDirectory> linkDirectoryPtr(new LinkDirectory());
                LinkDirectory* linkDirectory = linkDirectoryPtr.get();
                dialog.GetData(linkDirectory);
                if (HasNode(linkDirectory->Path()))
                {
                    throw std::runtime_error("path not unique");
                }
                AddLinkDirectory(linkDirectoryPtr.release());
                Open();
                TreeViewNode* linkDirectoriesTreeViewNode = GetTreeViewNode();
                if (linkDirectoriesTreeViewNode)
                {
                    TreeView* treeView = linkDirectoriesTreeViewNode->GetTreeView();
                    if (treeView)
                    {
                        TreeViewNode* linkDirectoryTreeViewNode = linkDirectory->ToTreeViewNode(treeView);
                        linkDirectoriesTreeViewNode->AddChild(linkDirectoryTreeViewNode);
                        treeView->SetSelectedNode(linkDirectoryTreeViewNode);
                    }
                }
            }
        }
    }
}

void LinkDirectories::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addNewLinkDirectoryMenuItem(new MenuItem("Add New Link Directory"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewLinkDirectoryMenuItem.get(), this, NodeKind::linkDirectory)));
    contextMenu->AddMenuItem(addNewLinkDirectoryMenuItem.release());
}

LinkDirectory::LinkDirectory() : Node(NodeKind::linkDirectory, "Link Directory")
{
}

LinkDirectory::LinkDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::linkDirectory, "Link Directory")
{
    std::u32string pathAttr = element->GetAttribute(U"path");
    if (!pathAttr.empty())
    {
        SetPath(ToUtf8(pathAttr));
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

void LinkDirectory::SetPath(const std::string& path_)
{
    path = path_;
}

void LinkDirectory::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(0, path);
}

void LinkDirectory::Edit()
{
    LinkDirectoryDialog dialog("Edit Link Directory");
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        Node* parent = Parent();
        if (parent && parent->Kind() == NodeKind::linkDirectories)
        {
            LinkDirectories* linkDirectories = static_cast<LinkDirectories*>(parent);
            std::string prevValue = Path();
            dialog.SetData(this);
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                if (dialog.PathDirectoryValue() != prevValue)
                {
                    if (linkDirectories->HasNode(dialog.PathDirectoryValue()))
                    {
                        throw std::runtime_error("path not unique");
                    }
                }
                dialog.GetData(this);
                linkDirectories->Open();
                TreeViewNode* linkDirectoryTreeViewNode = GetTreeViewNode();
                if (linkDirectoryTreeViewNode)
                {
                    linkDirectoryTreeViewNode->SetText(Path());
                }
            }
        }
    }
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
    listView->AddColumn("Link File Path", 400);
    listView->AddColumn("Path", 400);
    for (const auto& shortcut : shortcuts)
    {
        ListViewItem* item = listView->AddItem();
        shortcut->SetData(item, imageList);
    }
    return listView.release();
}

int Shortcuts::Count() const
{
    return shortcuts.size();
}

int Shortcuts::IndexOf(const Node* child) const
{
    if (child->Kind() == NodeKind::shortcut)
    {
        const Shortcut* shortcut = static_cast<const Shortcut*>(child);
        int n = shortcuts.size();
        for (int i = 0; i < n; ++i)
        {
            if (shortcuts[i].get() == shortcut)
            {
                return i;
            }
        }
    }
    return -1;
}

Node* Shortcuts::GetNode(int index) const
{
    if (index >= 0 && index < Count())
    {
        return shortcuts[index].get();
    }
    return nullptr;
}

bool Shortcuts::HasNode(const std::string& name) const
{
    for (const auto& shortcut : shortcuts)
    {
        if (shortcut->LinkFilePath() == name)
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr<Node> Shortcuts::RemoveChild(int index)
{
    if (index >= 0 && index < Count())
    {
        Shortcut* shortcut = shortcuts[index].release();
        shortcuts.erase(shortcuts.begin() + index);
        return std::unique_ptr<Node>(shortcut);
    }
    return std::unique_ptr<Node>();
}

void Shortcuts::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::shortcut && index >= 0 && index < Count())
    {
        Shortcut* shortcut = static_cast<Shortcut*>(child.release());
        shortcuts.insert(shortcuts.begin() + index, std::unique_ptr<Shortcut>(shortcut));
    }
    else
    {
        child.reset();
    }
}

void Shortcuts::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::shortcut && index >= 0 && index < Count())
    {
        Shortcut* shortcut = static_cast<Shortcut*>(child.release());
        shortcuts.insert(shortcuts.begin() + index + 1, std::unique_ptr<Shortcut>(shortcut));
    }
    else
    {
        child.reset();
    }
}

void Shortcuts::AddNew(NodeKind kind)
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        if (kind == NodeKind::shortcut)
        {
            ShortcutDialog dialog("Add New Shortcut");
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                std::unique_ptr<Shortcut> shortcutPtr(new Shortcut());
                Shortcut* shortcut = shortcutPtr.get();
                dialog.GetData(shortcut);
                if (HasNode(shortcut->LinkFilePath()))
                {
                    throw std::runtime_error("link file path not unique");
                }
                AddShortcut(shortcutPtr.release());
                Open();
                TreeViewNode* shortcutsTreeViewNode = GetTreeViewNode();
                if (shortcutsTreeViewNode)
                {
                    TreeView* treeView = shortcutsTreeViewNode->GetTreeView();
                    if (treeView)
                    {
                        TreeViewNode* shortcutTreeViewNode = shortcut->ToTreeViewNode(treeView);
                        shortcutsTreeViewNode->AddChild(shortcutTreeViewNode);
                        treeView->SetSelectedNode(shortcutsTreeViewNode);
                    }
                }
            }
        }
    }
}

void Shortcuts::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addNewShortcutMenuItem(new MenuItem("Add New Shortcut"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewShortcutMenuItem.get(), this, NodeKind::shortcut)));
    contextMenu->AddMenuItem(addNewShortcutMenuItem.release());
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
        iconFilePath = ToUtf8(iconPathAttr);
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

void Shortcut::SetLinkFilePath(const std::string& linkFilePath_)
{
    linkFilePath = linkFilePath_;
}

void Shortcut::SetPath(const std::string& path_)
{
    path = path_;
}

void Shortcut::SetArguments(const std::string& arguments_)
{
    arguments = arguments_;
}

void Shortcut::SetWorkingDirectory(const std::string& workingDirectory_)
{
    workingDirectory = workingDirectory_;
}

void Shortcut::SetDescription(const std::string& description_)
{
    description = description_;
}

void Shortcut::SetIconFilePath(const std::string& iconFilePath_)
{
    iconFilePath = iconFilePath_;
}

void Shortcut::Edit()
{
    ShortcutDialog dialog("Edit Shortcut");
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        Node* parent = Parent();
        if (parent && parent->Kind() == NodeKind::shortcuts)
        {
            Shortcuts* shortcuts = static_cast<Shortcuts*>(parent);
            std::string prevValue = LinkFilePath();
            dialog.SetData(this);
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                if (dialog.LinkFilePath() != prevValue)
                {
                    if (shortcuts->HasNode(dialog.LinkFilePath()))
                    {
                        throw std::runtime_error("link file path not unique");
                    }
                }
                dialog.GetData(this);
                shortcuts->Open();
                TreeViewNode* shortcutTreeViewNode = GetTreeViewNode();
                if (shortcutTreeViewNode)
                {
                    shortcutTreeViewNode->SetText(LinkFilePath());
                }
            }
        }
    }

}

} } // wingstall::package_editor

