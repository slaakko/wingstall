// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/component.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/action.hpp>
#include <package_editor/component_dialog.hpp>
#include <package_editor/add_directories_and_files_dialog.hpp>
#include <package_editor/configuration.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

struct DirectoryByName
{
    bool operator()(const std::unique_ptr<Directory>& left, const std::unique_ptr<Directory>& right) const
    {
        std::u32string l = ToUtf32(left->Name());
        std::u32string r = ToUtf32(right->Name());
        return l < r;
    }
};

struct FileByName
{
    bool operator()(const std::unique_ptr<File>& left, const std::unique_ptr<File>& right) const
    {
        std::u32string l = ToUtf32(left->Name());
        std::u32string r = ToUtf32(right->Name());
        return l < r;
    }
};

Components::Components() : Node(NodeKind::components, "Components")
{
}

void Components::AddElements(sngxml::dom::Element* packageElement)
{
    for (const auto& component : components)
    {
        packageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(component->ToXml()));
    }
}

void Components::AddComponent(Component* component)
{
    component->SetParent(this);
    components.push_back(std::unique_ptr<Component>(component));
}

TreeViewNode* Components::ToTreeViewNode(TreeView* view) 
{
    TreeViewNode* node = new TreeViewNode("Components");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("components.bitmap"));
    }
    for (const auto& component : components)
    {
        node->AddChild(component->ToTreeViewNode(view));
    }
    return node;
}

Control* Components::CreateView(ImageList* imageList)
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
    int nameColumnWidthValue = 200;
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
    for (const auto& component : components)
    {
        ListViewItem* item = listView->AddItem();
        component->SetData(item, imageList);
    }
    return listView.release();
}

int Components::Count() const
{
    return components.size();
}

int Components::IndexOf(const Node* child) const
{
    if (child->Kind() == NodeKind::component)
    {
        const Component* component = static_cast<const Component*>(child);
        int n = components.size();
        for (int i = 0; i < n; ++i)
        {
            if (components[i].get() == component)
            {
                return i;
            }
        }
    }
    return -1;
}

Node* Components::GetNode(int index) const
{
    if (index >= 0 && index < Count())
    {
        return components[index].get();
    }
    return nullptr;
}

bool Components::HasNode(const std::string& name) const
{
    for (const auto& component : components)
    {
        if (component->Name() == name) return true;
    }
    return false;
}

std::unique_ptr<Node> Components::RemoveChild(int index)
{
    if (index >= 0 && index < Count())
    {
        Component* component = components[index].release();
        components.erase(components.begin() + index);
        return std::unique_ptr<Node>(component);
    }
    return std::unique_ptr<Node>();
}

void Components::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::component && index >= 0 && index < Count())
    {
        Component* component = static_cast<Component*>(child.release());
        components.insert(components.begin() + index, std::unique_ptr<Component>(component));
    }
    else
    {
        child.reset();
    }
}

void Components::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    if (child->Kind() == NodeKind::component && index >= 0 && index < Count())
    {
        Component* component = static_cast<Component*>(child.release());
        components.insert(components.begin() + index + 1, std::unique_ptr<Component>(component));
    }
    else
    {
        child.reset();
    }
}

void Components::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addNewComponentMenuItem(new MenuItem("Add New Component"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewComponentMenuItem.get(), this, NodeKind::component)));
    contextMenu->AddMenuItem(addNewComponentMenuItem.release());
}

Component* Components::GetDirectoryComponent(const std::u32string& directoryName) const
{
    auto it = directoryNameComponentMap.find(directoryName);
    if (it != directoryNameComponentMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

Component* Components::GetFileComponent(const std::u32string& fileName) const
{
    auto it = fileNameComponentMap.find(fileName);
    if (it != fileNameComponentMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void Components::MapDirectoryComponent(const std::u32string& directoryName, Component* component)
{
    directoryNameComponentMap[directoryName] = component;
}

void Components::MapFileComponent(const std::u32string& fileName, Component* component)
{
    fileNameComponentMap[fileName] = component;
}

void Components::RemoveDirectoryName(const std::u32string& directoryName)
{
    directoryNameComponentMap.erase(directoryName);
}

void Components::RemoveFileName(const std::u32string& fileName)
{
    fileNameComponentMap.erase(fileName);
}

void Components::RemoveUnexistingDirectoriesAndFiles()
{
    for (const std::unique_ptr<Component>& component : components)
    {
        component->RemoveUnexistingDirectoriesAndFiles();
    }
}

void Components::MakeDisjoint()
{
    std::vector<std::u32string> directoryNames;
    std::vector<std::u32string> fileNames;
    for (const std::unique_ptr<Component>& component : components)
    {
        component->FilterDirectories(directoryNames);
        directoryNames = Merge(directoryNames, component->DirectoryNames());
        component->FilterFiles(fileNames);
        fileNames = Merge(fileNames, component->FileNames());
    }
}

void Components::MakeMaps()
{
    for (const std::unique_ptr<Component>& component : components)
    {
        std::vector<std::u32string> directoryNames = component->DirectoryNames();
        for (const std::u32string& directoryName : directoryNames)
        {
            directoryNameComponentMap[directoryName] = component.get();
        }
        std::vector<std::u32string> fileNames = component->FileNames();
        for (const std::u32string& fileName : fileNames)
        {
            fileNameComponentMap[fileName] = component.get();
        }
    }
}

void Components::AddNew(NodeKind kind)
{
    if (kind == NodeKind::component)
    {
        ComponentDialog dialog("Add New Component");
        MainWindow* mainWindow = GetMainWindow();
        if (mainWindow)
        {
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                if (HasNode(dialog.ComponentName()))
                {
                    throw std::runtime_error("name not unique");
                }
                Package* package = GetPackage();
                if (package)
                {
                    package->SetDirty();
                }
                Component* component = new Component(dialog.ComponentName());
                AddComponent(component);
                TreeViewNode* componentsTreeViewNode = GetTreeViewNode();
                if (componentsTreeViewNode)
                {
                    TreeView* treeView = componentsTreeViewNode->GetTreeView();
                    if (treeView)
                    {
                        TreeViewNode* componentTreeViewNode = component->ToTreeViewNode(treeView);
                        componentsTreeViewNode->AddChild(componentTreeViewNode);
                        treeView->SetSelectedNode(componentsTreeViewNode);
                    }
                }
                OpenAndExpand();
            }
        }
    }
}

Component::Component(const std::string& name_) : Node(NodeKind::component, name_)
{
}

Component::Component(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::component, std::string())
{
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw PackageXMLException("'component' element has no 'name' attribute", packageXMLFilePath, element);
    }
    std::unique_ptr<sngxml::xpath::XPathObject> directoryObject = sngxml::xpath::Evaluate(U"directory", element);
    if (directoryObject)
    {
        if (directoryObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(directoryObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Directory* directory = new Directory(packageXMLFilePath, element);
                    AddDirectory(directory);
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> fileObject = sngxml::xpath::Evaluate(U"file", element);
    if (fileObject)
    {
        if (fileObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(fileObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    File* file = new File(packageXMLFilePath, element);
                    AddFile(file);
                }
            }
        }
    }
    Sort();
}

sngxml::dom::Element* Component::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"component");
    element->SetAttribute(U"name", ToUtf32(Name()));
    for (const auto& directory : directories)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(directory->ToXml()));
    }
    for (const auto& file : files)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(file->ToXml()));
    }
    return element;
}

TreeViewNode* Component::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name());
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("component.bitmap"));
    }
    for (const auto& directory : directories)
    {
        node->AddChild(directory->ToTreeViewNode(view));
    }
    for (const auto& file : files)
    {
        node->AddChild(file->ToTreeViewNode(view));
    }
    return node;
}

Control* Component::CreateView(ImageList* imageList)
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
    for (const auto& directory : directories)
    {
        ListViewItem* item = listView->AddItem();
        directory->SetData(item, imageList);
    }
    for (const auto& file : files)
    {
        ListViewItem* item = listView->AddItem();
        file->SetData(item, imageList);
    }
    return listView.release();
}

void Component::AddDirectory(Directory* directory)
{
    directory->SetParent(this);
    directories.push_back(std::unique_ptr<Directory>(directory));
}

void Component::AddFile(File* file)
{
    file->SetParent(this);
    files.push_back(std::unique_ptr<File>(file));
}

int Component::Count() const
{
    return directories.size() + files.size();
}

int Component::IndexOf(const Node* child) const
{
    int nd = directories.size();
    if (child->Kind() == NodeKind::directory)
    {
        const Directory* directory = static_cast<const Directory*>(child);
        for (int i = 0; i < nd; ++i)
        {
            if (directories[i].get() == directory)
            {
                return i;
            }
        }
    }
    else if (child->Kind() == NodeKind::file)
    {
        const File* file = static_cast<const File*>(child);
        int n = files.size();
        for (int i = 0; i < n; ++i)
        {
            if (files[i].get() == file)
            {
                return nd + i;
            }
        }
    }
    return -1;
}

Node* Component::GetNode(int index) const
{
    int nd = directories.size();
    if (index >= 0 && index < nd)
    {
        return directories[index].get();
    }
    else if (index >= nd && index < Count())
    {
        return files[index - nd].get();
    }
    return nullptr;
}

std::unique_ptr<Node> Component::RemoveChild(int index)
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        int nd = directories.size();
        if (index >= 0 && index < nd)
        {
            Directory* directory = directories[index].release();
            components->RemoveDirectoryName(ToUtf32(directory->Name()));
            directories.erase(directories.begin() + index);
            return std::unique_ptr<Node>(directory);
        }
        else if (index >= nd && index < Count())
        {
            File* file = files[index - nd].release();
            components->RemoveDirectoryName(ToUtf32(file->Name()));
            files.erase(files.begin() + (index - nd));
            return std::unique_ptr<Node>(file);
        }
    }
    return std::unique_ptr<Node>();
}

void Component::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        int nd = directories.size();
        if (child->Kind() == NodeKind::directory && index >= 0 && index < nd)
        {
            Directory* directory = static_cast<Directory*>(child.release());
            components->MapDirectoryComponent(ToUtf32(directory->Name()), this);
            directories.insert(directories.begin() + index, std::unique_ptr<Directory>(directory));
        }
        else if (child->Kind() == NodeKind::file && index >= nd && index < Count())
        {
            File* file = static_cast<File*>(child.release());
            components->MapFileComponent(ToUtf32(file->Name()), this);
            files.insert(files.begin() + index - nd, std::unique_ptr<File>(file));
        }
        else
        {
            child.reset();
        }
    }
    else
    {
        child.reset();
    }
}

void Component::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        int nd = directories.size();
        if (child->Kind() == NodeKind::directory && index >= 0 && index < nd)
        {
            Directory* directory = static_cast<Directory*>(child.release());
            components->MapDirectoryComponent(ToUtf32(directory->Name()), this);
            directories.insert(directories.begin() + index + 1, std::unique_ptr<Directory>(directory));
        }
        else if (child->Kind() == NodeKind::file && index >= nd && index < Count())
        {
            File* file = static_cast<File*>(child.release());
            components->MapFileComponent(ToUtf32(file->Name()), this);
            files.insert(files.begin() + index - nd + 1, std::unique_ptr<File>(file));
        }
        else
        {
            child.reset();
        }
    }
    else
    {
        child.reset();
    }
}

bool Component::CanMoveUp(const Node* child) const
{
    return false;
}

bool Component::CanMoveDown(const Node* child) const
{
    return false;
}

void Component::Edit()
{
    ComponentDialog dialog("Edit Component");
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        std::string prevName = Name();
        dialog.SetComponentName(Name());
        if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
        {
            if (dialog.ComponentName() != prevName)
            {
                if (HasNode(dialog.ComponentName()))
                {
                    throw std::runtime_error("name not unique");
                }
                Package* package = GetPackage();
                if (package)
                {
                    package->SetDirty();
                }
                SetName(dialog.ComponentName());
                Open();
                TreeViewNode* componentTreeViewNode = GetTreeViewNode();
                if (componentTreeViewNode)
                {
                    componentTreeViewNode->SetText(Name());
                }
            }
        }
    }
}

void Component::AddNew(NodeKind kind)
{
    if (kind == NodeKind::content)
    {
        AddDirectoriesAndFilesDialog dialog(this);
        MainWindow* mainWindow = GetMainWindow();
        if (mainWindow)
        {
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                Package* package = GetPackage();
                if (package)
                {
                    package->SetDirty();
                }
                std::vector<std::u32string> selectedDirectories;
                std::vector<std::u32string> selectedFiles;
                dialog.GetSelectedDirectoriesAndFiles(selectedDirectories, selectedFiles);
                AddDirectoriesAndFiles(selectedDirectories, selectedFiles);
                Node* parent = Parent();
                if (parent && parent->Kind() == NodeKind::components)
                {
                    Components* components = static_cast<Components*>(parent);
                    TreeViewNode* componentsTreeViewNode = parent->GetTreeViewNode();
                    if (componentsTreeViewNode)
                    {
                        TreeView* view = componentsTreeViewNode->GetTreeView();
                        if (view)
                        {
                            componentsTreeViewNode->RemoveChildren();
                            for (const auto& component : components->GetComponents())
                            {
                                componentsTreeViewNode->AddChild(component->ToTreeViewNode(view));
                            }
                        }
                    }
                }
                OpenAndExpand();
            }
        }
    }
}

void Component::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addMenuItem(new MenuItem("Add Directories and Files"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addMenuItem.get(), this, NodeKind::content)));
    contextMenu->AddMenuItem(addMenuItem.release());
}

std::vector<std::u32string> Component::DirectoryNames() const
{
    std::vector<std::u32string> result;
    for (const std::unique_ptr<Directory>& directory : directories)
    {
        result.push_back(ToUtf32(directory->Name()));
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::u32string> Component::FileNames() const
{
    std::vector<std::u32string> result;
    for (const std::unique_ptr<File>& file : files)
    {
        result.push_back(ToUtf32(file->Name()));
    }
    std::sort(result.begin(), result.end());
    return result;
}

void Component::FilterDirectories(const std::vector<std::u32string>& exclude)
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        std::vector<std::unique_ptr<Directory>> result;
        for (std::unique_ptr<Directory>& directory : directories)
        {
            std::u32string directoryName = ToUtf32(directory->Name());
            if (!std::binary_search(exclude.begin(), exclude.end(), directoryName))
            {
                result.push_back(std::unique_ptr<Directory>(directory.release()));
            }
            else
            {
                components->RemoveDirectoryName(directoryName);
            }
        }
        std::swap(directories, result);
    }
}

void Component::FilterFiles(const std::vector<std::u32string>& exclude)
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        std::vector<std::unique_ptr<File>> result;
        for (std::unique_ptr<File>& file : files)
        {
            std::u32string fileName = ToUtf32(file->Name());
            if (!std::binary_search(exclude.begin(), exclude.end(), fileName))
            {
                result.push_back(std::unique_ptr<File>(file.release()));
            }
            else
            {
                components->RemoveFileName(fileName);
            }
        }
        std::swap(files, result);
    }
}

void Component::Map(const std::vector<std::u32string>& directoryNames, const std::vector<std::u32string>& fileNames)
{
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        for (const std::u32string& directoryName : directoryNames)
        {
            components->MapDirectoryComponent(directoryName, this);
        }
        for (const std::u32string& fileName : fileNames)
        {
            components->MapFileComponent(fileName, this);
        }
    }
}

void Component::Sort()
{
    std::sort(directories.begin(), directories.end(), DirectoryByName());
    std::sort(files.begin(), files.end(), FileByName());
}

void Component::RemoveUnexistingDirectoriesAndFiles()
{
    std::vector<std::u32string> directoriesToRemove;
    std::vector<std::u32string> filesToRemove;
    Package* package = GetPackage();
    if (package)
    {
        std::string sourceRootDir = package->GetProperties()->SourceRootDir();
        for (const auto& directory : directories)
        {
            boost::filesystem::path p(MakeNativeBoostPath(Path::Combine(sourceRootDir, directory->Name())));
            if (!boost::filesystem::exists(p))
            {
                directoriesToRemove.push_back(ToUtf32(directory->Name()));
            }
        }
        for (const auto& file : files)
        {
            boost::filesystem::path p(MakeNativeBoostPath(Path::Combine(sourceRootDir, file->Name())));
            if (!boost::filesystem::exists(p))
            {
                filesToRemove.push_back(ToUtf32(file->Name()));
            }
        }
        FilterDirectories(directoriesToRemove);
        FilterFiles(filesToRemove);
    }
}

void Component::AddDirectoriesAndFiles(const std::vector<std::u32string>& directoryNames, const std::vector<std::u32string>& fileNames)
{
    for (const std::u32string& directoryName : directoryNames)
    {
        AddDirectory(new Directory(ToUtf8(directoryName)));
    }
    for (const std::u32string& fileName : fileNames)
    {
        AddFile(new File(ToUtf8(fileName)));
    }
    Sort();
    Node* parent = Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        std::vector<std::u32string> allDirectoryNames = DirectoryNames();
        std::vector<std::u32string> allFileNames = FileNames();
        Components* components = static_cast<Components*>(parent);
        int n = components->GetComponents().size();
        for (int i = 0; i < n; ++i)
        {
            Component* component = components->GetComponents()[i].get();
            if (component != this)
            {
                component->FilterDirectories(allDirectoryNames);
                component->FilterFiles(allFileNames);
            }
        }
        Map(allDirectoryNames, allFileNames);
    }
}

} } // wingstall::package_editor
