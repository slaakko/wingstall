// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/component.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

Components::Components() : Node(NodeKind::components, "Components")
{
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
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Name", 200);
    for (const auto& component : components)
    {
        ListViewItem* item = listView->AddItem();
        component->SetData(item, imageList);
    }
    return listView.release();
}

Component::Component() : Node(NodeKind::component, std::string())
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
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->AddListViewEventHandlers(listView.get());
    }
    listView->SetDoubleBuffered();
    listView->SetImageList(imageList);
    listView->AddColumn("Name", 200);
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

} } // wingstall::package_editor
