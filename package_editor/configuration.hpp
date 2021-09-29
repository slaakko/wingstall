// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_CONFIGURATION_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_CONFIGURATION_INCLUDED
#include <wing/Window.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

const int configurationSavePeriod = 3000;
const int configurationSaveTimerId = 1;

class View;

class ColumnWidth
{
public:
    ColumnWidth();
    ColumnWidth(View* view_, const std::string& name_);
    void FromXml(sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    const std::string& Name() const { return name; }
    bool IsDefined() const { return defined; }
    void SetDefined() { defined = true; }
    int Get() const { return value; }
    void Set(int value_);
private:
    View* view;
    std::string name;
    bool defined;
    int value;
};

class ViewSettings;

class View
{
public:
    View();
    View(ViewSettings* viewSettings_, const std::string& viewName_);
    void FromXml(sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    const std::string& Name() const { return viewName; }
    void SetChanged();
    ColumnWidth& GetColumnWidth(const std::string& columnName);
private:
    ViewSettings* viewSettings;
    std::string viewName;
    std::map<std::string, ColumnWidth> columnWidthMap;
};

class ViewSettings
{
public:
    ViewSettings();
    ViewSettings(sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
    bool IsChanged() const { return changed; }
    void SetChanged();
    void ResetChanged();
    View& GetView(const std::string& name);
private:
    std::map<std::string, View> viewMap;
    bool changed;
};

void SetWindowState(WindowState windowState);
void SetWindowLocation(const Point& location);
void SetWindowSize(const Size& size);
void SetNormalHorizontalSplitterDistance(int distance);
void SetNormalVerticalSplitterDistance(int distance);
void SetMaximizedHorizontalSplitterDistance(int distance);
void SetMaximizedVerticalSplitterDistance(int distance);
WindowState GetConfiguredWindowState();
Point GetConfiguredWindowLocation();
Size GetConfiguredWindowSize();
int GetConfiguredNormalHorizontalSplitterDistance();
int GetConfiguredNormalVerticalSplitterDistance();
int GetConfiguredMaximizedHorizontalSplitterDistance();
int GetConfiguredMaximizedVerticalSplitterDistance();

ViewSettings& GetConfiguredViewSettings();

bool IsNormalWindowSettingsDefined();
void SetNormalWindowSettingsDefined();
bool IsMaximizedWindowSettingsDefined();
void SetMaximizedWindowSettingsDefined();
bool IsConfigurationChanged();
void LoadConfiguration();
void SaveConfiguration();
void ResetConfiguration();
void InitConfiguration();
void DoneConfiguration();

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_CONFIGURATION_INCLUDED
