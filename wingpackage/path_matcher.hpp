// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_PATH_MATCHER_INCLUDED
#define WINGSTALL_WINGPACKAGE_PATH_MATCHER_INCLUDED
#include <sngxml/dom/Element.hpp>
#include <soulng/rex/Context.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/rex/Nfa.hpp>
#include <stack>
#include <ctime>

namespace wingstall { namespace wingpackage {

struct FileInfo
{
    FileInfo(const std::string& name_, uintmax_t size_, std::time_t time_);
    std::string name;
    uintmax_t size;
    time_t time;
};

struct DirectoryInfo
{
    DirectoryInfo(const std::string& name_, std::time_t time_, sngxml::dom::Element* element_);
    std::string name;
    std::time_t time;
    sngxml::dom::Element* element;
};

class PathMatcher;

enum class RuleKind : int
{
    none = 0, include = 1 << 0, exclude = 1 << 1, cascade = 1 << 2
};

inline RuleKind operator|(RuleKind left, RuleKind right)
{
    return RuleKind(int(left) | int(right));
}

inline RuleKind operator&(RuleKind left, RuleKind right)
{
    return RuleKind(int(left) & int(right));
}

inline RuleKind operator~(RuleKind operand)
{
    return RuleKind(~int(operand));
}

enum class PathKind
{
    none, dir, file
};

class PathRule
{
public:
    PathRule(PathMatcher& pathMatcher, std::string name_, RuleKind ruleKind_, PathKind pathKind_);
    PathRule(PathMatcher& pathMatcher, sngxml::dom::Element* element_);
    PathKind GetPathKind() const { return pathKind; }
    RuleKind GetRuleKind() const { return ruleKind; }
    sngxml::dom::Element* GetElement() const { return element; }
    const std::string& Name() const { return name; }
    bool Matches(const std::string& name);
private:
    sngxml::dom::Element* element;
    std::string name;
    RuleKind ruleKind;
    PathKind pathKind;
    soulng::rex::Nfa nfa;
};

class PathRuleSet
{
public:
    PathRuleSet(PathRuleSet* parentRuleSet_);
    PathRuleSet* GetParentRuleSet() const { return parentRuleSet; }
    void AddRule(PathRule* rule);
    PathRule* GetRule(const std::string& name) const;
    bool IncludeDir(const std::string& dirName) const;
    bool IncludeFile(const std::string& fileName) const;
private:
    PathRuleSet* parentRuleSet;
    std::vector<std::unique_ptr<PathRule>> rules;
    std::map<std::string, PathRule*> ruleMap;
};

class PathMatcher
{
public:
    PathMatcher(const std::string& xmlFilePath_);
    const std::string& SourceRootDir() const { return sourceRootDir; }
    void SetSourceRootDir(const std::string& sourceRootDir_);
    const std::string& XmlFilePath() const { return xmlFilePath; }
    void BeginFiles(sngxml::dom::Element* element);
    void EndFiles();
    void BeginDirectory(const std::string& name, sngxml::dom::Element* element);
    void EndDirectory();
    const std::string& CurrentDir() const { return currentDir; }
    soulng::rex::Context& GetContext() { return context; }
    std::vector<DirectoryInfo> Directories() const;
    std::vector<FileInfo> Files() const;
private:
    soulng::rex::Context context;
    std::string xmlFilePath;
    std::string rootDir;
    std::string sourceRootDir;
    std::stack<std::string> dirStack;
    std::string currentDir;
    std::stack<std::unique_ptr<PathRuleSet>> ruleSetStack;
    std::unique_ptr<PathRuleSet> ruleSet;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_PATH_MATCHER_INCLUDED
