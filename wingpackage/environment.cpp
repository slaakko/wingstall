// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/environment.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/path_matcher.hpp>
#include <wing/Environment.hpp>
#include <soulng/util/Unicode.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::unicode;
using namespace wing;

EnvironmentVariable::EnvironmentVariable() : Node(NodeKind::environmentVariable), flags(EnvironmentVariableFlags::none)
{
}

EnvironmentVariable::EnvironmentVariable(const std::string& name_, const std::string& value_) : Node(NodeKind::environmentVariable, name_), value(value_), flags(EnvironmentVariableFlags::none)
{
}

EnvironmentVariable::EnvironmentVariable(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::environmentVariable), flags(EnvironmentVariableFlags::none)
{
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw std::runtime_error("environment variable element has no 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
    }
    std::u32string valueAttr = element->GetAttribute(U"value");
    if (!valueAttr.empty())
    {
        value = ToUtf8(valueAttr);
    }
    else
    {
        throw std::runtime_error("environment variable element has no 'value' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
    }
}

sngxml::dom::Element* EnvironmentVariable::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"variable");
    element->SetAttribute(U"name", ToUtf32(Name()));
    element->SetAttribute(U"value", ToUtf32(value));
    element->SetAttribute(U"oldValue", ToUtf32(oldValue));
    return element;
}

void EnvironmentVariable::SetFlag(EnvironmentVariableFlags flag, bool value)
{
    if (value) flags = flags | flag;
    else flags = flags & ~flag;
}

void EnvironmentVariable::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(value);
    writer.Write(oldValue);
    writer.Write(static_cast<uint8_t>(flags));
}

void EnvironmentVariable::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    value = reader.ReadUtf8String();
    oldValue = reader.ReadUtf8String();
    flags = static_cast<EnvironmentVariableFlags>(reader.ReadByte());
}

void EnvironmentVariable::Install()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        try
        {
            SetFlag(EnvironmentVariableFlags::exists, HasSystemEnvironmentVariable(Name()));
            if (GetFlag(EnvironmentVariableFlags::exists))
            {
                oldValue = GetSystemEnvironmentVariable(Name());
            }
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("could not get value of environment variable '" + Name() + "': " + ex.what());
        }
        std::string path = package->ExpandPath(value);
        try
        {
            SetSystemEnvironmentVariable(Name(), path, RegistryValueKind::regSz);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("could not create environment variable '" + Name() + "': " + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void EnvironmentVariable::SetOldValue()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            SetSystemEnvironmentVariable(Name(), oldValue, RegistryValueKind::regSz);
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not set old value of environment variable '" + Name() + "': " + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void EnvironmentVariable::Remove()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            DeleteSystemEnvironmentVariable(Name());
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not remove environment variable '" + Name() + "': " + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void EnvironmentVariable::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        Node::Uninstall();
        if (GetFlag(EnvironmentVariableFlags::exists) && !oldValue.empty())
        {
            SetOldValue();
        }
        else
        {
            Remove();
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

PathDirectory::PathDirectory() : Node(NodeKind::pathDirectory), flags(PathDirectoryFlags::none)
{
}

PathDirectory::PathDirectory(const std::string& value_) : Node(NodeKind::pathDirectory), value(value_), flags(PathDirectoryFlags::none)
{
}

PathDirectory::PathDirectory(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::pathDirectory), flags(PathDirectoryFlags::none)
{
    std::u32string valueAttr = element->GetAttribute(U"value");
    if (!valueAttr.empty())
    {
        value = ToUtf8(valueAttr);
    }
    else
    {
        throw std::runtime_error("path directory element has no 'value' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
    }
}

sngxml::dom::Element* PathDirectory::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"pathDirectory");
    element->SetAttribute(U"value", ToUtf32(value));
    return element;
}

void PathDirectory::WriteIndex(BinaryStreamWriter& writer)
{
    Node::WriteIndex(writer);
    writer.Write(value);
    writer.Write(expandedValue);
    writer.Write(static_cast<uint8_t>(flags));
}

void PathDirectory::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    value = reader.ReadUtf8String();
    expandedValue = reader.ReadUtf8String();
    flags = static_cast<PathDirectoryFlags>(reader.ReadByte());
}

void PathDirectory::Install()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        expandedValue = package->ExpandPath(value);
        SetFlag(PathDirectoryFlags::exists, HasPathDirectory(expandedValue));
        if (!GetFlag(PathDirectoryFlags::exists))
        {
            AppendPathDirectory(expandedValue);
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void PathDirectory::Remove()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            RemovePathDirectory(expandedValue);
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not remove directory '" + expandedValue +"' from PATH environment variable :" + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void PathDirectory::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        Node::Uninstall();
        if (!GetFlag(PathDirectoryFlags::exists))
        {
            Remove();
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void PathDirectory::SetFlag(PathDirectoryFlags flag, bool value)
{
    if (value) flags = flags | flag;
    else flags = flags & ~flag;
}

Environment::Environment() : Node(NodeKind::environment, "environment")
{
}

Environment::Environment(PathMatcher& pathMatcher, sngxml::dom::Element* element) : Node(NodeKind::environment, "environment")
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
                    EnvironmentVariable* variable = new EnvironmentVariable(pathMatcher, element);
                    AddVariable(variable);
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
                    PathDirectory* pathDirectory = new PathDirectory(pathMatcher, element);
                    AddPathDirectory(pathDirectory);
                }
            }
        }
    }
}

void Environment::AddVariable(EnvironmentVariable* variable)
{
    variable->SetParent(this);
    variables.push_back(std::unique_ptr<EnvironmentVariable>(variable));
}

void Environment::AddPathDirectory(PathDirectory* pathDirectory)
{
    pathDirectory->SetParent(this);
    pathDirectories.push_back(std::unique_ptr<PathDirectory>(pathDirectory));
}

void Environment::WriteIndex(BinaryStreamWriter& writer)
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
    }
    Node::WriteIndex(writer);
    int32_t numVariables = variables.size();
    writer.Write(numVariables);
    for (int32_t i = 0; i < numVariables; ++i)
    {
        EnvironmentVariable* variable = variables[i].get();
        variable->WriteIndex(writer);
    }
    int32_t numPathDirectories = pathDirectories.size();
    writer.Write(numPathDirectories);
    for (int32_t i = 0; i < numPathDirectories; ++i)
    {
        PathDirectory* pathDirectory = pathDirectories[i].get();
        pathDirectory->WriteIndex(writer);
    }
}

void Environment::ReadIndex(BinaryStreamReader& reader)
{
    Node::ReadIndex(reader);
    int32_t numVariables = reader.ReadInt();
    for (int32_t i = 0; i < numVariables; ++i)
    {
        EnvironmentVariable* variable = new EnvironmentVariable();
        AddVariable(variable);
        variable->ReadIndex(reader);
    }
    int32_t numPathDirectories = reader.ReadInt();
    for (int32_t i = 0; i < numPathDirectories; ++i)
    {
        PathDirectory* pathDirectory = new PathDirectory();
        AddPathDirectory(pathDirectory);
        pathDirectory->ReadIndex(reader);
    }
}

sngxml::dom::Element* Environment::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"environment");
    for (const auto& variable : variables)
    {
        sngxml::dom::Element* child = variable->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    for (const auto& pathDirectory : pathDirectories)
    {
        sngxml::dom::Element* child = pathDirectory->ToXml();
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(child));
    }
    return element;
}

void Environment::Install()
{
    Package* package = GetPackage();
    if (package)
    {
        package->CheckInterrupted();
        for (const auto& variable : variables)
        {
            variable->Install();
        }
        for (const auto& pathDirectory : pathDirectories)
        {
            pathDirectory->Install();
        }
        try
        {
            BroadcastEnvironmentChangedMessage();
        }
        catch (const std::exception& ex)
        {
            std::string what = ex.what();
            int x = 0;
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void Environment::Uninstall()
{
    Node::Uninstall();
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            package->CheckInterrupted();
            for (const auto& variable : variables)
            {
                variable->Uninstall();
            }
            for (const auto& pathDirectory : pathDirectories)
            {
                pathDirectory->Uninstall();
            }
            try
            {
                BroadcastEnvironmentChangedMessage();
            }
            catch (const std::exception& ex)
            {
                package->LogError(ex.what());
            }
        }
        catch (const std::exception& ex)
        {
            package->LogError(ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

} } // namespace wingstall::wingpackage
