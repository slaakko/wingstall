// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/package.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <wing/ImageList.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;
using namespace soulng::util;

Properties::Properties() : Node(NodeKind::properties, "Properties"), compression(wingstall::wingpackage::Compression::none), includeUninstaller(true), id(boost::uuids::nil_uuid())
{
}

void Properties::SetSourceRootDir(const std::string& sourceRootDir_)
{
    sourceRootDir = sourceRootDir_;
}

void Properties::SetTargetRootDir(const std::string& targetRootDir_)
{
    targetRootDir = targetRootDir_;
}

void Properties::SetAppName(const std::string& appName_)
{
    appName = appName_;
}

void Properties::SetPublisher(const std::string& publisher_)
{
    publisher = publisher_;
}

void Properties::SetVersion(const std::string& version_)
{
    version = version_;
}

void Properties::SetCompression(wingstall::wingpackage::Compression compression_)
{
    compression = compression_;
}

void Properties::SetIconFilePath(const std::string& iconFilePath_)
{
    iconFilePath = iconFilePath_;
}

void Properties::SetIncludeUninstaller(bool includeUninstaller_)
{
    includeUninstaller = includeUninstaller_;
}

void Properties::SetId(const boost::uuids::uuid& id_)
{
    id = id_;
}

TreeViewNode* Properties::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Properties");
    SetTreeViewNode(node);
    node->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        node->SetImageIndex(imageList->GetImageIndex("package.properties.bitmap"));
    }
    return node;
}

Package::Package() : Node(NodeKind::package, std::string()), filePath()
{
    properties.reset(new Properties());
    properties->SetParent(this);
    components.reset(new Components());
    components->SetParent(this);
    environment.reset(new Environment());
    environment->SetParent(this);
    links.reset(new Links());
    links->SetParent(this);
    engineVariables.reset(new EngineVariables());
    engineVariables->SetParent(this);
}

Package::Package(const std::string& packageXMLFilePath, sngxml::dom::Element* root) : 
    Node(NodeKind::package, std::string()), filePath(packageXMLFilePath)
{
    properties.reset(new Properties());
    properties->SetParent(this);
    components.reset(new Components());
    components->SetParent(this);
    environment.reset(new Environment());
    environment->SetParent(this);
    links.reset(new Links());
    links->SetParent(this);
    if (root->Name() != U"package")
    {
        throw PackageXMLException("not a Wingstall package XML file: the root element is not 'package' element", packageXMLFilePath, root);
    }
    std::u32string nameAttr = root->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw PackageXMLException("'package' element has no 'name' attribute", packageXMLFilePath, root);
    }
    std::u32string sourceRootDirAttr = root->GetAttribute(U"sourceRootDir");
    if (!sourceRootDirAttr.empty())
    {
        properties->SetSourceRootDir(ToUtf8(sourceRootDirAttr));
    }
    else
    {
        throw PackageXMLException("'package' element has no 'sourceRootDir' attribute", packageXMLFilePath, root);
    }
    std::u32string targetRootDirAttr = root->GetAttribute(U"targetRootDir");
    if (!targetRootDirAttr.empty())
    {
        properties->SetTargetRootDir(ToUtf8(targetRootDirAttr));
    }
    else
    {
        throw PackageXMLException("'package' element has no 'targetRootDir' attribute", packageXMLFilePath, root);
    }
    std::u32string appNameAttr = root->GetAttribute(U"appName");
    if (!appNameAttr.empty())
    {
        properties->SetAppName(ToUtf8(appNameAttr));
    }
    else
    {
        throw PackageXMLException("'package' element has no 'appName' attribute", packageXMLFilePath, root);
    }
    std::u32string publisherAttr = root->GetAttribute(U"publisher");
    if (!publisherAttr.empty())
    {
        properties->SetPublisher(ToUtf8(publisherAttr));
    }
    std::u32string versionAttr = root->GetAttribute(U"version");
    if (!versionAttr.empty())
    {
        properties->SetVersion(ToUtf8(versionAttr));
    }
    else
    {
        properties->SetVersion("1.0.0");
    }
    std::u32string compressionAttr = root->GetAttribute(U"compression");
    if (!compressionAttr.empty())
    {
        try
        {
            properties->SetCompression(wingstall::wingpackage::ParseCompressionStr(ToUtf8(compressionAttr)));
        }
        catch (const std::exception& ex)
        {
            throw PackageXMLException("'package' element has invalid 'compression' attribute value: " + std::string(ex.what()), packageXMLFilePath, root);
        }
    }
    std::u32string iconFilePathAttr = root->GetAttribute(U"iconFilePath");
    if (!iconFilePathAttr.empty())
    {
        properties->SetIconFilePath(ToUtf8(iconFilePathAttr));
    }
    std::u32string includeUninstallerAttr = root->GetAttribute(U"includeUninstaller");
    if (!includeUninstallerAttr.empty())
    {
        try
        {
            properties->SetIncludeUninstaller(ParseBool(ToUtf8(includeUninstallerAttr)));
        }
        catch (const std::exception& ex)
        {
            throw PackageXMLException("'package' element has invalid 'includeUninstaller' attribute value: " + std::string(ex.what()), packageXMLFilePath, root);
        }
    }
    std::u32string idAttr = root->GetAttribute(U"id");
    if (!idAttr.empty())
    {
        properties->SetId(boost::lexical_cast<boost::uuids::uuid>(ToUtf8(idAttr)));
    }
    else
    {
        properties->SetId(boost::uuids::random_generator()());
    }
    std::unique_ptr<sngxml::xpath::XPathObject> componentObject = sngxml::xpath::Evaluate(U"component", root);
    if (componentObject)
    {
        if (componentObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(componentObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Component* component = new Component(packageXMLFilePath, element);
                    components->AddComponent(component);
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> environmentObject = sngxml::xpath::Evaluate(U"environment", root);
    if (environmentObject)
    {
        if (environmentObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(environmentObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    environment.reset(new Environment(packageXMLFilePath, element));
                    environment->SetParent(this);
                }
            }
            else if (n > 1)
            {
                throw PackageXMLException("'package' element should contain at most one 'environment' element", packageXMLFilePath, root);
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> linksObject = sngxml::xpath::Evaluate(U"links", root);
    if (linksObject)
    {
        if (linksObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(linksObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    links.reset(new Links(packageXMLFilePath, element));
                    links->SetParent(this);
                }
            }
            else if (n > 1)
            {
                throw PackageXMLException("'package' element should contain at most one 'links' element", packageXMLFilePath, root);
            }
        }
    }
    engineVariables.reset(new EngineVariables());
    engineVariables->SetParent(this);
}

TreeViewNode* Package::ToTreeViewNode(TreeView* view) 
{
    TreeViewNode* root = new TreeViewNode("Package '" + Name() + "'");
    SetTreeViewNode(root);
    root->SetData(this);
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        root->SetImageIndex(imageList->GetImageIndex("package.bitmap"));
    }
    root->AddChild(properties->ToTreeViewNode(view));
    root->AddChild(components->ToTreeViewNode(view));
    root->AddChild(environment->ToTreeViewNode(view));
    root->AddChild(links->ToTreeViewNode(view));
    root->AddChild(engineVariables->ToTreeViewNode(view));
    return root;
}

Control* Package::CreateView(ImageList* imageList)
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
    ListViewItem* propertiesItem = listView->AddItem();
    properties->SetData(propertiesItem, imageList);
    ListViewItem* componentsItem = listView->AddItem();
    components->SetData(componentsItem, imageList);
    ListViewItem* environmentItem = listView->AddItem();
    environment->SetData(environmentItem, imageList);
    ListViewItem* linksItem = listView->AddItem();
    links->SetData(linksItem, imageList);
    ListViewItem* engineVariablesItem = listView->AddItem();
    engineVariables->SetData(engineVariablesItem, imageList);
    return listView.release();
}

} } // wingstall::package_editor
