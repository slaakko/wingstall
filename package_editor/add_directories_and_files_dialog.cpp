// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/add_directories_and_files_dialog.hpp>
#include <package_editor/main_window.hpp>
#include <wing/Label.hpp>
#include <wing/Metrics.hpp>
#include <wing/Panel.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/Button.hpp>
#include <wing/ScrollableControl.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem/directory.hpp>
#include <algorithm>

namespace wingstall { namespace package_editor {

using namespace soulng::util;
using namespace soulng::unicode;

Color DefaultAddDirectoriesAndFilesDialogListViewBorderColor()
{
    return Color(204, 206, 219);
}

void GetDirectoriesAndFiles(const std::string& dirPath, std::vector<std::u32string>& directories, std::vector<std::u32string>& files)
{
    boost::filesystem::path nativeDirPath = MakeNativeBoostPath(dirPath);
    boost::system::error_code ec;
    boost::filesystem::directory_iterator it(nativeDirPath, ec);
    if (ec)
    {
        throw std::runtime_error("cannot iterate directory '" + dirPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    while (it != boost::filesystem::directory_iterator())
    {
        if (it->status().type() == boost::filesystem::file_type::regular_file)
        {
            std::wstring str = it->path().native();
            std::u16string s((const char16_t*)str.c_str());
            std::u32string fileName = ToUtf32(Path::GetFileName(GetFullPath(ToUtf8(s))));
            files.push_back(fileName);
        }
        else if (it->status().type() == boost::filesystem::file_type::directory_file)
        {
            if (!it->path().filename_is_dot() && !it->path().filename_is_dot_dot())
            {
                std::wstring str = it->path().native();
                std::u16string s((const char16_t*)str.c_str());
                std::u32string dirName = ToUtf32(Path::GetFileName(GetFullPath(ToUtf8(s))));
                directories.push_back(dirName);
            }
        }
        ++it;
    }
    std::sort(files.begin(), files.end());
    std::sort(directories.begin(), directories.end());
}

std::vector<std::u32string> Filter(const std::vector<std::u32string>& names, const std::vector<std::u32string>& exclude)
{
    std::vector<std::u32string> result;
    for (const std::u32string& name : names)
    {
        if (!std::binary_search(exclude.cbegin(), exclude.cend(), name))
        {
            result.push_back(name);
        }
    }
    return result;
}

std::vector<std::u32string> Merge(const std::vector<std::u32string>& left, const std::vector<std::u32string>& right)
{
    std::vector<std::u32string> result;
    std::merge(left.begin(), left.end(), right.begin(), right.end(), std::back_inserter(result));
    return result;
}

AddDirectoriesAndFilesDialog::AddDirectoriesAndFilesDialog(wingstall::package_editor::Component* component_) : 
    Window(WindowCreateParams().Text("Add Directories and Files").SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(100)))),
    component(component_)
{
    if (component->GetPackage()->GetProperties()->SourceRootDir().empty())
    {
        throw std::runtime_error("Package | Properties | source root directory is empty");
    }

    SetCaretDisabled();

    MainWindow* mainWindow = component->GetMainWindow();
    imageList = mainWindow->GetImageList();
    Icon& icon = Application::GetResourceManager().GetIcon("package.icon");
    SetIcon(icon);
    SetSmallIcon(icon);

    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultTextBoxSize = ScreenMetrics::Get().DefaultTextBoxSize();
    Size defaultComboBoxSize = ScreenMetrics::Get().DefaultComboBoxSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    std::unique_ptr<Panel> panelPtr(new Panel(ControlCreateParams().SetSize(Size(0, defaultButtonSize.Height + 2 * defaultControlSpacing.Height)).SetDock(Dock::bottom)));
    Panel* panel = panelPtr.get();
    AddChild(panelPtr.release());

    std::unique_ptr<ListView> listViewPtr(new ListView(ListViewCreateParams().AllowMultiselect(true)));
    listView = listViewPtr.get();
    listView->SetFlag(ControlFlags::scrollSubject);
    listView->SetDoubleBuffered();
    listView->SetImageList(mainWindow->GetImageList());
    listView->AddColumn("Name", 400);
    borderColor = DefaultAddDirectoriesAndFilesDialogListViewBorderColor();
    std::unique_ptr<PaddedControl> paddedListViewPtr(new PaddedControl(PaddedControlCreateParams(listViewPtr.release()).Defaults()));
    std::unique_ptr<ScrollableControl> scrollableListViewPtr(new ScrollableControl(ScrollableControlCreateParams(paddedListViewPtr.release()).Defaults()));
    std::unique_ptr<BorderedControl> borderedListViewPtr(new BorderedControl(BorderedControlCreateParams(scrollableListViewPtr.release()).NormalSingleBorderColor(borderColor).SetDock(Dock::fill)));
    AddChild(borderedListViewPtr.release());

    Size panelSize = panel->GetSize();

    Point labelLoc(24, 8);
    std::unique_ptr<Label> sourceRootDirLabelPtr(new Label(LabelCreateParams().Location(labelLoc).Text(GetFullPath(component->GetPackage()->GetProperties()->SourceRootDir()))));
    panel->AddChild(sourceRootDirLabelPtr.release());

    int x = panelSize.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = panelSize.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    Point cancelButtonLoc(x, y);
    std::unique_ptr<Button> cancelButtonPtr(new Button(ControlCreateParams().Location(cancelButtonLoc).SetSize(defaultButtonSize).Text("Cancel").SetAnchors(static_cast<Anchors>(Anchors::bottom | Anchors::right))));
    Button* cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(DialogResult::cancel);
    panel->AddChild(cancelButtonPtr.release());

    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    Point okButtonLoc(x, y);
    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(okButtonLoc).SetSize(defaultButtonSize).Text("OK").SetAnchors(static_cast<Anchors>(Anchors::bottom | Anchors::right))));
    Button* okButton = okButtonPtr.get();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    panel->AddChild(okButtonPtr.release());

    AddDirectoriesAndFiles();
    AddListViewEventHandlers();
}

void AddDirectoriesAndFilesDialog::GetSelectedDirectoriesAndFiles(std::vector<std::u32string>& selectedDirectories, std::vector<std::u32string>& selectedFiles) const
{
    std::vector<ListViewItem*> selectedItems = listView->GetSelectedItems();
    for (ListViewItem* item : selectedItems)
    {
        DialogListViewItemData* itemData = static_cast<DialogListViewItemData*>(item->Data());
        if (itemData->kind == DialogListViewItemData::Kind::directory)
        {
            selectedDirectories.push_back(directoryNames[itemData->index]);
        }
        else if (itemData->kind == DialogListViewItemData::Kind::file)
        {
            selectedFiles.push_back(fileNames[itemData->index]);
        }
    }
}

void AddDirectoriesAndFilesDialog::AddDirectoriesAndFiles()
{
    Node* parent = component->Parent();
    if (parent && parent->Kind() == NodeKind::components)
    {
        Components* components = static_cast<Components*>(parent);
        if (components)
        {
            Package* package = component->GetPackage();
            if (package)
            {
                GetDirectoriesAndFiles(package->GetProperties()->SourceRootDir(), directoryNames, fileNames);
                directoryNames = Filter(directoryNames, component->DirectoryNames());
                fileNames = Filter(fileNames, component->FileNames());
                int nd = directoryNames.size();
                for (int i = 0; i < nd; ++i)
                {
                    const std::u32string& directoryName = directoryNames[i];
                    std::string dirName = ToUtf8(directoryName);
                    wingstall::package_editor::Component* dirComponent = components->GetDirectoryComponent(directoryName);
                    if (dirComponent && dirComponent != component)
                    {
                        dirName.append(" (").append(dirComponent->Name()).append(")");
                    }
                    ListViewItem* item = listView->AddItem();
                    item->SetImageIndex(imageList->GetImageIndex("folder.closed.bitmap"));
                    item->SetColumnValue(0, dirName);
                    DialogListViewItemData* itemData = new DialogListViewItemData(DialogListViewItemData::Kind::directory, i);
                    data.push_back(std::unique_ptr<DialogListViewItemData>(itemData));
                    item->SetData(itemData);
                }
                int nf = fileNames.size();
                for (int i = 0; i < nf; ++i)
                {
                    const std::u32string& fileName = fileNames[i];
                    std::string fName = ToUtf8(fileName);
                    wingstall::package_editor::Component* fileComponent = components->GetFileComponent(fileName);
                    if (fileComponent && fileComponent != component)
                    {
                        fName.append(" (").append(fileComponent->Name()).append(")");
                    }
                    ListViewItem* item = listView->AddItem();
                    item->SetImageIndex(imageList->GetImageIndex("file.bitmap"));
                    item->SetColumnValue(0, fName);
                    DialogListViewItemData* itemData = new DialogListViewItemData(DialogListViewItemData::Kind::file, i);
                    data.push_back(std::unique_ptr<DialogListViewItemData>(itemData));
                    item->SetData(itemData);
                }
            }
        }
    }
}

void AddDirectoriesAndFilesDialog::AddListViewEventHandlers()
{
    listView->ItemClick().AddHandler(this, &AddDirectoriesAndFilesDialog::ListViewItemClick);
    listView->ItemDoubleClick().AddHandler(this, &AddDirectoriesAndFilesDialog::ListViewItemDoubleClick);
}
 
void AddDirectoriesAndFilesDialog::ListViewItemClick(ListViewItemEventArgs& args)
{
    if (args.item && args.view)
    {
        if (args.control)
        {
            if (args.item->IsSelected())
            {
                args.item->ResetSelected();
            }
            else
            {
                args.item->SetSelected();
            }
        }
        else
        {
            args.view->SetSelectedItem(args.item);
        }
    }
}

void AddDirectoriesAndFilesDialog::ListViewItemDoubleClick(ListViewItemEventArgs& args)
{
    if (args.item && args.view)
    {
        args.view->SetSelectedItem(args.item);
    }
    SetDialogResult(DialogResult::ok);
}

} } // wingstall::package_editor
