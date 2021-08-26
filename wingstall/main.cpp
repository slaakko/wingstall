// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/package.hpp>
#include <wingpackage/path_matcher.hpp>
#include <wingstall/make_setup.hpp>
#include <wing/InitDone.hpp>
#include <wing/Environment.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        wing::Init(nullptr);
    }
    ~InitDone()
    {
        wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;
using namespace soulng::unicode;
using namespace wingstall::wingpackage;

enum class Command
{
    none, createPackage, installPackage, makeSetup, installPackageFromVector, setCompression, setContent
};

std::string WingstallVersionStr()
{
    return "1.0.0";
}

void PrintHelp()
{
    std::cout << "wingstall installer generator version " << WingstallVersionStr() << std::endl;
    std::cout << "usage: wingstall OPTIONS" << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "--verbose (-v)" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--help (-h)" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--create-package (-c) PACKAGE.xml" << std::endl;
    std::cout << "  Create binary package PACKAGE.bin, package info file PACKAGE.info.xml and package index PACKAGE.index.xml from package description file PACKAGE.xml." << std::endl;
    std::cout << "--make-setup (-m) PACKAGE.bin" << std::endl;
    std::cout << "  Create Visual C++ setup program from PACKAGE.bin and package info file PACKAGE.info.xml." << std::endl;
}

class PackageFileContentPositionObserver : public PackageObserver
{
public:
    enum class Kind
    {
        read, write
    };
    PackageFileContentPositionObserver(Kind kind_, bool verbose_);
    void StatusChanged(Package* package) override;
    void LogError(Package* package, const std::string& error) override;
    void FileContentPositionChanged(Package* package) override;
private:
    Kind kind;
    bool verbose;
    bool firstFileContent;
    bool inFileContent;
    int numBackspaces;
    int prevPercent;
};

PackageFileContentPositionObserver::PackageFileContentPositionObserver(Kind kind_, bool verbose_) : 
    kind(kind_), verbose(verbose_), numBackspaces(0), prevPercent(-1), firstFileContent(true), inFileContent(false)
{
}

void PackageFileContentPositionObserver::StatusChanged(Package* package)
{
    if (verbose)
    {
        if (inFileContent)
        {
            inFileContent = false;
            std::cout << std::endl;
        }
        std::cout << package->GetStatusStr() << std::endl;
        if (package->GetStatus() == Status::failed)
        {
            std::cout << package->GetErrorMessage() << std::endl;
        }
    }
}

void PackageFileContentPositionObserver::LogError(Package* package, const std::string& error)
{
    std::cout << package->GetErrorMessage() << std::endl;
}

void PackageFileContentPositionObserver::FileContentPositionChanged(Package* package)
{
    if (verbose)
    {
        if (firstFileContent)
        {
            firstFileContent = false;
            if (kind == Kind::write)
            {
                std::cout << "writing files ";
            }
            else if (kind == Kind::read)
            {
                std::cout << "reading files ";
            }
        }
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
            WritePercent(std::cout, percent, numBackspaces);
            prevPercent = percent;
        }
        inFileContent = true;
    }
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        Command command = Command::none;
        bool verbose = false;
        std::vector<std::string> packagesToCreate;
        std::vector<std::string> packagesToInstall;
        std::vector<std::string> packagesToInstallFromVec;
        std::vector<std::string> setupsToCreate;
        Content content = Content::all;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--create-package")
                {
                    command = Command::createPackage;
                }
                else if (arg == "--install-package")
                {
                    command = Command::installPackage;
                }
                else if (arg == "--install-package-from-vector")
                {
                    command = Command::installPackageFromVector;
                }
                else if (arg == "--make-setup")
                {
                    command = Command::makeSetup;
                }
                else if (arg == "--compression")
                {
                    command = Command::setCompression;
                }
                else if (arg == "--content")
                {
                    command = Command::setContent;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v':
                        {
                            verbose = true;
                            break;
                        }
                        case 'h':
                        {
                            PrintHelp();
                            return 1;
                        }
                        case 'c':
                        {
                            command = Command::createPackage;
                            break;
                        }
                        case 'i':
                        {
                            command = Command::installPackage;
                            break;
                        }
                        case 'e':
                        {
                            command = Command::installPackageFromVector;
                            break;
                        }
                        case 'm':
                        {
                            command = Command::makeSetup;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                switch (command)
                {
                    case Command::createPackage:
                    {
                        packagesToCreate.push_back(GetFullPath(arg));
                        break;
                    }
                    case Command::installPackage:
                    {
                        packagesToInstall.push_back(GetFullPath(arg));
                        break;
                    }
                    case Command::installPackageFromVector:
                    {
                        packagesToInstallFromVec.push_back(GetFullPath(arg));
                        break;
                    }
                    case Command::makeSetup:
                    {
                        setupsToCreate.push_back(GetFullPath(arg));
                        break;
                    }
                    case Command::setContent:
                    {
                        if (arg == "preinstall")
                        {
                            content = Content::preinstall;
                        }
                        else if (arg == "index")
                        {
                            content = Content::index;
                        }
                        else if (arg == "data")
                        {
                            content = Content::data;
                        }
                        else if (arg == "all")
                        {
                            content = Content::all;
                        }
                        else
                        {
                            throw std::runtime_error("unknown content type '" + arg + "'");
                        }
                        break;
                    }
                    case Command::none:
                    {
                        throw std::runtime_error("command argument not set");
                    }
                }
            }
        }
        for (const std::string& packageXmlFilePath : packagesToCreate)
        {
            if (verbose)
            {
                std::cout << "creating package '" << packageXmlFilePath + "'..." << std::endl;
            }
            std::unique_ptr<sngxml::dom::Document> packageDoc = sngxml::dom::ReadDocument(packageXmlFilePath);
            PathMatcher pathMatcher(packageXmlFilePath);
            std::unique_ptr<Package> package(new Package(pathMatcher, packageDoc.get()));
            std::string xmlIndexFilePath = Path::ChangeExtension(packageXmlFilePath, ".index.xml");
            package->WriteIndexToXmlFile(xmlIndexFilePath);
            if (verbose)
            {
                std::cout << "==> " << xmlIndexFilePath << std::endl;
            }
            std::string packageBinFilePath = Path::ChangeExtension(packageXmlFilePath, ".bin");
            {
                PackageFileContentPositionObserver observer(PackageFileContentPositionObserver::Kind::write, verbose);
                package->AddObserver(&observer);
                package->Create(packageBinFilePath, content);
                package->RemoveObserver(&observer);
            }
            if (verbose)
            {
                std::cout << "==> " << packageBinFilePath << std::endl;
            }
            std::string xmlInfoFilePath = Path::ChangeExtension(packageXmlFilePath, ".info.xml");
            package->WriteInfoXmlFile(xmlInfoFilePath);
            if (verbose)
            {
                std::cout << "==> " << xmlInfoFilePath << std::endl;
            }
        }
        for (const std::string& packageBinFilePath : packagesToInstall)
        {
            if (verbose)
            {
                std::cout << "installing package '" << packageBinFilePath << "'..." << std::endl;
            }
            std::unique_ptr<Package> package(new Package());
            {
                PackageFileContentPositionObserver observer(PackageFileContentPositionObserver::Kind::read, verbose);
                package->AddObserver(&observer);
                package->Install(DataSource::file, packageBinFilePath, nullptr, 0, content);
                package->RemoveObserver(&observer);
            }
            std::string xmlIndexFilePath = Path::ChangeExtension(packageBinFilePath, ".read.index.xml");
            package->WriteIndexToXmlFile(xmlIndexFilePath);
            if (verbose)
            {
                std::cout << "==> " << xmlIndexFilePath << std::endl;
            }
            if (verbose)
            {
                std::cout << "package '" << packageBinFilePath << "' installed to directory '" << package->TargetRootDir() << "'" << std::endl;
            }
        }
        for (const std::string& packageBinFilePath : packagesToInstallFromVec)
        {
            if (verbose)
            {
                std::cout << "installing package '" << packageBinFilePath << "'..." << std::endl;
            }
            std::unique_ptr<Package> package(new Package());
            std::vector<uint8_t> vec;
            FileStream fileStream(packageBinFilePath, OpenMode::read | OpenMode::binary);
            BufferedStream bufferedStream(fileStream);
            BinaryStreamReader reader(bufferedStream);
            int64_t n = boost::filesystem::file_size(packageBinFilePath);
            for (int64_t i = 0; i < n; ++i)
            {
                uint8_t x = reader.ReadByte();
                vec.push_back(x);
            }
            package->Install(DataSource::memory, std::string(), vec.data(), vec.size(), content);
            std::string xmlIndexFilePath = Path::ChangeExtension(packageBinFilePath, ".read.index.xml");
            package->WriteIndexToXmlFile(xmlIndexFilePath);
            if (verbose)
            {
                std::cout << "==> " << xmlIndexFilePath << std::endl;
            }
            if (verbose)
            {
                std::cout << "package '" << packageBinFilePath << "' installed to directory '" << package->TargetRootDir() << "'" << std::endl;
            }
        }
        for (const std::string& packageBinFilePath : setupsToCreate)
        {
            if (verbose)
            {
                std::cout << "creating setup from package '" << packageBinFilePath << "'..." << std::endl;
            }
            if (verbose)
            {
                wingstall::MakeSetup(packageBinFilePath, verbose);
            }
            if (verbose)
            {
                std::cout << "setup for package '" << packageBinFilePath << "' created" << std::endl;
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}