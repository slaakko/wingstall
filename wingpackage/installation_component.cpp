// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/installation_component.hpp>
#include <wingpackage/package.hpp>
#include <wing/Installation.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace wingstall { namespace wingpackage {

using namespace soulng::util;
using namespace wing;

InstallationComponent::InstallationComponent() : Component(NodeKind::installation_component, "installation")
{
}

void InstallationComponent::CreateInstallationInfo()
{
    Package* package = GetPackage();
    if (package)
    {
        std::string softwareKey = SoftwareKey(package->Id());
        try
        {
            RegistryKey key = OpenOrCreateRegistryKeyForSoftwareKey(softwareKey);
            key.SetValue("DisplayName", package->AppName(), RegistryValueKind::regSz);
            key.SetValue("DisplayVersion", package->Version(), RegistryValueKind::regSz);
            if (!package->Publisher().empty())
            {
                key.SetValue("Publisher", package->Publisher(), RegistryValueKind::regSz);
            }
            if (!package->IconFilePath().empty())
            {
                key.SetValue("DisplayIcon", package->ExpandPath(package->IconFilePath()), RegistryValueKind::regSz);
            }
            int majorVersion = package->MajorVersion();
            if (majorVersion != -1)
            {
                key.SetIntegerValue("VersionMajor", majorVersion);
            }
            int minorVersion = package->MinorVersion();
            if (minorVersion != -1)
            {
                key.SetIntegerValue("VersionMinor", minorVersion);
            }
            int binaryVersion = package->BinaryVersion();
            if (binaryVersion != -1)
            {
                key.SetIntegerValue("Version", binaryVersion);
            }
            std::string installLocation = package->TargetRootDir();
            if (!installLocation.empty())
            {
                key.SetValue("InstallLocation", installLocation, RegistryValueKind::regSz);
            }
            std::string uninstallString = package->UninstallString();
            if (!uninstallString.empty())
            {
                key.SetValue("UninstallString", uninstallString, RegistryValueKind::regSz);
            }
            int sizeKB = static_cast<int>(package->FileContentSize() / 1024ll);
            key.SetIntegerValue("EstimatedSize", sizeKB);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("could not create registry installation information for software key '" + softwareKey + "': " + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

void InstallationComponent::RemoveInstallationInfo()
{
    Package* package = GetPackage();
    if (package)
    {
        std::string softwareKey = SoftwareKey(package->Id());
        try
        {
            DeleteRegistryKeyForSoftwareKey(softwareKey);
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not remove software installation key '" + softwareKey + "' from registry: " + ex.what());
        }
    }
    else
    {
        throw std::runtime_error("package not set");
    }
}

std::string InstallationComponent::SoftwareKey(const boost::uuids::uuid& packageId) const
{
    std::string key = "{" + ToUpper(boost::lexical_cast<std::string>(packageId)) + "}";
    return key;
}

} } // namespace wingstall::wingpackage
