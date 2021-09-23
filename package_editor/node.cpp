// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/node.hpp>
#include <package_editor/package_content_view.hpp>
#include <package_editor/package_explorer.hpp>
#include <package_editor/action.hpp>

namespace wingstall { namespace package_editor {

Node::Node(NodeKind kind_, const std::string& name_) : kind(kind_), name(name_), parent(nullptr), treeViewNode(nullptr), listViewItem(nullptr)
{
}

Node::~Node()
{
}

MainWindow* Node::GetMainWindow() 
{
    Package* package = GetPackage();
    if (package)
    {
        PackageContentView* view = package->View();
        if (view)
        {
            return view->GetMainWindow();
        }
    }
    return nullptr;
}

void Node::SetName(const std::string& name_)
{
    name = name_;
}

Package* Node::GetPackage() const
{
    if (parent)
    {
        return parent->GetPackage();
    }
    return nullptr;
}

Control* Node::CreateView(ImageList* imageList)
{
    return nullptr;
}

std::string Node::ImageName() const
{
    return std::string();
}

void Node::SetData(ListViewItem* item, ImageList* imageList)
{
    listViewItem = item;
    item->SetData(this);
    item->SetColumnValue(0, Name());
    std::string imageName = ImageName();
    if (!imageName.empty())
    {
        item->SetImageIndex(imageList->GetImageIndex(imageName));
        if (CanDisable())
        {
            item->SetDisabledImageIndex(imageList->GetDisabledImageIndex(imageName));
            if (IsDisabled())
            {
                item->SetState(ListViewItemState::disabled);
            }
        }
    }
}

void Node::Explore()
{
    Package* package = GetPackage();
    if (package)
    {
        PackageExplorer* explorer = package->Explorer();
        if (explorer)
        {
            explorer->Open(this);
        }
    }
}

void Node::ViewContent()
{
    Package* package = GetPackage();
    if (package)
    {
        PackageContentView* view = package->View();
        if (view)
        {
            view->ViewContent(this);
        }
    }
}

void Node::Open()
{
    Explore();
    ViewContent();
}

void Node::Select()
{
    if (listViewItem)
    {
        ListView* view = listViewItem->View();
        if (view)
        {
            view->SetSelectedItem(listViewItem);
        }
    }
    if (treeViewNode)
    {
        TreeView* treeView = treeViewNode->GetTreeView();
        if (treeView)
        {
            treeView->SetSelectedNode(treeViewNode);
        }
    }
}

int Node::Count() const
{
    return 0;
}

int Node::IndexOf(const Node* child) const
{
    return -1;
}

Node* Node::GetNode(int index) const
{
    return nullptr;
}

bool Node::HasNode(const std::string& name) const
{
    return false;
}

void Node::AddNew(NodeKind kind)
{
}


void Node::Edit()
{
}

bool Node::CanRemove() const
{
    if (parent)
    {
        int index = parent->IndexOf(this);
        return index >= 0 && index < parent->Count();
    }
    return false;
}

bool Node::CanMoveUp() const
{
    if (parent)
    {
        return parent->CanMoveUp(this);
    }
    return false;
}

bool Node::CanMoveDown() const
{
    if (parent)
    {
        return parent->CanMoveDown(this);
    }
    return false;
}

void Node::Remove()
{
    Node* p = parent;
    if (p)
    {
        int index = p->IndexOf(this);
        if (treeViewNode && treeViewNode->Parent())
        {
            treeViewNode->Parent()->RemoveChild(treeViewNode);
        }
        p->RemoveChild(index);
        p->Open();
    }
}

void Node::MoveUp()
{
    if (parent)
    {
        int index = parent->IndexOf(this);
        if (index > 0 && index < parent->Count())
        {
            std::unique_ptr<Node> thisNode = parent->RemoveChild(index);
            Node* node = thisNode.get();
            parent->InsertBefore(index - 1, std::move(thisNode));
            parent->Open();
            node->Select();
            TreeViewNode* p = treeViewNode->Parent();
            if (treeViewNode && p)
            {
                TreeViewNode* before = static_cast<TreeViewNode*>(treeViewNode->PrevSibling());
                if (before)
                {
                    TreeView* treeView = treeViewNode->GetTreeView();
                    p->InsertChildBefore(treeViewNode, before);
                    if (treeView)
                    {
                        treeView->SetSelectedNode(p);
                    }
                }
            }
        }
    }
}

void Node::MoveDown()
{
    if (parent)
    {
        int index = parent->IndexOf(this);
        if (index >= 0 && index < parent->Count())
        {
            std::unique_ptr<Node> thisNode = parent->RemoveChild(index);
            Node* node = thisNode.get();
            parent->InsertAfter(index, std::move(thisNode));
            parent->Open();
            node->Select();
            TreeViewNode* p = treeViewNode->Parent();
            if (treeViewNode && p)
            {
                TreeViewNode* after = static_cast<TreeViewNode*>(treeViewNode->NextSibling());
                if (after)
                {
                    TreeView* treeView = treeViewNode->GetTreeView();
                    p->InsertChildAfter(treeViewNode, after);
                    if (treeView)
                    {
                        treeView->SetSelectedNode(p);
                    }
                }
            }
        }
    }
}

void Node::AddMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions, MenuItemsKind menuItemsKind)
{
    if (menuItemsKind == MenuItemsKind::newMenuItems)
    {
        if (CanAdd())
        {
            AddAddNewMenuItems(contextMenu, clickActions);
        }
    }
    else if (menuItemsKind == MenuItemsKind::allMenuItems)
    {
        if (CanOpen())
        {
            std::unique_ptr<MenuItem> openMenuItem(new MenuItem("Open"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new OpenAction(openMenuItem.get(), this)));
            contextMenu->AddMenuItem(openMenuItem.release());
        }
        if (CanAdd())
        {
            AddAddNewMenuItems(contextMenu, clickActions);
        }
        if (CanEdit())
        {
            std::unique_ptr<MenuItem> editMenuItem(new MenuItem("Edit"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new EditAction(editMenuItem.get(), this)));
            contextMenu->AddMenuItem(editMenuItem.release());
        }
        if (CanRemove())
        {
            std::unique_ptr<MenuItem> removeMenuItem(new MenuItem("Remove"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new RemoveAction(removeMenuItem.get(), this)));
            contextMenu->AddMenuItem(removeMenuItem.release());
        }
        if (CanMoveUp())
        {
            std::unique_ptr<MenuItem> moveUpMenuItem(new MenuItem("Move Up"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new MoveUpAction(moveUpMenuItem.get(), this)));
            contextMenu->AddMenuItem(moveUpMenuItem.release());
        }
        if (CanMoveDown())
        {
            std::unique_ptr<MenuItem> moveDownMenuItem(new MenuItem("Move Down"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new MoveDownAction(moveDownMenuItem.get(), this)));
            contextMenu->AddMenuItem(moveDownMenuItem.release());
        }
    }
}

void Node::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
}

std::unique_ptr<Node> Node::RemoveChild(int index)
{
    return std::unique_ptr<Node>();
}

void Node::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    child.reset();
}

void Node::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    child.reset();
}

bool Node::CanMoveUp(const Node* child) const
{
    int index = IndexOf(child);
    return index > 0 && index < Count();
}

bool Node::CanMoveDown(const Node* child) const
{
    int index = IndexOf(child);
    return index >= 0 && index < Count() - 1;
}

} } // wingstall::package_editor
