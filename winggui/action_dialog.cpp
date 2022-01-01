// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/action_dialog.hpp>
#include <wing/Metrics.hpp>

namespace wingstall { namespace winggui {

std::string GetActionDialogKindStr(ActionDialogKind kind)
{
    switch (kind)
    {
        case ActionDialogKind::installationAction: return "Installation";
        case ActionDialogKind::uninstallationAction: return "Uninstallation";
    }
    return std::string();
}

std::string GetActionStr(ActionDialogKind kind)
{
    switch (kind)
    {
    case ActionDialogKind::installationAction: return "installation";
    case ActionDialogKind::uninstallationAction: return "uninstallation";
    }
    return std::string();
}

ActionDialog::ActionDialog(ActionDialogKind kind) : Window(WindowCreateParams().Text(GetActionDialogKindStr(kind) + " actions").WindowStyle(DialogWindowStyle()).WindowClassName("winggui.ActionDialog").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    abortButton(nullptr), continueButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();

    Point abortLabelLocation(16, 16);
    std::unique_ptr<Label> abortLabelPtr(new Label(LabelCreateParams().Location(abortLabelLocation).
        Text("Abort action: no work is undone, the system is left as it is in the middle of " + GetActionStr(kind) + ".").SetAnchors(Anchors::left | Anchors::top)));
    AddChild(abortLabelPtr.release());

    Point continueLabelLocation(16, 16 + 24);
    std::unique_ptr<Label> continueLabelPtr(new Label(LabelCreateParams().Location(continueLabelLocation).
        Text("Continue action: continue " + GetActionStr(kind) + ".").SetAnchors(Anchors::left | Anchors::top)));
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

    std::unique_ptr<Button> abortButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("Abort").SetAnchors(Anchors::right | Anchors::bottom)));
    abortButton = abortButtonPtr.get();
    abortButton->SetDialogResult(DialogResult::abort);
    AddChild(abortButtonPtr.release());

    continueButton->SetFocus();
}

} } // namespace wingstall::wingpackage
