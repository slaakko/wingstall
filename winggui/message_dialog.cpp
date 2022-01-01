// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/message_dialog.hpp>
#include <wing/Metrics.hpp>

namespace wingstall { namespace winggui {

std::string GetMessageWindowCaptionText(MessageKind messageKind)
{
    switch (messageKind)
    {
        case MessageKind::info: return "Information";
        case MessageKind::error: return "Error";
    }
    return std::string();
}

MessageDialog::MessageDialog(MessageKind messageKind, const std::string& messageText) : Window(WindowCreateParams().Text(GetMessageWindowCaptionText(messageKind)).WindowStyle(DialogWindowStyle()).
    WindowClassName("winggui.MessageDialog").WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    Location(DefaultLocation()).SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(160), ScreenMetrics::Get().MMToVerticalPixels(60)))),
    okButton(nullptr)
{
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();

    Point labelLocation(16, 16);
    std::unique_ptr<Label> labelPtr(new Label(LabelCreateParams().Text(messageText).Location(labelLocation).SetSize(
        Size(ScreenMetrics::Get().MMToHorizontalPixels(100), ScreenMetrics::Get().MMToVerticalPixels(40))).AutoSize(false).SetAnchors(Anchors::top | Anchors::left)));
    AddChild(labelPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("OK").SetAnchors(Anchors::right | Anchors::bottom)));
    okButton = okButtonPtr.get();
    okButton->SetDialogResult(DialogResult::ok);
    okButton->SetDefault();
    SetDefaultButton(okButton);
    AddChild(okButtonPtr.release());
    okButton->SetFocus();
}

} } // namespace wingstall::wingui
