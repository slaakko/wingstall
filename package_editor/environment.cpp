// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/environment.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/action.hpp>
#include <package_editor/environment_variable_dialog.hpp>
#include <package_editor/path_directory_dialog.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

Environment::Environment() : Node(NodeKind::environment, "Environment")
{
}

Environment::Environment(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::environment, "Environment")
{
    std::unique_ptr<sngxml::xpath::XPathObject> variableObject = sngxml::xpath::Evaluate(U"variable", element);
    if (variableObject)
    {
        if (variableObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(variableObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    EnvironmentVariable* environmentVariable = new EnvironmentVariable(packageXMLFilePath, element);
                    AddEnvironentVariable(environmentVariable);
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> pathDirectoryObject = sngxml::xpath::Evaluate(U"pathDirectory", element);
    if (pathDirectoryObject)
    {
        if (pathDirectoryObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(pathDirectoryObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    PathDirectory* pathDirectory = new PathDirectory(packageXMLFilePath, element);
                    AddPathDirectory(pathDirectory);
                }
            }
        }
    }
}

TreeViewNode* Environment::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Environment");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("environment.bitmap"));
    }
    for (const auto& environmentVariable : environmentVariables)
    {
        node->AddChild(environmentVariable->ToTreeViewNode(view));
    }
    for (const auto& pathDirectory : pathDirectories)
    {
        node->AddChild(pathDirectory->ToTreeViewNode(view));
    }
    return node;
}

Control* Environment::CreateView(ImageList* imageList)
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
    listView->AddColumn("Value", 200);
    for (const auto& environmentVariable : environmentVariables)
    {
        ListViewItem* item = listView->AddItem();
        environmentVariable->SetData(item, imageList);
    }
    for (const auto& pathDirectory : pathDirectories)
    {
        ListViewItem* item = listView->AddItem();
        pathDirectory->SetData(item, imageList);
    }
    return listView.release();
}

void Environment::AddEnvironentVariable(EnvironmentVariable* environmentVariable)
{
    environmentVariable->SetParent(this);
    environmentVariables.push_back(std::unique_ptr<EnvironmentVariable>(environmentVariable));
}

void Environment::AddPathDirectory(PathDirectory* pathDirectory)
{
    pathDirectory->SetParent(this);
    pathDirectories.push_back(std::unique_ptr<PathDirectory>(pathDirectory));
}

int Environment::Count() const
{
    return environmentVariables.size() + pathDirectories.size();
}

int Environment::IndexOf(const Node* child) const
{
    int ne = environmentVariables.size();
    if (child->Kind() == NodeKind::environmentVariable)
    {
        const EnvironmentVariable* environmentVariable = static_cast<const EnvironmentVariable*>(child);
        for (int i = 0; i < ne; ++i)
        {
            if (environmentVariables[i].get() == environmentVariable)
            {
                return i;
            }
        }
    }
    else if (child->Kind() == NodeKind::pathDirectory)
    {
        const PathDirectory* pathDirectory = static_cast<const PathDirectory*>(child);
        int n = pathDirectories.size();
        for (int i = 0; i < n; ++i)
        {
            if (pathDirectories[i].get() == pathDirectory)
            {
                return ne + i;
            }
        }
    }
    return -1;
}

Node* Environment::GetNode(int index) const
{
    int ne = environmentVariables.size();
    if (index >= 0 && index < ne)
    {
        return environmentVariables[index].get();
    }
    else if (index >= ne && index < Count())
    {
        return pathDirectories[index - ne].get();
    }
    return nullptr;
}

std::unique_ptr<Node> Environment::RemoveChild(int index)
{
    int ne = environmentVariables.size();
    if (index >= 0 && index < ne)
    {
        EnvironmentVariable* environmentVariable = environmentVariables[index].release();
        environmentVariables.erase(environmentVariables.begin() + index);
        return std::unique_ptr<Node>(environmentVariable);
    }
    else if (index >= ne && index < Count())
    {
        PathDirectory* pathDirectory = pathDirectories[index - ne].release();
        pathDirectories.erase(pathDirectories.begin() + (index - ne));
        return std::unique_ptr<Node>(pathDirectory);
    }
    return std::unique_ptr<Node>();
}

void Environment::InsertBefore(int index, std::unique_ptr<Node>&& child)
{
    int ne = environmentVariables.size();
    if (child->Kind() == NodeKind::environmentVariable && index >= 0 && index < ne)
    {
        EnvironmentVariable* environmentVariable = static_cast<EnvironmentVariable*>(child.release());
        environmentVariables.insert(environmentVariables.begin() + index, std::unique_ptr<EnvironmentVariable>(environmentVariable));
    }
    else if (child->Kind() == NodeKind::pathDirectory && index >= ne && index < Count())
    {
        PathDirectory* pathDirectory = static_cast<PathDirectory*>(child.release());
        pathDirectories.insert(pathDirectories.begin() + index - ne, std::unique_ptr<PathDirectory>(pathDirectory));
    }
    else
    {
        child.reset();
    }
}

void Environment::InsertAfter(int index, std::unique_ptr<Node>&& child)
{
    int ne = environmentVariables.size();
    if (child->Kind() == NodeKind::environmentVariable && index >= 0 && index < ne)
    {
        EnvironmentVariable* environmentVariable = static_cast<EnvironmentVariable*>(child.release());
        environmentVariables.insert(environmentVariables.begin() + index + 1, std::unique_ptr<EnvironmentVariable>(environmentVariable));
    }
    else if (child->Kind() == NodeKind::pathDirectory && index >= ne && index < Count())
    {
        PathDirectory* pathDirectory = static_cast<PathDirectory*>(child.release());
        pathDirectories.insert(pathDirectories.begin() + index - ne + 1, std::unique_ptr<PathDirectory>(pathDirectory));
    }
    else
    {
        child.reset();
    }
}

bool Environment::CanMoveUp(const Node* child) const
{
    int ne = environmentVariables.size();
    if (child->Kind() == NodeKind::environmentVariable)
    {
        int index = IndexOf(child);
        return index > 0 && index < ne;
    }
    else if (child->Kind() == NodeKind::pathDirectory)
    {
        int index = IndexOf(child);
        return index - ne > 0 && index - ne < pathDirectories.size();
    }
    return false;
}

bool Environment::CanMoveDown(const Node* child) const
{
    int ne = environmentVariables.size();
    if (child->Kind() == NodeKind::environmentVariable)
    {
        int index = IndexOf(child);
        return index >= 0 && index < ne - 1;
    }
    else if (child->Kind() == NodeKind::pathDirectory)
    {
        int index = IndexOf(child);
        return index - ne >= 0 && index - ne < pathDirectories.size() - 1;
    }
    return false;
}

bool Environment::HasEnvironmentVariable(const std::string& name) const
{
    for (const auto& environmentVariable : environmentVariables)
    {
        if (environmentVariable->Name() == name) return true;
    }
    return false;
}

bool Environment::HasPathDirectory(const std::string& value) const
{
    for (const auto& pathDirectory : pathDirectories)
    {
        if (pathDirectory->Value() == value) return true;
    }
    return false;
}

void Environment::AddNew(NodeKind kind)
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        if (kind == NodeKind::environmentVariable)
        {
            EnvironmentVariableDialog dialog("Add New Environment Variable");
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                std::unique_ptr<EnvironmentVariable> environmentVariablePtr(new EnvironmentVariable());
                EnvironmentVariable* environemntVariable = environmentVariablePtr.get();
                dialog.GetData(environemntVariable);
                if (HasEnvironmentVariable(environemntVariable->Name()))
                {
                    throw std::runtime_error("name not unique");
                }
                AddEnvironentVariable(environmentVariablePtr.release());
                Open();
                TreeViewNode* environmentTreeViewNode = GetTreeViewNode();
                if (environmentTreeViewNode)
                {
                    TreeView* treeView = environmentTreeViewNode->GetTreeView();
                    if (treeView)
                    {
                        TreeViewNode* environmentVariableTreeViewNode = environemntVariable->ToTreeViewNode(treeView);
                        environmentTreeViewNode->AddChild(environmentVariableTreeViewNode);
                        treeView->SetSelectedNode(environmentTreeViewNode);
                    }
                }
            }
        }
        else if (kind == NodeKind::pathDirectory)
        {
            PathDirectoryDialog dialog("Add New PATH directory");
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                std::unique_ptr<PathDirectory> pathDirectoryPtr(new PathDirectory());
                PathDirectory* pathDirectory = pathDirectoryPtr.get();
                dialog.GetData(pathDirectory);
                if (HasPathDirectory(pathDirectory->Value()))
                {
                    throw std::runtime_error("path not unique");
                }
                AddPathDirectory(pathDirectoryPtr.release());
                Open();
                TreeViewNode* environmentTreeViewNode = GetTreeViewNode();
                if (environmentTreeViewNode)
                {
                    TreeView* treeView = environmentTreeViewNode->GetTreeView();
                    if (treeView)
                    {
                        TreeViewNode* pathDirectoryTreeViewNode = pathDirectory->ToTreeViewNode(treeView);
                        environmentTreeViewNode->AddChild(pathDirectoryTreeViewNode);
                        treeView->SetSelectedNode(environmentTreeViewNode);
                    }
                }
            }
        }
    }
}

void Environment::AddAddNewMenuItems(ContextMenu* contextMenu, std::vector<std::unique_ptr<ClickAction>>& clickActions)
{
    std::unique_ptr<MenuItem> addNewEnvironmentVariableMenuItem(new MenuItem("Add New Environment Variable"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewEnvironmentVariableMenuItem.get(), this, NodeKind::environmentVariable)));
    contextMenu->AddMenuItem(addNewEnvironmentVariableMenuItem.release());

    std::unique_ptr<MenuItem> addNewPathDirectoryMenuItem(new MenuItem("Add New PATH directory"));
    clickActions.push_back(std::unique_ptr<ClickAction>(new AddAction(addNewPathDirectoryMenuItem.get(), this, NodeKind::pathDirectory)));
    contextMenu->AddMenuItem(addNewPathDirectoryMenuItem.release());
}

EnvironmentVariable::EnvironmentVariable() : Node(NodeKind::environmentVariable, std::string())
{
}

EnvironmentVariable::EnvironmentVariable(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::environmentVariable, std::string())
{
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw PackageXMLException("'variable' element has no 'name' attribute", packageXMLFilePath, element);
    }
    std::u32string valueAttr = element->GetAttribute(U"value");
    if (!valueAttr.empty())
    {
        SetValue(ToUtf8(valueAttr));
    }
    else
    {
        throw PackageXMLException("'variable' element has no 'value' attribute", packageXMLFilePath, element);
    }
}

void EnvironmentVariable::SetValue(const std::string& value_)
{
    value = value_;
}

void EnvironmentVariable::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(1, value);
}

TreeViewNode* EnvironmentVariable::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name() + "=" + value);
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("environment.var.bitmap"));
    }
    return node;
}

void EnvironmentVariable::Edit()
{
    EnvironmentVariableDialog dialog("Edit Environment Variable");
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        Node* parent = Parent();
        if (parent && parent->Kind() == NodeKind::environment)
        {
            Environment* environment = static_cast<Environment*>(parent);
            std::string prevName = Name();
            dialog.SetData(this);
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                if (dialog.EnvironmentVariableName() != prevName)
                {
                    if (environment->HasEnvironmentVariable(dialog.EnvironmentVariableName()))
                    {
                        throw std::runtime_error("name not unique");
                    }
                }
                dialog.GetData(this);
                environment->Open();
                TreeViewNode* environmentVariableTreeViewNode = GetTreeViewNode();
                if (environmentVariableTreeViewNode)
                {
                    environmentVariableTreeViewNode->SetText(Name() + "=" + Value());
                }
            }
        }
    }
}

PathDirectory::PathDirectory() : Node(NodeKind::pathDirectory, std::string())
{
}

PathDirectory::PathDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::pathDirectory, "PATH Directory")
{
    std::u32string valueAttr = element->GetAttribute(U"value");
    if (!valueAttr.empty())
    {
        SetValue(ToUtf8(valueAttr));
    }
    else
    {
        throw PackageXMLException("'pathDirectory' element has no 'value' attribute", packageXMLFilePath, element);
    }
}

void PathDirectory::SetValue(const std::string& value_)
{
    value = value_;
}

TreeViewNode* PathDirectory::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(value);
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("path.directory.bitmap"));
    }
    return node;
}

void PathDirectory::SetData(ListViewItem* item, ImageList* imageList)
{
    Node::SetData(item, imageList);
    item->SetColumnValue(1, value);
}

void PathDirectory::Edit()
{
    PathDirectoryDialog dialog("Edit PATH Directory");
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        Node* parent = Parent();
        if (parent && parent->Kind() == NodeKind::environment)
        {
            Environment* environment = static_cast<Environment*>(parent);
            std::string prevValue = Value();
            dialog.SetData(this);
            if (dialog.ShowDialog(*mainWindow) == DialogResult::ok)
            {
                if (dialog.PathDirectoryValue() != prevValue)
                {
                    if (environment->HasPathDirectory(dialog.PathDirectoryValue()))
                    {
                        throw std::runtime_error("path not unique");
                    }
                }
                dialog.GetData(this);
                environment->Open();
                TreeViewNode* pathDirectoryTreeViewNode = GetTreeViewNode();
                if (pathDirectoryTreeViewNode)
                {
                    pathDirectoryTreeViewNode->SetText(Value());
                }
            }
        }
    }
}

} } // wingstall::package_editor
