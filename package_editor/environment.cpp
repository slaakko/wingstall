// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/environment.hpp>
#include <package_editor/error.hpp>
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
        value = ToUtf8(valueAttr);
    }
    else
    {
        throw PackageXMLException("'variable' element has no 'value' attribute", packageXMLFilePath, element);
    }
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

PathDirectory::PathDirectory() : Node(NodeKind::pathDirectory, std::string())
{
}

PathDirectory::PathDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::pathDirectory, "Path Directory")
{
    std::u32string valueAttr = element->GetAttribute(U"value");
    if (!valueAttr.empty())
    {
        value = ToUtf8(valueAttr);
    }
    else
    {
        throw PackageXMLException("'pathDirectory' element has no 'value' attribute", packageXMLFilePath, element);
    }
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

} } // wingstall::package_editor
