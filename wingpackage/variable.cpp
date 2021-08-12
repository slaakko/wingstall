// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/variable.hpp>
#include <wingpackage/package.hpp>
#include <wing/Shell.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace wingpackage {

using namespace cmajor::wing;
using namespace soulng::unicode;

Variable::Variable(const std::string& name_) : Node(NodeKind::variable, name_)
{
}

std::string Variable::Value() const
{
    return std::string();
}

sngxml::dom::Element* Variable::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"variable");
    element->SetAttribute(U"name", ToUtf32(Name()));
    element->SetAttribute(U"value", ToUtf32(Value()));
    return element;
}

TargetRootDirVariable::TargetRootDirVariable() : Variable("TARGET_ROOT_DIR")
{
}

std::string TargetRootDirVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->TargetRootDir();
    }
    else
    {
        return Variable::Value();
    }
}

AppNameVariable::AppNameVariable() : Variable("APP_NAME")
{
}

std::string AppNameVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->AppName();
    }
    else
    {
        return Variable::Value();
    }
}

AppVersionVariable::AppVersionVariable() : Variable("APP_VERSION")
{
}

std::string AppVersionVariable::Value() const
{
    Package* package = GetPackage();
    if (package)
    {
        return package->Version();
    }
    else
    {
        return Variable::Value();
    }
}

StartMenuProgramsFolderVariable::StartMenuProgramsFolderVariable() : Variable("START_MENU_PROGRAMS_FOLDER")
{
}

std::string StartMenuProgramsFolderVariable::Value() const
{
    try
    {
        return GetStartMenuProgramsFolderPath();
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not get start menu programs folder path: " + std::string(ex.what()));
    }
}

DesktopFolderVariable::DesktopFolderVariable() : Variable("DESKTOP_FOLDER")
{
}

std::string DesktopFolderVariable::Value() const
{
    try
    {
        return GetDesktopFolderPath();
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not get desktop folder path: " + std::string(ex.what()));
    }
}

ProgramFilesDirVariable::ProgramFilesDirVariable() : Variable("PROGRAM_FILES_DIR")
{
}

std::string ProgramFilesDirVariable::Value() const
{
    try
    {
        return GetProgramFilesDirectoryPath();
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not get program files folder path: " + std::string(ex.what()));
    }
}

Variables::Variables() : Node(NodeKind::variables, "variables")
{
    AddVariable(new TargetRootDirVariable());
    AddVariable(new AppNameVariable());
    AddVariable(new AppVersionVariable());
    AddVariable(new StartMenuProgramsFolderVariable());
    AddVariable(new DesktopFolderVariable());
    AddVariable(new ProgramFilesDirVariable());
}

void Variables::AddVariable(Variable* variable)
{
    variable->SetParent(this);
    variables.push_back(std::unique_ptr<Variable>(variable));
    variableMap[variable->Name()] = variable;
}

Variable* Variables::GetVariable(const std::string& name) const
{
    auto it = variableMap.find(name);
    if (it != variableMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

sngxml::dom::Element* Variables::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"variables");
    for (const auto& variable : variables)
    {
        sngxml::dom::Element* child = variable->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    return element;
}

std::string Variables::ExpandPath(const std::string& path) const
{
    std::string result;
    std::string variableName;
    int state = 0;
    for (char c : path)
    {
        switch (state)
        {
            case 0:
            {
                if (c == '$')
                {
                    variableName.clear();
                    state = 1;
                }
                else
                {
                    result.append(1, c);
                }
                break;
            }
            case 1:
            {
                if (c == '$')
                {
                    Variable* variable = GetVariable(variableName);
                    if (variable)
                    {
                        result.append(variable->Value());
                    }
                    else
                    {
                        throw std::runtime_error("installation variable '" + variableName + "' not found");
                    }
                    state = 0;
                }
                else
                {
                    variableName.append(1, c);
                }
                break;
            }
        }
    }
    if (state == 1)
    {
        throw std::runtime_error("installation variable '" + variableName + "' has no ending '$' character");
    }
    return MakeNativePath(result);
}

} } // namespace wingstall::wingpackage
