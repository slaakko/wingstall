// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/node.hpp>
#include <package_editor/package_content_view.hpp>
#include <package_editor/package_explorer.hpp>
#include <package_editor/action.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/path_bar.hpp>
#include <package_editor/configuration.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem/directory.hpp>
#include <algorithm>

namespace wingstall { namespace package_editor {

using namespace soulng::util;
using namespace soulng::unicode;

void GetDirectoriesAndFiles(const std::string& dirPath, std::vector<std::u32string>& directories, std::vector<std::u32string>& files)
{
    boost::filesystem::path nativeDirPath = MakeNativeBoostPath(dirPath);
    boost::system::error_code ec;
    boost::filesystem::directory_iterator it(nativeDirPath, ec);
    if (ec)
    {
        throw std::runtime_error("cannot iterate directory '" + dirPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    while (it != boost::filesystem::directory_iterator())
    {
        if (it->status().type() == boost::filesystem::file_type::regular_file)
        {
            std::wstring str = it->path().native();
            std::u16string s((const char16_t*)str.c_str());
            std::u32string fileName = ToUtf32(Path::GetFileName(GetFullPath(ToUtf8(s))));
            files.push_back(fileName);
        }
        else if (it->status().type() == boost::filesystem::file_type::directory_file)
        {
            if (!it->path().filename_is_dot() && !it->path().filename_is_dot_dot())
            {
                std::wstring str = it->path().native();
                std::u16string s((const char16_t*)str.c_str());
                std::u32string dirName = ToUtf32(Path::GetFileName(GetFullPath(ToUtf8(s))));
                directories.push_back(dirName);
            }
        }
        ++it;
    }
    std::sort(files.begin(), files.end());
    std::sort(directories.begin(), directories.end());
}

std::vector<std::u32string> Filter(const std::vector<std::u32string>& names, const std::vector<std::u32string>& exclude)
{
    std::vector<std::u32string> result;
    for (const std::u32string& name : names)
    {
        if (!std::binary_search(exclude.cbegin(), exclude.cend(), name))
        {
            result.push_back(name);
        }
    }
    return result;
}

std::vector<std::u32string> Merge(const std::vector<std::u32string>& left, const std::vector<std::u32string>& right)
{
    std::vector<std::u32string> result;
    std::merge(left.begin(), left.end(), right.begin(), right.end(), std::back_inserter(result));
    return result;
}

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
        PackageContentView* view = package->ContentView();
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

void Node::ResetDirectoryPath()
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        PathBar* pathBar = mainWindow->GetPathBar();
        if (pathBar)
        {
            pathBar->SetDirectoryPath(std::string());
        }
    }
}

void Node::ViewContent()
{
    Package* package = GetPackage();
    if (package)
    {
        PackageContentView* view = package->ContentView();
        if (view)
        {
            view->ViewContent(this);
        }
    }
}

void Node::SetCurrentPathNode()
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        PathBar* pathBar = mainWindow->GetPathBar();
        if (pathBar)
        {
            pathBar->SetCurrentNode(this);
        }
    }
}

void Node::Open()
{
    Explore();
    ViewContent();
    SetCurrentPathNode();
}

void Node::OpenAndExpand()
{
    Open();
    TreeViewNode* treeViewNode = GetTreeViewNode();
    if (treeViewNode && !treeViewNode->Children().IsEmpty())
    {
        treeViewNode->Expand();
    }
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

void Node::ExecuteDefaultAction()
{
    if (CanOpen())
    {
        Open();
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

void Node::AddNode(Node* node)
{
}

void Node::AddNew(NodeKind kind)
{
}

void Node::AddRule(Rule* rule)
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
    Package* package = GetPackage();
    if (package->Building())
    {
        return;
    }
    if ((menuItemsKind & MenuItemsKind::newMenuItems) != MenuItemsKind::none)
    {
        if (CanAdd())
        {
            AddAddNewMenuItems(contextMenu, clickActions);
        }
    }
    else if ((menuItemsKind & MenuItemsKind::allMenuItems) != MenuItemsKind::none)
    {
        if (CanOpen())
        {
            std::unique_ptr<MenuItem> openMenuItem(new MenuItem("Open"));
            if ((menuItemsKind & MenuItemsKind::treeView) != MenuItemsKind::none)
            {
                clickActions.push_back(std::unique_ptr<ClickAction>(new OpenAndExpandAction(openMenuItem.get(), this)));
            }
            else
            {
                clickActions.push_back(std::unique_ptr<ClickAction>(new OpenAction(openMenuItem.get(), this)));
            }
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
        if (CanExclude())
        {
            std::unique_ptr<MenuItem> excludeMenuItem(new MenuItem("Exclude"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new ExcludeAction(excludeMenuItem.get(), this)));
            contextMenu->AddMenuItem(excludeMenuItem.release());
        }
        if (CanInclude())
        {
            std::unique_ptr<MenuItem> includeMenuItem(new MenuItem("Include"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new IncludeAction(includeMenuItem.get(), this)));
            contextMenu->AddMenuItem(includeMenuItem.release());
        }
        if (CanSave())
        {
            std::unique_ptr<MenuItem> saveMenuItem(new MenuItem("Save"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new SaveAction(saveMenuItem.get(), this)));
            contextMenu->AddMenuItem(saveMenuItem.release());
        }
        if (CanBuild())
        {
            std::unique_ptr<MenuItem> buildMenuItem(new MenuItem("Build"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new BuildAction(buildMenuItem.get(), this)));
            contextMenu->AddMenuItem(buildMenuItem.release());
        }
        if (CanClose())
        {
            std::unique_ptr<MenuItem> closeMenuItem(new MenuItem("Close"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new CloseAction(closeMenuItem.get(), this)));
            contextMenu->AddMenuItem(closeMenuItem.release());
        }
        if (CanClear())
        {
            std::unique_ptr<MenuItem> clearMenuItem(new MenuItem("Clear"));
            clickActions.push_back(std::unique_ptr<ClickAction>(new ClearAction(clearMenuItem.get(), this)));
            contextMenu->AddMenuItem(clearMenuItem.release());
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

void Node::Save()
{
}

void Node::Build()
{
}

void Node::Close()
{
}

void Node::Clear()
{
}

std::string Node::DirectoryPath() const
{
    return parent->DirectoryPath();
}

bool Node::IncludeDirectory(const std::u32string& directoryName) const
{
    bool include = true;
    bool matched = false;
    int n = RuleCount();
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = GetRule(i);
        if (rule->GetPathKind() == PathKind::dir)
        {
            if ((rule->GetRuleKind() == RuleKind::include) && rule->Matches(directoryName))
            {
                include = true;
                matched = true;
            }
            else if ((rule->GetRuleKind() == RuleKind::exclude) && rule->Matches(directoryName))
            {
                include = false;
                matched = true;
            }
        }
    }
    Node* p = parent;
    while (p && !matched)
    {
        int n = p->RuleCount();
        for (int i = 0; i < n; ++i)
        {
            Rule* rule = p->GetRule(i);
            if (rule->GetPathKind() == PathKind::dir)
            {
                if (rule->Cascade())
                {
                    if ((rule->GetRuleKind() == RuleKind::include) && rule->Matches(directoryName))
                    {
                        include = true;
                        matched = true;
                    }
                    else if ((rule->GetRuleKind() == RuleKind::exclude) && rule->Matches(directoryName))
                    {
                        include = false;
                        matched = true;
                    }
                }
            }
        }
        p = p->Parent();
    }
    return include;
}

bool Node::IncludeFile(const std::u32string& fileName) const
{
    bool include = true;
    bool matched = false;
    int n = RuleCount();
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = GetRule(i);
        if (rule->GetPathKind() == PathKind::file)
        {
            if (rule->GetRuleKind() == RuleKind::include && rule->Matches(fileName))
            {
                include = true;
                matched = true;
            }
            else if (rule->GetRuleKind() == RuleKind::exclude && rule->Matches(fileName))
            {
                include = false;
                matched = true;
            }
        }
    }
    Node* p = parent;
    while (p && !matched)
    {
        int n = p->RuleCount();
        for (int i = 0; i < n; ++i)
        {
            Rule* rule = p->GetRule(i);
            if (rule->GetPathKind() == PathKind::file)
            {
                if (rule->Cascade())
                {
                    if (rule->GetRuleKind() == RuleKind::include && rule->Matches(fileName))
                    {
                        include = true;
                        matched = true;
                    }
                    else if (rule->GetRuleKind() == RuleKind::exclude && rule->Matches(fileName))
                    {
                        include = false;
                        matched = true;
                    }
                }
            }
        }
        p = p->Parent();
    }
    return include;
}

Control* Node::CreateContentView(ImageList* imageList)
{
    std::string directoryPath = DirectoryPath();
    std::unique_ptr<ListView> listViewPtr(new ListView(ListViewCreateParams().Defaults().SetDock(Dock::fill)));
    ListView* listView = listViewPtr.get();
    listView->SetData(this);
    listView->SetFlag(ControlFlags::scrollSubject);
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        listView->ColumnWidthChanged().AddHandler(mainWindow, &MainWindow::ListViewColumnWidthChanged);
    }
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
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView);
        PathBar* pathBar = mainWindow->GetPathBar();
        pathBar->SetDirectoryPath(directoryPath);
    }
    TreeViewNode* treeViewNode = GetTreeViewNode();
    if (treeViewNode)
    {
        treeViewNode->RemoveChildren();
    }
    std::vector<std::u32string> directories;
    std::vector<std::u32string> files;
    GetDirectoriesAndFiles(directoryPath, directories, files);
    for (const std::u32string& directory : directories)
    {
        std::unique_ptr<ContentDirectory> contentDirectory(new ContentDirectory(ToUtf8(directory)));
        ListViewItem* item = listView->AddItem();
        if (!IncludeDirectory(directory))
        {
            item->SetFlag(ListViewItemFlags::disabled);
            contentDirectory->Disable();
        }
        contentDirectory->SetData(item, imageList);
        if (treeViewNode && !contentDirectory->IsDisabled())
        {
            TreeViewNode* childNode = new TreeViewNode(contentDirectory->Name());
            childNode->SetImageIndex(imageList->GetImageIndex("folder.closed.bitmap"));
            childNode->SetExpandedImageIndex(imageList->GetImageIndex("folder.opened.bitmap"));
            contentDirectory->SetTreeViewNode(childNode);
            childNode->SetData(contentDirectory.get());
            treeViewNode->AddChild(childNode);
            SetTreeViewNode(treeViewNode);
            treeViewNode->SetData(this);
        }
        AddNode(contentDirectory.release());
    }
    for (const std::u32string& file : files)
    {
        std::unique_ptr<ContentFile> contentFile(new ContentFile(ToUtf8(file)));
        ListViewItem* item = listView->AddItem();
        if (!IncludeFile(file))
        {
            item->SetFlag(ListViewItemFlags::disabled);
            contentFile->Disable();
        }
        contentFile->SetData(item, imageList);
        AddNode(contentFile.release());
    }
    return listViewPtr.release();
}

} } // wingstall::package_editor
