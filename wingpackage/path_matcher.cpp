// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/path_matcher.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;
using namespace soulng::unicode;

FileInfo::FileInfo(const std::string& name_, uintmax_t size_, std::time_t time_) : name(name_), size(size_), time(time_)
{
}

DirectoryInfo::DirectoryInfo(const std::string& name_, std::time_t time_, sngxml::dom::Element* element_) : name(name_), time(time_), element(element_)
{
}

PathRule::PathRule(PathMatcher& pathMatcher, std::string name_, RuleKind ruleKind_, PathKind pathKind_) :
    element(nullptr), name(name_), ruleKind(ruleKind_), pathKind(pathKind_)
{
    nfa = soulng::rex::CompileFilePattern(pathMatcher.GetContext(), ToUtf32(name));
}

PathRule::PathRule(PathMatcher& pathMatcher, sngxml::dom::Element* element_) :
    element(element_), ruleKind(RuleKind::none), pathKind(PathKind::none)
{
    if (element->Name() == U"include" || element->Name() == U"directory" || element->Name() == U"file")
    {
        ruleKind = ruleKind | RuleKind::include;
        if (element->Name() == U"include")
        {
            std::u32string cascadeAttr = element->GetAttribute(U"cascade");
            if (!cascadeAttr.empty())
            {
                if (cascadeAttr == U"true")
                {
                    ruleKind = ruleKind | RuleKind::cascade;
                }
            }
        }
    }
    else if (element->Name() == U"exclude")
    {
        ruleKind = ruleKind | RuleKind::exclude;
        std::u32string cascadeAttr = element->GetAttribute(U"cascade");
        if (!cascadeAttr.empty())
        {
            if (cascadeAttr == U"true")
            {
                ruleKind = ruleKind | RuleKind::cascade;
            }
        }
    }
    else
    {
        throw std::runtime_error("unknown path matching rule '" + ToUtf8(element->Name()) + "' in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
            std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
    }
    if (element->Name() == U"directory" || element->Name() == U"file")
    {
        std::u32string nameAttr = element->GetAttribute(U"name");
        if (!nameAttr.empty())
        {
            if (element->Name() == U"directory")
            {
                pathKind = PathKind::dir;
                if (nameAttr.find('/') != std::u32string::npos ||
                    (nameAttr.find('\\') != std::u32string::npos))
                {
                    throw std::runtime_error("the value of the 'name' attribute may not have '/' or '\\' characters in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                        std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                }
                nfa = soulng::rex::CompileFilePattern(pathMatcher.GetContext(), nameAttr);
                name = ToUtf8(nameAttr);
            }
            else if (element->Name() == U"file")
            {
                pathKind = PathKind::file;
                if (nameAttr.find('/') != std::u32string::npos ||
                    (nameAttr.find('\\') != std::u32string::npos))
                {
                    throw std::runtime_error("the value of the 'name' attribute may not have '/' or '\\' characters in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                        std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
                }
                nfa = soulng::rex::CompileFilePattern(pathMatcher.GetContext(), nameAttr);
                name = ToUtf8(nameAttr);
            }
        }
        else
        {
            throw std::runtime_error("path matching rule element 'directory' or 'file' must have 'name' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
        }
    }
    else
    {
        std::u32string dirAttr = element->GetAttribute(U"dir");
        if (!dirAttr.empty())
        {
            pathKind = PathKind::dir;
            if (dirAttr.find('/') != std::u32string::npos ||
                (dirAttr.find('\\') != std::u32string::npos))
            {
                throw std::runtime_error("the value of the 'dir' attribute may not have '/' or '\\' characters in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                    std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
            }
            nfa = soulng::rex::CompileFilePattern(pathMatcher.GetContext(), dirAttr);
            name = ToUtf8(dirAttr);
        }
        std::u32string fileAttr = element->GetAttribute(U"file");
        if (!fileAttr.empty())
        {
            if (pathKind == PathKind::dir)
            {
                throw std::runtime_error("path matching rule element 'include' or 'exclude' cannot have both 'dir' and 'file' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                    std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
            }
            pathKind = PathKind::file;
            if (fileAttr.find('/') != std::u32string::npos ||
                (fileAttr.find('\\') != std::u32string::npos))
            {
                throw std::runtime_error("the value of the 'file' attribute may not have '/' or '\\' characters in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                    std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
            }
            nfa = soulng::rex::CompileFilePattern(pathMatcher.GetContext(), fileAttr);
            name = ToUtf8(fileAttr);
        }
        if (dirAttr.empty() && fileAttr.empty())
        {
            throw std::runtime_error("path matching rule element 'include' or 'exclude' must have either 'dir' or 'file' attribute in package XML document '" + pathMatcher.XmlFilePath() + "' line " + 
                std::to_string(element->GetSourcePos().line) + ", column " + std::to_string(element->GetSourcePos().col));
        }
    }
}

bool PathRule::Matches(const std::string& name)
{
    return soulng::rex::PatternMatch(ToUtf32(name), nfa);
}

PathRuleSet::PathRuleSet(PathRuleSet* parentRuleSet_) : parentRuleSet(parentRuleSet_)
{
}

void PathRuleSet::AddRule(PathRule* rule)
{
    rules.push_back(std::unique_ptr<PathRule>(rule));
    ruleMap[rule->Name()] = rule;
}

PathRule* PathRuleSet::GetRule(const std::string& name) const
{
    auto it = ruleMap.find(name);
    if (it != ruleMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

bool PathRuleSet::IncludeDir(const std::string& dirName) const
{
    bool include = true;
    bool matched = false;
    for (const auto& rule : rules)
    {
        if (rule->GetPathKind() == PathKind::dir)
        {
            if ((rule->GetRuleKind() & RuleKind::include) != RuleKind::none && rule->Matches(dirName))
            {
                include = true;
                matched = true;
            }
            else if ((rule->GetRuleKind() & RuleKind::exclude) != RuleKind::none && rule->Matches(dirName))
            {
                include = false;
                matched = true;
            }
        }
    }
    PathRuleSet* parent = parentRuleSet;
    while (parent && !matched)
    {
        for (const auto& rule : parent->rules)
        {
            if (rule->GetPathKind() == PathKind::dir)
            {
                if ((rule->GetRuleKind() & RuleKind::cascade) != RuleKind::none)
                {
                    if ((rule->GetRuleKind() & RuleKind::include) != RuleKind::none && rule->Matches(dirName))
                    {
                        include = true;
                        matched = true;
                    }
                    else if ((rule->GetRuleKind() & RuleKind::exclude) != RuleKind::none && rule->Matches(dirName))
                    {
                        include = false;
                        matched = true;
                    }
                }
            }
        }
        parent = parent->parentRuleSet;
    }
    return include;
}

bool PathRuleSet::IncludeFile(const std::string& fileName) const
{
    bool include = true;
    bool matched = false;
    for (const auto& rule : rules)
    {
        if (rule->GetPathKind() == PathKind::file)
        {
            if ((rule->GetRuleKind() & RuleKind::include) != RuleKind::none && rule->Matches(fileName))
            {
                include = true;
                matched = true;
            }
            else if ((rule->GetRuleKind() & RuleKind::exclude) != RuleKind::none && rule->Matches(fileName))
            {
                include = false;
                matched = true;
            }
        }
    }
    PathRuleSet* parent = parentRuleSet;
    while (parent && !matched)
    {
        for (const auto& rule : parent->rules)
        {
            if (rule->GetPathKind() == PathKind::file)
            {
                if ((rule->GetRuleKind() & RuleKind::cascade) != RuleKind::none)
                {
                    if ((rule->GetRuleKind() & RuleKind::include) != RuleKind::none && rule->Matches(fileName))
                    {
                        include = true;
                        matched = true;
                    }
                    else if ((rule->GetRuleKind() & RuleKind::exclude) != RuleKind::none && rule->Matches(fileName))
                    {
                        include = false;
                        matched = true;
                    }
                }
            }
        }
        parent = parent->parentRuleSet;
    }
    return include;
}

PathMatcher::PathMatcher(const std::string& xmlFilePath_) : xmlFilePath(xmlFilePath_), rootDir(Path::GetDirectoryName(GetFullPath(xmlFilePath)))
{
}

void PathMatcher::SetSourceRootDir(const std::string& sourceRootDir_)
{
    sourceRootDir = GetFullPath(Path::Combine(rootDir, sourceRootDir_));
    currentDir = sourceRootDir;
}

void PathMatcher::BeginFiles(sngxml::dom::Element* element)
{
    PathRuleSet* parentRuleSet = ruleSet.get();
    ruleSetStack.push(std::move(ruleSet));
    ruleSet.reset(new PathRuleSet(parentRuleSet));
    ruleSet->AddRule(new PathRule(*this, "*", RuleKind::exclude, PathKind::file));
    std::unique_ptr<sngxml::xpath::XPathObject> ruleObject = sngxml::xpath::Evaluate(U"file", element);
    if (ruleObject)
    {
        if (ruleObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(ruleObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* childElement = static_cast<sngxml::dom::Element*>(node);
                    PathRule* rule = new PathRule(*this, childElement);
                    ruleSet->AddRule(rule);
                }
            }
        }
    }
}

void PathMatcher::EndFiles()
{
    ruleSet = std::move(ruleSetStack.top());
    ruleSetStack.pop();
}

void PathMatcher::BeginDirectory(const std::string& name, sngxml::dom::Element* element)
{
    PathRuleSet* parentRuleSet = ruleSet.get();
    ruleSetStack.push(std::move(ruleSet));
    ruleSet.reset(new PathRuleSet(parentRuleSet));
    dirStack.push(currentDir);
    currentDir = Path::Combine(currentDir, name);
    if (element)
    {
        std::unique_ptr<sngxml::xpath::XPathObject> ruleObject = sngxml::xpath::Evaluate(U"*", element);
        if (ruleObject)
        {
            if (ruleObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(ruleObject.get());
                int n = nodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    sngxml::dom::Node* node = (*nodeSet)[i];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* childElement = static_cast<sngxml::dom::Element*>(node);
                        PathRule* rule = new PathRule(*this, childElement);
                        ruleSet->AddRule(rule);
                    }
                }
            }
        }
    }
}

void PathMatcher::EndDirectory()
{
    ruleSet = std::move(ruleSetStack.top());
    ruleSetStack.pop();
    currentDir = std::move(dirStack.top());
    dirStack.pop();
}

std::vector<DirectoryInfo> PathMatcher::Directories() const
{
    std::vector<DirectoryInfo> directories;
    boost::system::error_code ec;
    boost::filesystem::directory_iterator it(currentDir, ec);
    if (ec)
    {
        throw std::runtime_error("could not iterate directory '" + currentDir + "': " + PlatformStringToUtf8(ec.message()));
    }
    while (it != boost::filesystem::directory_iterator())
    {
        if (boost::filesystem::is_directory(it->status()))
        {
            if (!it->path().filename_is_dot() && !it->path().filename_is_dot_dot())
            {
                std::string name = it->path().filename().generic_string();
                if (ruleSet->IncludeDir(name))
                {
                    sngxml::dom::Element* element = nullptr;
                    PathRule* rule = ruleSet->GetRule(name);
                    if (rule)
                    {
                        element = rule->GetElement();
                    }
                    std::time_t time = boost::filesystem::last_write_time(it->path());
                    DirectoryInfo directoryInfo(name, time, element);
                    directories.push_back(directoryInfo);
                }
            }
        }
        ++it;
    }
    return directories;
}

std::vector<FileInfo> PathMatcher::Files() const
{
    std::vector<FileInfo> files;
    boost::system::error_code ec;
    boost::filesystem::directory_iterator it(currentDir, ec);
    if (ec)
    {
        throw std::runtime_error("could not iterate directory '" + currentDir + "': " + PlatformStringToUtf8(ec.message()));
    }
    while (it != boost::filesystem::directory_iterator())
    {
        if (boost::filesystem::is_regular_file(it->status()))
        {
            std::string name = it->path().filename().generic_string();
            if (ruleSet->IncludeFile(name))
            {
                FileInfo fileInfo(name, boost::filesystem::file_size(it->path()), boost::filesystem::last_write_time(it->path()));
                files.push_back(fileInfo);
            }
        }
        ++it;
    }
    return files;
}

} } // namespace wingstall::wingpackage
