// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/package.hpp>
#include <package_editor/error.hpp>
#include <package_editor/main_window.hpp>
#include <package_editor/properties_view.hpp>
#include <package_editor/configuration.hpp>
#include <wing/ImageList.hpp>
#include <wing/ScrollableControl.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;
using namespace soulng::util;

Properties::Properties() : Node(NodeKind::properties, "Properties"), compression(wingstall::wingpackage::Compression::none), includeUninstaller(true), id(boost::uuids::nil_uuid())
{
}

void Properties::SetAttributes(sngxml::dom::Element* packageElement)
{
    packageElement->SetAttribute(U"sourceRootDir", ToUtf32(sourceRootDir));
    packageElement->SetAttribute(U"targetRootDir", ToUtf32(targetRootDir));
    packageElement->SetAttribute(U"appName", ToUtf32(appName));
    if (!publisher.empty())
    {
        packageElement->SetAttribute(U"publisher", ToUtf32(publisher));
    }
    if (!version.empty())
    {
        packageElement->SetAttribute(U"version", ToUtf32(version));
    }
    packageElement->SetAttribute(U"compression", ToUtf32(wingpackage::CompressionStr(compression)));
    if (!iconFilePath.empty())
    {
        packageElement->SetAttribute(U"iconFilePath", ToUtf32(iconFilePath));
    }
    packageElement->SetAttribute(U"includeUninstaller", includeUninstaller ? U"true" : U"false");
    if (!id.is_nil())
    {
        packageElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(id)));
    }
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

Control* Properties::CreateView(ImageList* imageList)
{
    std::unique_ptr<PropertiesView> viewPtr(new PropertiesView(GetPackage()));
    return viewPtr.release();
}

Package::Package(const std::string& packageXMLFilePath) : Node(NodeKind::package, std::string()), filePath(packageXMLFilePath), view(nullptr), explorer(nullptr), dirty(false), building(false)
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
    Node(NodeKind::package, std::string()), filePath(packageXMLFilePath), view(nullptr), explorer(nullptr), dirty(false), building(false)
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
    std::unique_ptr<sngxml::xpath::XPathObject> preinstallObject = sngxml::xpath::Evaluate(U"preinstall", root);
    if (preinstallObject)
    {
        if (preinstallObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(preinstallObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    preinstallComponent.reset(new PreinstallComponent(packageXMLFilePath, element));
                }
            }
            else if (n > 1)
            {
                throw PackageXMLException("'package' element should contain at most one 'preinstall' element", packageXMLFilePath, root);
            }
        }
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
    components->MakeDisjoint();
    components->MakeMaps();
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
    std::unique_ptr<sngxml::xpath::XPathObject> uninstallObject = sngxml::xpath::Evaluate(U"uninstall", root);
    if (uninstallObject)
    {
        if (uninstallObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(uninstallObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    uninstallComponent.reset(new UninstallComponent(packageXMLFilePath, element));
                }
            }
            else if (n > 1)
            {
                throw PackageXMLException("'package' element should contain at most one 'uninstall' element", packageXMLFilePath, root);
            }
        }

    }
    engineVariables.reset(new EngineVariables());
    engineVariables->SetParent(this);
}

bool Package::CanSave() const
{
    return IsDirty();
}

void Package::Save()
{
    MainWindow* mainWindow = GetMainWindow();
    sngxml::dom::Document packageDoc;
    packageDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(ToXml())); 
    std::string directory = Path::GetDirectoryName(filePath);
    boost::filesystem::create_directories(directory);
    std::ofstream file(filePath);
    CodeFormatter formatter(file); 
    formatter.SetIndentSize(1);
    packageDoc.Write(formatter);
    dirty = false;
    if (mainWindow)
    {
        mainWindow->DisableSave();
    }
}

bool Package::CanBuild() const
{
    return true;
}

void Package::Build()
{
    building = true;
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->StartBuild();
    }
}

void Package::Close()
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->ClosePackageClick();
    }
}

void Package::SetDirty()
{
    MainWindow* mainWindow = GetMainWindow();
    if (mainWindow)
    {
        mainWindow->EnableSave();
    }
    dirty = true;
}

void Package::ResetBuilding()
{
    building = false;
}

sngxml::dom::Element* Package::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"package");
    element->SetAttribute(U"name", ToUtf32(Name()));
    properties->SetAttributes(element);
    if (preinstallComponent)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(preinstallComponent->ToXml()));
    }
    components->AddElements(element);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(environment->ToXml()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(links->ToXml()));
    if (uninstallComponent)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(uninstallComponent->ToXml()));
    }
    return element;
}

std::string Package::BinDirectoryPath() const
{
    std::string packageDirectoryPath = Path::GetDirectoryName(filePath);
    return Path::Combine(packageDirectoryPath, "bin");
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
