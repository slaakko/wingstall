#include <winggui/install_window.hpp>
#include <wingpackage/info.hpp>
#include <wing/InitDone.hpp>
#include <wing/Environment.hpp>
#include <wing/Shell.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>

using namespace cmajor::wing;
using namespace wingstall::winggui;
using namespace wingstall::wingpackage;

struct InitDone
{
    InitDone(HINSTANCE instance)
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::wing::Init(instance);
    }
    ~InitDone()
    {
        cmajor::wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    InitDone initDone(instance);
    try
    {
        SetInfoItem(InfoItemKind::appName, new StringItem("Cmajor"));
        SetInfoItem(InfoItemKind::appVersion, new StringItem("4.1.0"));
        SetInfoItem(InfoItemKind::installDirName, new StringItem("cmajor"));
        SetInfoItem(InfoItemKind::defaultContainingDirPath, new StringItem("C:/"));
        //SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(Compression::none)));
        SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(Compression::deflate)));
        //SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(Compression::bzip2)));
        SetInfoItem(InfoItemKind::dataSource, new IntegerItem(static_cast<int64_t>(DataSource::file)));
        //std::string packageFilePath = "C:/work/cmajorm/setup/plain/cmajor.bin";
        std::string packageFilePath = "C:/work/cmajorm/setup/deflate/cmajor.bin";
        //std::string packageFilePath = "C:/work/cmajorm/setup/bzip2/cmajor.bin";
        SetInfoItem(InfoItemKind::packageFilePath, new StringItem(packageFilePath));
        //SetInfoItem(InfoItemKind::uncompressedPackageSize, new IntegerItem(boost::filesystem::file_size(packageFilePath)));
        SetInfoItem(InfoItemKind::uncompressedPackageSize, new IntegerItem(1368392867)); // 1368392867
        SetInfoItem(InfoItemKind::compressedPackageSize, new IntegerItem(boost::filesystem::file_size(packageFilePath)));
        Package package;
        InstallWindow installWindow;
        installWindow.SetPackage(&package);
        Application::Run(installWindow);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    return 0;
}
