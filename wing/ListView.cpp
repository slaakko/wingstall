// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ListView.hpp>
#include <wing/ImageList.hpp>
#include <soulng/util/Unicode.hpp>

namespace wing {

using namespace soulng::unicode;

std::string DefaultListViewFontFamilyName()
{
    return "Segoe UI";
}

float DefaultListViewFontSize()
{
    return 9.0f;
}

Color DefaultListViewColumnTextColor()
{
    return Color(76, 96, 122);
}

Color DefaultListViewItemTextColor()
{
    return Color::Black;
}

Color DefaultListViewDisabledItemTextColor()
{
    return Color(201, 201, 201);
}

Color DefaultListViewSelectedItemBackgroundColor()
{
    return Color(204, 232, 255);
}

Padding DefaultListViewColumnHeaderPadding()
{
    return Padding(4, 0, 4, 4);
}

Padding DefaultListViewItemPadding()
{
    return Padding(4, 0, 4, 0);
}

Padding DefaultListViewImagePadding()
{
    return Padding(2, 2, 2, 2);
}

ListViewCreateParams::ListViewCreateParams() : 
    controlCreateParams(),
    fontFamilyName(DefaultListViewFontFamilyName()),
    fontSize(DefaultListViewFontSize()),
    listViewColumnTextColor(DefaultListViewColumnTextColor()),
    listViewItemTextColor(DefaultListViewItemTextColor()),
    listViewDisabledItemTextColor(DefaultListViewDisabledItemTextColor()),
    listViewSelectedItemBackgroundColor(DefaultListViewSelectedItemBackgroundColor()),
    columnHeaderPadding(DefaultListViewColumnHeaderPadding()),
    itemPadding(DefaultListViewItemPadding()),
    imagePadding(DefaultListViewImagePadding())
{
    controlCreateParams.WindowClassName("wing.ListView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.WindowClassStyle(DoubleClickWindowClassStyle());
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    controlCreateParams.BackgroundColor(Color::White);
}

ListViewCreateParams& ListViewCreateParams::Defaults()
{
    return *this;
}

ListViewCreateParams& ListViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ListViewCreateParams& ListViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ListView::ListView(ListViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    flags(ListViewFlags::none), 
    imageList(nullptr), 
    columnHeaderTextBrush(createParams.listViewColumnTextColor),
    itemTextBrush(createParams.listViewItemTextColor),
    disabledItemTextBrush(createParams.listViewDisabledItemTextColor),
    columnHeaderPadding(createParams.columnHeaderPadding),
    itemPadding(createParams.itemPadding),
    imagePadding(createParams.imagePadding),
    charWidth(0), 
    charHeight(0)
{
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    std::u16string familyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetContentSize(Size(400, 200));
}

void ListView::AddColumn(const std::string& name, int width)
{
    ListViewColumn* column = new ListViewColumn(this, name, width);
    columns.push_back(std::unique_ptr<ListViewColumn>(column));
}

const ListViewColumn& ListView::GetColumn(int columnIndex) const
{
    if (columnIndex < 0 || columnIndex >= ColumnCount())
    {
        throw std::runtime_error("invalid column index");
    }
    return *columns[columnIndex];
}

ListViewColumn& ListView::GetColumn(int columnIndex)
{
    if (columnIndex < 0 || columnIndex >= ColumnCount())
    {
        throw std::runtime_error("invalid column index");
    }
    return *columns[columnIndex];
}

ListViewItem* ListView::AddItem()
{
    ListViewItem* item = new ListViewItem(this);
    items.push_back(std::unique_ptr<ListViewItem>(item));
    return item;
}

const ListViewItem& ListView::GetItem(int itemIndex) const
{
    if (itemIndex < 0 || itemIndex >= ItemCount())
    {
        throw std::runtime_error("invalid item index");
    }
    return *items[itemIndex];
}

ListViewItem& ListView::GetItem(int itemIndex)
{
    if (itemIndex < 0 || itemIndex >= ItemCount())
    {
        throw std::runtime_error("invalid item index");
    }
    return *items[itemIndex];
}

void ListView::OnPaint(PaintEventArgs& args)
{
    try
    {
        if (!Measured())
        {
            SetMeasured();
            Measure(args.graphics);
        }
        args.graphics.Clear(BackgroundColor());
        PointF origin;
        DrawColumnHeader(args.graphics, origin);
        DrawItems(args.graphics, origin);
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::Measure(Graphics& graphics)
{
    RectF charRect = MeasureString(graphics, "This is test string", GetFont(), PointF(0, 0), stringFormat);
    charHeight = charRect.Height;
    charWidth = charRect.Width;
    SetScrollUnits(ScrollUnits(static_cast<int>(charHeight + 0.5), static_cast<int>(2 * (charWidth + 0.5))));
}

void ListView::DrawColumnHeader(Graphics& graphics, PointF& origin)
{
    PointF headerOrigin = origin;
    headerOrigin.X = headerOrigin.X + columnHeaderPadding.left;
    headerOrigin.Y = headerOrigin.Y + columnHeaderPadding.top;
    for (const auto& column : columns)
    {
        column->Draw(graphics, headerOrigin);
        headerOrigin.X = headerOrigin.X + columnHeaderPadding.Horizontal() + column->Width();
    }
    origin.Y = origin.Y + charHeight + columnHeaderPadding.Vertical();
}

void ListView::DrawItems(Graphics& graphics, PointF& origin)
{
    for (const auto& item : items)
    {
        PointF itemOrigin = origin;
        itemOrigin.Y = itemOrigin.Y + itemPadding.top;
        itemOrigin.X = itemOrigin.X + itemPadding.left;
        item->Draw(graphics, itemOrigin);
        origin.Y = origin.Y + charHeight + itemPadding.Vertical();
    }
}

ListViewColumn::ListViewColumn(ListView* view_, const std::string& name_, int width_) : view(view_), name(name_), width(width_)
{
}

void ListViewColumn::SetWidth(int width_)
{
    if (width != width_)
    {
        width = width_;
        view->Invalidate();
    }
}

void ListViewColumn::Draw(Graphics& graphics, const PointF& origin)
{
    DrawString(graphics, name, view->GetFont(), origin, view->GetColumnHeaderTextBrush());
}

ListViewItem::ListViewItem(ListView* view_) : view(view_), state(ListViewItemState::enabled), imageIndex(-1), disabledImageIndex(-1)
{
}

void ListViewItem::SetColumnValue(int columnIndex, const std::string& columnValue)
{
    if (columnIndex < 0)
    {
        throw std::runtime_error("invalid column index");
    }
    while (columnIndex >= columnValues.size())
    {
        columnValues.push_back(std::string());
    }
    columnValues[columnIndex] = columnValue;
}

std::string ListViewItem::GetColumnValue(int columnIndex) const
{
    if (columnIndex < 0)
    {
        throw std::runtime_error("invalid column index");
    }
    if (columnIndex >= columnValues.size())
    {
        return std::string();
    }
    return columnValues[columnIndex];
}

void ListViewItem::SetState(ListViewItemState state_)
{
    if (state != state_)
    {
        state = state_;
        view->Invalidate();
    }
}

void ListViewItem::SetImageIndex(int imageIndex_)
{
    imageIndex = imageIndex_;
}

void ListViewItem::SetDisabledImageIndex(int disabledImageIndex_)
{
    disabledImageIndex = disabledImageIndex_;
}

void ListViewItem::Draw(Graphics& graphics, const PointF& origin)
{
    PointF itemOrigin = origin;
    for (int index = 0; index < view->ColumnCount(); ++index)
    {
        DrawImage(graphics, itemOrigin);
        if (state == ListViewItemState::enabled)
        {
            DrawString(graphics, GetColumnValue(index), view->GetFont(), itemOrigin, view->GetItemTextBrush());
        }
        else
        {
            DrawString(graphics, GetColumnValue(index), view->GetFont(), itemOrigin, view->GetDisabledItemTextBrush());
        }
        itemOrigin.X = itemOrigin.X + view->ItemPadding().Horizontal() + view->GetColumn(index).Width();
    }
}

void ListViewItem::DrawImage(Graphics& graphics, PointF& origin)
{
    Bitmap* image = nullptr;
    ImageList* imageList = view->GetImageList();
    if (state == ListViewItemState::enabled)
    {
        image = imageList->GetImage(imageIndex);
    }
    else
    {
        image = imageList->GetImage(disabledImageIndex);
    }
    if (image)
    {
        int imageWidth = image->GetWidth();
        int imageHeight = image->GetHeight();
        Padding padding = view->ImagePadding();
        PointF imageLoc = origin;
        imageLoc.X = imageLoc.X + padding.left;
        imageLoc.Y = imageLoc.Y + padding.top;
        RectF r(imageLoc, SizeF(imageWidth + padding.Horizontal(), imageHeight + padding.Vertical()));
        Gdiplus::ImageAttributes attributes;
        attributes.SetColorKey(DefaultBitmapTransparentColor(), DefaultBitmapTransparentColor());
        CheckGraphicsStatus(graphics.DrawImage(image, r, 0, 0, imageWidth + padding.Horizontal(), imageHeight + padding.Vertical(), Unit::UnitPixel, &attributes));
        origin.X = origin.X + imageWidth + padding.Horizontal();
    }
}

} // wing
