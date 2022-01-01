// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wingpackage/uninstall_component.hpp>
#include <wingpackage/package.hpp>
#include <wingpackage/uninstall_exe_file.hpp>
#include <wingpackage/uninstall_bin_file.hpp>
#include <wingpackage/directory.hpp>
#include <wing/FileUtil.hpp>
#include <soulng/util/Path.hpp>

namespace wingstall { namespace wingpackage {

using namespace wing;
using namespace soulng::util;

UninstallComponent::UninstallComponent() : Component(NodeKind::uninstall_component, "uninstall")
{
}

void UninstallComponent::Initialize()
{
    UninstallExeFile* uninstallExeFile = new UninstallExeFile();
    AddFile(uninstallExeFile);
    uninstallExeFile->SetInfo();
    UninstallBinFile* uinstallBinFile = new UninstallBinFile();
    AddFile(uinstallBinFile);
}

void UninstallComponent::AddFile(File* file)
{
    file->SetParent(this);
    files.push_back(std::unique_ptr<File>(file));
}

void UninstallComponent::WriteIndex(BinaryStreamWriter& writer)
{
    Component::WriteIndex(writer);
    int32_t numFiles = files.size();
    writer.Write(numFiles);
    for (int32_t i = 0; i < numFiles; ++i)
    {
        File* file = files[i].get();
        wingpackage::WriteIndex(file, writer);
    }
}

void UninstallComponent::ReadIndex(BinaryStreamReader& reader)
{
    Component::ReadIndex(reader);
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    int32_t numFiles = reader.ReadInt();
    for (int32_t i = 0; i < numFiles; ++i)
    {
        File* file = BeginReadFile(reader);
        AddFile(file);
        file->ReadIndex(reader);
    }
}

void UninstallComponent::WriteData(BinaryStreamWriter& writer)
{
    for (const auto& file : files)
    {
        file->WriteData(writer);
    }
}

void UninstallComponent::ReadData(BinaryStreamReader& reader)
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    for (const auto& file : files)
    {
        file->ReadData(reader);
    }
}

void UninstallComponent::Uninstall()
{
    Package* package = GetPackage();
    if (package)
    {
        package->SetComponent(this);
        package->CheckInterrupted();
    }
    for (const auto& file : files)
    {
        file->Uninstall();
    }
    ScheduleRootDirToBeRemoved();
}

void UninstallComponent::ScheduleRootDirToBeRemoved()
{
    Package* package = GetPackage();
    if (package)
    {
        try
        {
            RemoveOnReboot(MakeNativePath(package->TargetRootDir()));
        }
        catch (const std::exception& ex)
        {
            package->LogError("could not schedule directory '" + package->TargetRootDir() + "' to be removed on reboot: " + ex.what());
        }
    }
}

} } // namespace wingstall::wingpackage
