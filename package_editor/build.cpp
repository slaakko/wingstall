// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/build.hpp>
#include <package_editor/main_window.hpp>
#include <wingpackage/path_matcher.hpp>
#include <wingpackage/make_setup.hpp>
#include <wingstall_config/config.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/FileUtil.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace wingstall { namespace package_editor {

using namespace soulng::util;
using namespace soulng::unicode;

class BuildLogger : public wingstall::wingpackage::Logger
{
public:
    BuildLogger(LogView* log_);
    void WriteLine(const std::string& line) override;
private:
    LogView* log;
};

BuildLogger::BuildLogger(LogView* log_) : log(log_)
{
}

void BuildLogger::WriteLine(const std::string& line) 
{
    log->WriteLine(line);
}

class BuildCopyFileObserver : public CopyFileObserver
{
public:
    BuildCopyFileObserver(LogView* log_, BuildTask* task_);
    void WriteLine(const std::string& line) override;
    void FileCopyProgress(int64_t pos, int64_t size) override;
private:
    LogView* log;
    BuildTask* task;
};

BuildCopyFileObserver::BuildCopyFileObserver(LogView* log_, BuildTask* task_) : log(log_), task(task_)
{
}

void BuildCopyFileObserver::WriteLine(const std::string& line)
{
    if (log)
    {
        log->WriteLine(line);
    }
}

void BuildCopyFileObserver::FileCopyProgress(int64_t pos, int64_t size)
{
    if (task)
    {
        task->FileCopyProgress(pos, size);
    }
}

BuildTask::BuildTask(MainWindow* mainWindow_) : mainWindow(mainWindow_), prevPercent(-1), log(nullptr), package(nullptr), buildCanceled(false)
{
}

void BuildTask::Run()
{
    try
    {
        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        log = mainWindow->GetLog();
        log->Clear();
        log->WriteLine("================ Building Package '" + mainWindow->GetPackage()->Name() + "' ================");
        log->WriteLine("Creating package index...");
        std::unique_ptr<sngxml::dom::Document> packageDoc = sngxml::dom::ReadDocument(mainWindow->GetPackage()->FilePath());
        wingpackage::PathMatcher pathMatcher(mainWindow->GetPackage()->FilePath());
        std::unique_ptr<wingpackage::Package> packagePtr(new wingpackage::Package(pathMatcher, packageDoc.get()));
        package = packagePtr.get();
        std::string xmlIndexFilePath = Path::ChangeExtension(mainWindow->GetPackage()->FilePath(), ".index.xml");
        package->WriteIndexToXmlFile(xmlIndexFilePath);
        log->WriteLine("==> " + xmlIndexFilePath);
        std::string packageBinFilePath = Path::ChangeExtension(mainWindow->GetPackage()->FilePath(), ".bin");
        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        log->WriteLine("Creating binary package " + packageBinFilePath + "...");
        package->AddObserver(this);
        package->Create(packageBinFilePath, wingpackage::Content::all);
        package->RemoveObserver(this);
        log->WriteLine("==> " + packageBinFilePath);
        log->WriteLine("Creating package info...");
        std::string xmlInfoFilePath = Path::ChangeExtension(mainWindow->GetPackage()->FilePath(), ".info.xml");
        package->WriteInfoXmlFile(xmlInfoFilePath);
        log->WriteLine("==> " + xmlInfoFilePath);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        log->WriteLine("Making setup programs...");
        BuildLogger buildLogger(log);
        wingstall::wingpackage::MakeSetup(packageBinFilePath, true, &buildLogger);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        std::string compileDebugFilePath;
        std::string debugStdoutFilePath;
        std::string debugStderrFilePath;
        std::string compileReleaseFilePath;
        std::string releaseStdoutFilePath;
        std::string releaseStderrFilePath;
        log->WriteLine("Making compile batches..");
        MakeCompileBatches(packageBinFilePath, compileDebugFilePath, debugStdoutFilePath, debugStderrFilePath, compileReleaseFilePath, releaseStdoutFilePath, releaseStderrFilePath);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        log->WriteLine("Compiling using debug configuration...");
        Compile(compileDebugFilePath, debugStdoutFilePath, debugStderrFilePath, 0, 50);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        log->WriteLine("Compiling using release configuration...");
        Compile(compileReleaseFilePath, releaseStdoutFilePath, releaseStderrFilePath, 50, 100);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        std::string binDirectory = GetFullPath(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "bin"));
        std::string debugSetupFile = GetFullPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "x64"), "Debug"), "setupd.exe"));
        std::string releaseSetupFile = GetFullPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "x64"), "Release"), "setup.exe"));
        log->WriteLine("Copying setup files to '" + binDirectory + "' directory...");

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        Copy(debugSetupFile, binDirectory);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        Copy(releaseSetupFile, binDirectory);

        if (buildCanceled)
        {
            throw wingstall::wingpackage::AbortException();
        }
        mainWindow->EndBuild();
        log->WriteLine("================ Package Build Succeeded ================ ");
    }
    catch (const wingstall::wingpackage::AbortException&)
    {
        mainWindow->EndBuild();
        log->WriteLine("Package build canceled");
    }
    catch (const std::exception& ex)
    {
        mainWindow->EndBuild();
        log->WriteLine("error: " + std::string(ex.what()));
    }
}

void BuildTask::Cancel()
{
    buildCanceled = true;
    package->Interrupt();
    package->SetAction(wingstall::wingpackage::Action::abortAction);
}

void BuildTask::MakeCompileBatches(
    const std::string& packageBinFilePath, 
    std::string& compileDebugFilePath, 
    std::string& debugStdoutFilePath, 
    std::string& debugStderrFilePath, 
    std::string& compileReleaseFilePath,
    std::string& releaseStdoutFilePath,
    std::string& releaseStderrFilePath)
{
    std::string setupFilePath = GetFullPath(Path::Combine(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "program"), "setup.vcxproj"));
    std::string compileDirectory = GetFullPath(Path::Combine(Path::GetDirectoryName(GetFullPath(packageBinFilePath)), "compile"));
    boost::system::error_code ec;
    boost::filesystem::create_directories(compileDirectory, ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + compileDirectory + "': " + PlatformStringToUtf8(ec.message()));
    }

    std::u32string vcvarsAttribute = wingstall::config::ConfigurationDocument()->DocumentElement()->GetAttribute(U"vcVarsFilePath");
    if (vcvarsAttribute.empty())
    {
        throw std::runtime_error("'vcVarsFilePath' attribute not found from the root element of configuration file '" + wingstall::config::ConfigFilePath() + "'");
    }
    std::string vcvars64BatFilePath = GetFullPath(ToUtf8(vcvarsAttribute));

    compileDebugFilePath = Path::Combine(compileDirectory, "compile_debug.bat");
    std::ofstream compileDebugFile(compileDebugFilePath);
    CodeFormatter debugFormatter(compileDebugFile);
    debugFormatter.WriteLine("call \"" + MakeNativePath(vcvars64BatFilePath) + "\"");
    debugStdoutFilePath = Path::Combine(compileDirectory, "debug_stdout.txt");
    debugStderrFilePath = Path::Combine(compileDirectory, "debug_stderr.txt");
    debugFormatter.WriteLine("msbuild -verbosity:minimal -t:Rebuild -p:Configuration=\"Debug\" \"" +
        MakeNativePath(setupFilePath) + "\" > \"" +
        MakeNativePath(debugStdoutFilePath) + "\" 2> \"" +
        MakeNativePath(debugStderrFilePath) + "\"");

    compileReleaseFilePath = Path::Combine(compileDirectory, "compile_release.bat");
    std::ofstream compileReleaseFile(compileReleaseFilePath);
    CodeFormatter releaseFormatter(compileReleaseFile);
    releaseFormatter.WriteLine("call \"" + MakeNativePath(vcvars64BatFilePath) + "\"");
    releaseStdoutFilePath = Path::Combine(compileDirectory, "release_stdout.txt");
    releaseStderrFilePath = Path::Combine(compileDirectory, "release_stderr.txt");
    releaseFormatter.WriteLine("msbuild -verbosity:minimal -t:Rebuild -p:Configuration=\"Release\" \"" +
        MakeNativePath(setupFilePath) + "\" > \"" +
        MakeNativePath(releaseStdoutFilePath) + "\" 2> \"" +
        MakeNativePath(releaseStderrFilePath) + "\"");
}

void BuildTask::Compile(const std::string& compileBatFilePath, const std::string& stdOutFilePath, const std::string& stdErrFilePath, int startPercent, int endPercent)
{
    mainWindow->SetBuildProgressPercent(startPercent);
    std::string command = "cmd /C \"" + MakeNativePath(compileBatFilePath) + "\"";
    Process process(command, Process::Redirections::none);
    process.WaitForExit();
    int exitCode = process.ExitCode();
    if (exitCode != 0)
    {
        throw std::runtime_error("compile command returned exit code " + std::to_string(exitCode));
    }
    if (boost::filesystem::exists(stdOutFilePath))
    {
        std::string stdoutFileStr = ReadFile(stdOutFilePath);
        if (!stdoutFileStr.empty())
        {
            log->WriteLine(stdoutFileStr);
        }
    }
    if (boost::filesystem::exists(stdErrFilePath))
    {
        std::string stderrFileStr = ReadFile(stdErrFilePath);
        if (!stderrFileStr.empty())
        {
            log->WriteLine(stderrFileStr);
        }
    }
    mainWindow->SetBuildProgressPercent(endPercent);
}

void BuildTask::Copy(const std::string& setupFilePath, const std::string& binDirectory)
{
    BuildCopyFileObserver observer(log, this);
    prevPercent = -1;
    std::string dest = Path::Combine(binDirectory, Path::GetFileName(setupFilePath));
    CopyFile(setupFilePath, dest, true, true, true, &observer);
}

void BuildTask::StatusChanged(wingpackage::Package* package)
{
    if (log)
    {
        log->WriteLine(package->GetStatusStr());
        if (package->GetStatus() == wingpackage::Status::failed)
        {
            log->WriteLine(package->GetErrorMessage());
        }
    }
}

void BuildTask::LogError(wingpackage::Package* package, const std::string& error)
{
    if (log)
    {
        log->WriteLine(package->GetErrorMessage());
        log->WriteLine("Package Build Failed");
    }
}

void BuildTask::FileContentPositionChanged(wingpackage::Package* package)
{
    int percent = 0;
    if (package->FileContentPosition() == package->FileContentSize())
    {
        percent = 100;
    }
    else
    {
        percent = static_cast<int>((100.0 * package->FileContentPosition()) / package->FileContentSize());
    }
    if (percent != prevPercent)
    {
        mainWindow->SetBuildProgressPercent(percent);
        prevPercent = percent;
    }
}

void BuildTask::FileCopyProgress(int64_t pos, int64_t size)
{
    int percent = 0;
    if (pos == size)
    {
        percent = 100;
    }
    else
    {
        percent = static_cast<int>((100.0 * pos) / size);
    }
    if (percent != prevPercent)
    {
        mainWindow->SetBuildProgressPercent(percent);
        prevPercent = percent;
    }
}

void RunBuildTask(BuildTask* buildTask)
{
    try
    {
        buildTask->Run();
    }
    catch (...)
    {
    }
}

} } // wingstall::package_editor
