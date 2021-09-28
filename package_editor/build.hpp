// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_BUILD_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_BUILD_INCLUDED
#include <wingpackage/package.hpp>
#include <wing/LogView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class MainWindow;

class BuildTask : wingpackage::PackageObserver
{
public:
    BuildTask(MainWindow* mainWindow_);
    void Run();
    void Cancel();
    void StatusChanged(wingpackage::Package* package) override;
    void LogError(wingpackage::Package* package, const std::string& error) override;
    void FileContentPositionChanged(wingpackage::Package* package) override;
    void FileCopyProgress(int64_t pos, int64_t size);
private:
    void MakeCompileBatches(
        const std::string& packageBinFilePath,
        std::string& compileDebugFilePath,
        std::string& debugStdoutFilePath,
        std::string& debugStderrFilePath,
        std::string& compileReleaseFilePath,
        std::string& releaseStdoutFilePath,
        std::string& releaseStderrFilePath);
    void Compile(const std::string& compileBatFilePath, const std::string& stdOutFilePath, const std::string& stdErrFilePath, int startPercent, int endPercent);
    void Copy(const std::string& setupFilePath, const std::string& binDirectory);
    bool buildCanceled;
    wingstall::wingpackage::Package* package;
    MainWindow* mainWindow;
    LogView* log;
    int prevPercent;
};

void RunBuildTask(BuildTask* buildTask);

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_BUILD_INCLUDED
