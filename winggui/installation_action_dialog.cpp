// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/installation_action_dialog.hpp>
#include <wing/Metrics.hpp>

namespace wingstall { namespace winggui {

InstallationActionDialog::InstallationActionDialog() : Window(WindowCreateParams().Text("Installation actions").WindowStyle(DialogWindowStyle()).WindowClassName("winggui.InstallationActionDialog").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    abortButton(nullptr), rollbackButton(nullptr), continueButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();

    Point abortLabelLocation(16, 16);
    std::unique_ptr<Label> abortLabelPtr(new Label(LabelCreateParams().Location(abortLabelLocation).
        Text("Abort action: no work is undone, the system is left as it is in the middle of installation.").SetAnchors(Anchors::left | Anchors::top)));
    AddChild(abortLabelPtr.release());

    Point rollbackLabelLocation(16, 16 + 24);
    std::unique_ptr<Label> rollbackLabelPtr(new Label(LabelCreateParams().Location(rollbackLabelLocation).
        Text("Rollback action: changes made to the system are undone.").SetAnchors(Anchors::left | Anchors::top)));
    AddChild(rollbackLabelPtr.release());

    Point continueLabelLocation(16, 16 + 24 + 24);
    std::unique_ptr<Label> continueLabelPtr(new Label(LabelCreateParams().Location(continueLabelLocation).
        Text("Continue action: continue installation.").SetAnchors(Anchors::left | Anchors::top)));
    AddChild(continueLabelPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    std::unique_ptr<Button> continueButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Continue").SetAnchors(Anchors::right | Anchors::bottom)));
    continueButton = continueButtonPtr.get();
    continueButton->SetDialogResult(DialogResult::ok);
    continueButton->SetDefault();
    SetDefaultButton(continueButton);
    AddChild(continueButtonPtr.release());
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    std::unique_ptr<Button> rollbackButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Rollback").SetAnchors(Anchors::right | Anchors::bottom)));
    rollbackButton = rollbackButtonPtr.get();
    rollbackButton->SetDialogResult(DialogResult::cancel);
    AddChild(rollbackButtonPtr.release());
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    std::unique_ptr<Button> abortButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Abort").SetAnchors(Anchors::right | Anchors::bottom)));
    abortButton = abortButtonPtr.get();
    abortButton->SetDialogResult(DialogResult::abort);
    AddChild(abortButtonPtr.release());

    continueButton->SetFocus();
}

} } // namespace wingstall::wingpackage
