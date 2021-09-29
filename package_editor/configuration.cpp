// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/configuration.hpp>
#include <wingstall_config/config.hpp>
#include <wing/Graphics.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>

namespace wingstall { namespace package_editor {

using namespace soulng::util;
using namespace soulng::unicode;

ColumnWidth::ColumnWidth() : view(), name(), defined(false), value(0) 
{
}

ColumnWidth::ColumnWidth(View* view_, const std::string& name_) : view(view_), name(name_), defined(false), value(0)
{
}

void ColumnWidth::FromXml(sngxml::dom::Element* element)
{
    std::u32string valueAttr = element->GetAttribute(U"value");
    if (!valueAttr.empty())
    {
        value = boost::lexical_cast<int>(ToUtf8(valueAttr));
    }
    std::u32string definedAttr = element->GetAttribute(U"defined");
    if (!definedAttr.empty())
    {
        if (definedAttr == U"true")
        {
            defined = true;
        }
        else if (definedAttr == U"false")
        {
            defined = false;
        }
    }
}

sngxml::dom::Element* ColumnWidth::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"column");
    element->SetAttribute(U"name", ToUtf32(name)); 
    element->SetAttribute(U"value", ToUtf32(std::to_string(value)));
    element->SetAttribute(U"defined", defined ? U"true" : U"false");
    return element;
}

void ColumnWidth::Set(int value_)
{
    if (value != value_)
    {
        value = value_;
        defined = true;
        view->SetChanged();
    }
}

View::View() : viewSettings(), viewName()
{
}

View::View(ViewSettings* viewSettings_, const std::string& viewName_) : viewSettings(viewSettings_), viewName(viewName_)
{
}

void View::FromXml(sngxml::dom::Element* element)
{
    std::unique_ptr<sngxml::xpath::XPathObject> columnWidthObject = sngxml::xpath::Evaluate(U"column", element);
    if (columnWidthObject)
    {
        if (columnWidthObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(columnWidthObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string nameAttr = element->GetAttribute(U"name");
                    if (!nameAttr.empty())
                    {
                        ColumnWidth& columnWidth = GetColumnWidth(ToUtf8(nameAttr));
                        columnWidth.FromXml(element);
                    }
                }
            }
        }
    }
}

sngxml::dom::Element* View::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"view");
    element->SetAttribute(U"name", ToUtf32(viewName));
    for (const auto& p : columnWidthMap)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(p.second.ToXml()));
    }
    return element;
}

void View::SetChanged()
{
    viewSettings->SetChanged();
}

ColumnWidth& View::GetColumnWidth(const std::string& columnName)
{
    auto it = columnWidthMap.find(columnName);
    if (it == columnWidthMap.cend())
    {
        columnWidthMap.insert(std::make_pair(columnName, ColumnWidth(this, columnName)));
    }
    return columnWidthMap[columnName];
}

ViewSettings::ViewSettings() : changed(false)
{
}

ViewSettings::ViewSettings(sngxml::dom::Element* element) : changed(false)
{
    std::unique_ptr<sngxml::xpath::XPathObject> viewObject = sngxml::xpath::Evaluate(U"view", element);
    if (viewObject)
    {
        if (viewObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(viewObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string nameAttr = element->GetAttribute(U"name");
                    if (!nameAttr.empty())
                    {
                        View& view = GetView(ToUtf8(nameAttr));
                        view.FromXml(element);
                    }
                }
            }
        }
    }
}

sngxml::dom::Element* ViewSettings::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"viewSettings");
    for (const auto& p : viewMap)
    {
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(p.second.ToXml()));
    }
    return element;
}

void ViewSettings::SetChanged()
{
    changed = true;
}

void ViewSettings::ResetChanged()
{
    changed = false;
}

View& ViewSettings::GetView(const std::string& name)
{
    auto it = viewMap.find(name);
    if (it == viewMap.cend())
    {
        viewMap.insert(std::make_pair(name, View(this, name)));
    }
    return viewMap[name];
}

class WindowLocation
{
public:
    WindowLocation();
    WindowLocation(sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    const Point& Loc() const { return loc; }
    void SetLoc(const Point& loc_, bool& changed);
private:
    Point loc;
};

sngxml::dom::Element* WindowLocation::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"location");
    element->SetAttribute(U"x", ToUtf32(std::to_string(loc.X)));
    element->SetAttribute(U"y", ToUtf32(std::to_string(loc.Y)));
    return element;
}

void WindowLocation::SetLoc(const Point& loc_, bool& changed)
{
    if (loc != loc_)
    {
        loc = loc_;
        changed = true;
    }
}

WindowLocation::WindowLocation() : loc()
{
}

WindowLocation::WindowLocation(sngxml::dom::Element* element)
{
    std::u32string xAttr = element->GetAttribute(U"x");
    if (!xAttr.empty())
    {
        loc.X = boost::lexical_cast<int>(ToUtf8(xAttr));
    }
    std::u32string yAttr = element->GetAttribute(U"y");
    if (!yAttr.empty())
    {
        loc.Y = boost::lexical_cast<int>(ToUtf8(yAttr));
    }
}

class WindowSize
{
public:
    WindowSize();
    WindowSize(sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    const Size& Sz() const { return sz; }
    void SetSz(const Size& sz_, bool& changed);
private:
    Size sz;
};

WindowSize::WindowSize() : sz()
{
}

WindowSize::WindowSize(sngxml::dom::Element* element)
{
    std::u32string widthAttr = element->GetAttribute(U"width");
    if (!widthAttr.empty())
    {
        sz.Width = boost::lexical_cast<int>(ToUtf8(widthAttr));
    }
    std::u32string heightAttr = element->GetAttribute(U"height");
    if (!heightAttr.empty())
    {
        sz.Height = boost::lexical_cast<int>(ToUtf8(heightAttr));
    }
}

sngxml::dom::Element* WindowSize::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"size");
    element->SetAttribute(U"width", ToUtf32(std::to_string(sz.Width)));
    element->SetAttribute(U"height", ToUtf32(std::to_string(sz.Height)));
    return element;
}

void WindowSize::SetSz(const Size& sz_, bool& changed)
{
    if (sz != sz_)
    {
        sz = sz_;
        changed = true;
    }
}

class WindowSettings
{
public:
    WindowSettings();
    WindowSettings(sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    bool IsNormalDefined() const { return normalDefined; }
    void SetNormalDefined() { normalDefined = true; }
    bool IsMaximizedDefined() const { return maximizedDefined; }
    void SetMaximizedDefined() { maximizedDefined = true; }
    bool IsChanged() const { return changed; }
    void ResetChanged();
    WindowState GetWindowState() const { return windowState; }
    void SetWindowState(WindowState windowState_);
    const Point& GetWindowLocation() const { return windowLocation.Loc(); }
    void SetWindowLocation(const Point& loc_);
    const Size& GetWindowSize() const { return windowSize.Sz(); }
    void SetWindowSize(const Size& sz_);
    int MaximizedHorizontalSplitterDistance() { return maximizedHorizontalSplitterDistance; }
    void SetMaximizedHorizontalSplitterDistance(int distance);
    int MaximizedVerticalSplitterDistance() const { return maximizedVerticalSplitterDistance; }
    void SetMaximizedVerticalSplitterDistance(int distance);
    int NormalHorizontalSplitterDistance() const { return normalHorizontalSplitterDistance; }
    void SetNormalHorizontalSplitterDistance(int distance);
    int NormalVerticalSplitterDistance() const { return normalVerticalSplitterDistance; }
    void SetNormalVerticalSplitterDistance(int distance);
private:
    bool normalDefined;
    bool maximizedDefined;
    bool changed;
    WindowState windowState;
    WindowLocation windowLocation;
    WindowSize windowSize;
    int maximizedHorizontalSplitterDistance;
    int maximizedVerticalSplitterDistance;
    int normalHorizontalSplitterDistance;
    int normalVerticalSplitterDistance;
};

WindowSettings::WindowSettings() : 
    normalDefined(false), 
    maximizedDefined(false),
    changed(false),
    windowState(WindowState::normal),
    windowLocation(),
    windowSize(),
    maximizedHorizontalSplitterDistance(0), 
    maximizedVerticalSplitterDistance(0), 
    normalHorizontalSplitterDistance(0), 
    normalVerticalSplitterDistance(0)
{
}

WindowSettings::WindowSettings(sngxml::dom::Element* element) : 
    normalDefined(false),
    maximizedDefined(false),
    changed(false),
    windowState(WindowState::normal),
    windowLocation(),
    windowSize(),
    maximizedHorizontalSplitterDistance(0),
    maximizedVerticalSplitterDistance(0),
    normalHorizontalSplitterDistance(0),
    normalVerticalSplitterDistance(0)
{
    std::u32string normalDefinedAttr = element->GetAttribute(U"normalDefined");
    if (normalDefinedAttr == U"true")
    {
        normalDefined = true;
    }
    else if (normalDefinedAttr == U"false")
    {
        normalDefined = false;
    }
    else
    {
        throw std::runtime_error("invalid configuration, normalDefined attribute not found");
    }
    std::u32string maximizedDefinedAttr = element->GetAttribute(U"maximizedDefined");
    if (maximizedDefinedAttr == U"true")
    {
        maximizedDefined = true;
    }
    else if (maximizedDefinedAttr == U"false")
    {
        maximizedDefined = false;
    }
    else
    {
        throw std::runtime_error("invalid configuration, maximizedDefined attribute not found");
    }
    std::u32string windowStateAttr = element->GetAttribute(U"windowState");
    if (!windowStateAttr.empty())
    {
        windowState = static_cast<WindowState>(boost::lexical_cast<int>(ToUtf8(windowStateAttr)));
    }
    std::u32string maximizedHorizontalSplitterDistanceAttr = element->GetAttribute(U"maximizedHorizontalSplitterDistance");
    if (!maximizedHorizontalSplitterDistanceAttr.empty())
    {
        maximizedHorizontalSplitterDistance = boost::lexical_cast<int>(ToUtf8(maximizedHorizontalSplitterDistanceAttr));
    }
    std::u32string maximizedVerticalSplitterDistanceAttr = element->GetAttribute(U"maximizedVerticalSplitterDistance");
    if (!maximizedVerticalSplitterDistanceAttr.empty())
    {
        maximizedVerticalSplitterDistance = boost::lexical_cast<int>(ToUtf8(maximizedVerticalSplitterDistanceAttr));
    }
    std::u32string normalHorizontalSplitterDistanceAttr = element->GetAttribute(U"normalHorizontalSplitterDistance");
    if (!normalHorizontalSplitterDistanceAttr.empty())
    {
        normalHorizontalSplitterDistance = boost::lexical_cast<int>(ToUtf8(normalHorizontalSplitterDistanceAttr));
    }
    std::u32string normalVerticalSplitterDistanceAttr = element->GetAttribute(U"normalVerticalSplitterDistance");
    if (!normalVerticalSplitterDistanceAttr.empty())
    {
        normalVerticalSplitterDistance = boost::lexical_cast<int>(ToUtf8(normalVerticalSplitterDistanceAttr));
    }
    std::unique_ptr<sngxml::xpath::XPathObject> windowLocationObject = sngxml::xpath::Evaluate(U"location", element);
    if (windowLocationObject)
    {
        if (windowLocationObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(windowLocationObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    windowLocation = WindowLocation(element);
                }
            }
            else
            {
                throw std::runtime_error("invalid configuration, single /configuration/windowSettings/location element expected");
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> windowSizeObject = sngxml::xpath::Evaluate(U"size", element);
    if (windowSizeObject)
    {
        if (windowSizeObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(windowSizeObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    windowSize = WindowSize(element);
                }
            }
            else
            {
                throw std::runtime_error("invalid configuration, single /configuration/windowSettings/size element expected");
            }
        }
    }
}

sngxml::dom::Element* WindowSettings::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"windowSettings");
    element->SetAttribute(U"normalDefined", normalDefined ? U"true" : U"false");
    element->SetAttribute(U"maximizedDefined", maximizedDefined ? U"true" : U"false");
    element->SetAttribute(U"windowState", ToUtf32(std::to_string(static_cast<int>(windowState))));
    element->SetAttribute(U"maximizedHorizontalSplitterDistance", ToUtf32(std::to_string(maximizedHorizontalSplitterDistance)));
    element->SetAttribute(U"maximizedVerticalSplitterDistance", ToUtf32(std::to_string(maximizedVerticalSplitterDistance)));
    element->SetAttribute(U"normalHorizontalSplitterDistance", ToUtf32(std::to_string(normalHorizontalSplitterDistance)));
    element->SetAttribute(U"normalVerticalSplitterDistance", ToUtf32(std::to_string(normalVerticalSplitterDistance)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowLocation.ToXml()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowSize.ToXml()));
    return element;
}

void WindowSettings::ResetChanged()
{
    changed = false;
}

void WindowSettings::SetWindowState(WindowState windowState_)
{
    if (windowState != windowState_)
    {
        windowState = windowState_;
        changed = true;
    }
}

void WindowSettings::SetWindowLocation(const Point& loc_)
{
    windowLocation.SetLoc(loc_, changed);
}

void WindowSettings::SetWindowSize(const Size& sz_)
{
    windowSize.SetSz(sz_, changed);
}

void WindowSettings::SetMaximizedHorizontalSplitterDistance(int distance)
{
    if (maximizedHorizontalSplitterDistance != distance)
    {
        maximizedHorizontalSplitterDistance = distance;
        changed = true;
    }
}

void WindowSettings::SetMaximizedVerticalSplitterDistance(int distance)
{
    if (maximizedVerticalSplitterDistance != distance)
    {
        maximizedVerticalSplitterDistance = distance;
        changed = true;
    }
}

void WindowSettings::SetNormalHorizontalSplitterDistance(int distance)
{
    if (normalHorizontalSplitterDistance != distance)
    {
        normalHorizontalSplitterDistance = distance;
        changed = true;
    }
}

void WindowSettings::SetNormalVerticalSplitterDistance(int distance)
{
    if (normalVerticalSplitterDistance != distance)
    {
        normalVerticalSplitterDistance = distance;
        changed = true;
    }
}

class Configuration
{
public:
    Configuration();
    Configuration(sngxml::dom::Document* document);
    sngxml::dom::Element* ToXml() const;
    WindowSettings& GetWindowSettings() { return *windowSettings; }
    ViewSettings& GetViewSettings() { return *viewSettings; }
private:
    std::unique_ptr<WindowSettings> windowSettings;
    std::unique_ptr<ViewSettings> viewSettings;
};

Configuration::Configuration() : windowSettings(new WindowSettings()), viewSettings(new ViewSettings())
{
}

Configuration::Configuration(sngxml::dom::Document* document)
{
    std::unique_ptr<sngxml::xpath::XPathObject> windowSettingsObject = sngxml::xpath::Evaluate(U"/configuration/windowSettings", document);
    if (windowSettingsObject)
    {
        if (windowSettingsObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(windowSettingsObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    windowSettings.reset(new WindowSettings(element));
                }
            }
            else
            {
                throw std::runtime_error("invalid configuration, single /configuration/windowSettings element expected");
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> viewSettingsObject = sngxml::xpath::Evaluate(U"/configuration/viewSettings", document);
    if (viewSettingsObject)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(viewSettingsObject.get());
        int n = nodeSet->Length();
        if (n == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                viewSettings.reset(new ViewSettings(element));
            }
        }
        else
        {
            throw std::runtime_error("invalid configuration, single /configuration/viewSettings element expected");
        }
    }
}

sngxml::dom::Element* Configuration::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"configuration");
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowSettings->ToXml()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(viewSettings->ToXml()));
    return element;
}

class Config
{
public:
    static void Init();
    static void Done();
    static Config& Instance() { return *instance; }
    Configuration& GetConfiguration() { return *configuration; }
    void SetConfiguration(Configuration* configuration_);
    const std::string& FilePath() const { return filePath; }
private:
    Config();
    static std::unique_ptr<Config> instance;
    std::unique_ptr<Configuration> configuration;
    std::string filePath;
};

std::unique_ptr<Config> Config::instance;

Config::Config() : configuration(new Configuration()), filePath(Path::Combine(GetFullPath(wingstall::config::WingstallConfigDir()), "package_editor.config.xml"))
{
}

void Config::SetConfiguration(Configuration* configuration_)
{
    configuration.reset(configuration_);
}

void Config::Init()
{
    instance.reset(new Config());
}

void Config::Done()
{
    instance.reset();
}

ViewSettings& GetConfiguredViewSettings()
{
    return Config::Instance().GetConfiguration().GetViewSettings();
}

bool IsNormalWindowSettingsDefined()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().IsNormalDefined();
}

void SetNormalWindowSettingsDefined()
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetNormalDefined();
}

bool IsMaximizedWindowSettingsDefined()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().IsMaximizedDefined();
}

void SetMaximizedWindowSettingsDefined()
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetMaximizedDefined();
}

void SetWindowState(WindowState windowState)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetWindowState(windowState);
}

void SetWindowLocation(const Point& location)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetWindowLocation(location);
}

void SetWindowSize(const Size& size)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetWindowSize(size);
}

void SetNormalHorizontalSplitterDistance(int distance)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetNormalHorizontalSplitterDistance(distance);
}

void SetNormalVerticalSplitterDistance(int distance)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetNormalVerticalSplitterDistance(distance);
}

void SetMaximizedHorizontalSplitterDistance(int distance)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetMaximizedHorizontalSplitterDistance(distance);
}

void SetMaximizedVerticalSplitterDistance(int distance)
{
    Config::Instance().GetConfiguration().GetWindowSettings().SetMaximizedVerticalSplitterDistance(distance);
}

WindowState GetConfiguredWindowState()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().GetWindowState();
}

Point GetConfiguredWindowLocation()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().GetWindowLocation();
}

Size GetConfiguredWindowSize()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().GetWindowSize();
}

int GetConfiguredNormalHorizontalSplitterDistance()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().NormalHorizontalSplitterDistance();
}

int GetConfiguredNormalVerticalSplitterDistance()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().NormalVerticalSplitterDistance();
}

int GetConfiguredMaximizedHorizontalSplitterDistance()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().MaximizedHorizontalSplitterDistance();
}

int GetConfiguredMaximizedVerticalSplitterDistance()
{
    return Config::Instance().GetConfiguration().GetWindowSettings().MaximizedVerticalSplitterDistance();
}

bool IsConfigurationChanged()
{
    if (Config::Instance().GetConfiguration().GetWindowSettings().IsChanged())
    {
        return true;
    }
    if (Config::Instance().GetConfiguration().GetViewSettings().IsChanged())
    {
        return true;
    }
    return false;
}

void LoadConfiguration()
{
    if (boost::filesystem::exists(Config::Instance().FilePath()))
    {
        std::unique_ptr<sngxml::dom::Document> configurationDoc = sngxml::dom::ReadDocument(Config::Instance().FilePath());
        Config::Instance().SetConfiguration(new Configuration(configurationDoc.get()));
    }
}

void SaveConfiguration()
{
    sngxml::dom::Document configurationDoc;
    configurationDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(Config::Instance().GetConfiguration().ToXml()));
    std::ofstream configurationFile(Config::Instance().FilePath());
    CodeFormatter formatter(configurationFile);
    formatter.SetIndentSize(1);
    configurationDoc.Write(formatter);
    Config::Instance().GetConfiguration().GetWindowSettings().ResetChanged();
    Config::Instance().GetConfiguration().GetViewSettings().ResetChanged();
}

void ResetConfiguration()
{
    InitConfiguration();
    if (boost::filesystem::exists(Config::Instance().FilePath()))
    {
        boost::system::error_code ec;
        boost::filesystem::remove(Config::Instance().FilePath(), ec);
        if (ec)
        {
            throw std::runtime_error("could not remove '" + Config::Instance().FilePath() + "': " + PlatformStringToUtf8(ec.message()));
        }
    }
}

void InitConfiguration()
{
    Config::Init();
}

void DoneConfiguration()
{
    Config::Done();
}

} } // wingstall::package_editor
