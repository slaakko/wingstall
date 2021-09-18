// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ListView.hpp>
#include <wing/Application.hpp>
#include <wing/ResourceManager.hpp>
#include <wing/ImageList.hpp>
#include <soulng/util/Unicode.hpp>

#undef max
#undef min

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

Color DefaultListViewColumnDividerColor()
{
    return Color(229, 229, 229);
}

Color DefaultListViewItemSelectedColor()
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

Padding DefaultListViewItemColumnPadding()
{
    return Padding(4, 0, 4, 0);
}

Padding DefaultListViewColumnDividerPadding()
{
    return Padding(1, 0, 1, 0);
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
    listViewColumnDividerColor(DefaultListViewColumnDividerColor()),
    listViewItemSelectedColor(DefaultListViewItemSelectedColor()),
    columnHeaderPadding(DefaultListViewColumnHeaderPadding()),
    itemPadding(DefaultListViewItemPadding()),
    itemColumnPadding(DefaultListViewItemColumnPadding()),
    columnDividerPadding(DefaultListViewColumnDividerPadding()),
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
    itemSelectedBrush(createParams.listViewItemSelectedColor),
    columnDividerPen(createParams.listViewColumnDividerColor),
    columnHeaderPadding(createParams.columnHeaderPadding),
    itemPadding(createParams.itemPadding),
    itemColumnPadding(createParams.itemColumnPadding),
    columnDividerPadding(createParams.columnDividerPadding),
    imagePadding(createParams.imagePadding),
    charWidth(0), 
    charHeight(0),
    columnDividerWidth(1),
    ellipsisWidth(0),
    mouseDownItem(nullptr),
    mouseEnterItem(nullptr), 
    selectedItem(nullptr),
    mouseDownColumnDivider(nullptr),
    arrowCursor(LoadStandardCursor(StandardCursorId::arrow)),
    columnSizeCursor(Application::GetResourceManager().GetCursor("column.size.wing.cursor"))
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
    columnDividers.push_back(std::unique_ptr<ListViewColumnDivider>(new ListViewColumnDivider(this, column)));
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

void ListView::SetSelectedItem(ListViewItem* selectedItem_)
{
    if (selectedItem != selectedItem_)
    {
        if (selectedItem)
        {
            selectedItem->ResetSelected();
        }
        selectedItem = selectedItem_;
        if (selectedItem)
        {
            selectedItem->SetSelected();
        }
    }
}

ListViewItem* ListView::ItemAt(const Point& location) const
{
    for (const std::unique_ptr<ListViewItem>& item : items)
    {
        Rect rect(item->Location(), item->GetSize());
        if (rect.Contains(location))
        {
            return item.get();
        }
    }
    return nullptr;
}

ListViewColumnDivider* ListView::ColumnDividerAt(const Point& location) const
{
    for (const std::unique_ptr<ListViewColumnDivider>& columnDivider : columnDividers)
    {
        Rect rect(columnDivider->Location(), columnDivider->GetSize());
        if (rect.Contains(location))
        {
            return columnDivider.get();
        }
    }
    return nullptr;
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
        MeasureItems(args.graphics);
        DrawColumnHeader(args.graphics, origin);
        DrawItems(args.graphics, origin);
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::OnMouseDown(MouseEventArgs& args)
{
    try
    {
        mouseDownItem = nullptr;
        ListViewItem* item = ItemAt(args.location);
        if (item)
        {
            mouseDownItem = item;
        }
        else
        {
            SetSelectedItem(nullptr);
        }
        if (args.buttons == MouseButtons::lbutton)
        {
            ListViewColumnDivider* columnDivider = ColumnDividerAt(args.location);
            if (columnDivider)
            {
                columnDivider->OnLButtonDown(args.location);
                mouseDownColumnDivider = columnDivider;
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::OnMouseUp(MouseEventArgs& args)
{
    try
    {
        ListViewItem* item = ItemAt(args.location);
        if (item == mouseDownItem)
        {
            ListViewItemEventArgs itemArgs(item);
            if (args.buttons == MouseButtons::lbutton)
            {
                itemClick.Fire(itemArgs);
            }
            else if (args.buttons == MouseButtons::rbutton)
            {
                itemRightClick.Fire(itemArgs);
            }
        }
        mouseDownItem = nullptr;
        if (args.buttons == MouseButtons::lbutton)
        {
            if (mouseDownColumnDivider)
            {
                mouseDownColumnDivider->OnLButtonUp();
                mouseDownColumnDivider = nullptr;
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::OnMouseDoubleClick(MouseEventArgs& args) 
{
    try
    {
        ListViewItem* item = ItemAt(args.location);
        if (item)
        {
            ListViewItemEventArgs args(item);
            itemDoubleClick.Fire(args);
        }
        mouseDownItem = nullptr;
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::OnMouseEnter()
{
    try
    {
        mouseEnterItem = nullptr;
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::OnMouseLeave()
{
    try
    {
        if (mouseEnterItem)
        {
            ListViewItemEventArgs leaveItemArgs(mouseEnterItem);
            itemLeave.Fire(leaveItemArgs);
            mouseEnterItem = nullptr;
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::OnMouseMove(MouseEventArgs& args)
{
    try
    {
        if (!mouseEnterItem)
        {
            mouseEnterItem = ItemAt(args.location);
            if (mouseEnterItem)
            {
                ListViewItemEventArgs itemArgs(mouseEnterItem);
                itemEnter.Fire(itemArgs);
            }
        }
        else
        {
            ListViewItem* item = ItemAt(args.location);
            if (item != mouseEnterItem)
            {
                ListViewItemEventArgs leaveItemArgs(mouseEnterItem);
                itemLeave.Fire(leaveItemArgs);
                mouseEnterItem = item;
                if (mouseEnterItem)
                {
                    ListViewItemEventArgs enterItemArgs(mouseEnterItem);
                    itemEnter.Fire(enterItemArgs);
                }
            }
        }
        if (args.buttons == MouseButtons::lbutton)
        {
            if (mouseDownColumnDivider)
            {
                if (mouseDownColumnDivider->HasMouseCapture())
                {
                    mouseDownColumnDivider->OnMouseMove(args.location);
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void ListView::SetCursor()
{
    Point cursorPos = ScreenToClient(GetCursorPos());
    bool cursorSet = false;
    for (const std::unique_ptr<ListViewColumnDivider>& columnDivider : columnDividers)
    {
        Rect r(columnDivider->Location(), columnDivider->GetSize());
        if (r.Contains(cursorPos))
        {
            wing::SetCursor(columnSizeCursor);
            cursorSet = true;
            break;
        }
    }
    if (!cursorSet)
    {
        wing::SetCursor(arrowCursor);
    }
}

void ListView::Measure(Graphics& graphics)
{
    RectF charRect = MeasureString(graphics, "This is test string", GetFont(), PointF(0, 0), stringFormat);
    charHeight = charRect.Height;
    charWidth = charRect.Width;
    SetScrollUnits(ScrollUnits(static_cast<int>(charHeight + 0.5), static_cast<int>(2 * (charWidth + 0.5))));
    RectF ellipsisRect = MeasureString(graphics, "...", GetFont(), PointF(0, 0), stringFormat);
    ellipsisWidth = ellipsisRect.Width;
}

void ListView::MeasureItems(Graphics& graphics)
{
    Point loc(itemPadding.left, charHeight + columnHeaderPadding.Vertical());
    for (const auto& item : items)
    {
        item->SetLocation(loc);
        item->Measure(graphics);
        loc.Y = loc.Y + item->GetSize().Height;
    }
}

void ListView::DrawColumnHeader(Graphics& graphics, PointF& origin)
{
    PointF headerOrigin = origin;
    headerOrigin.X = headerOrigin.X + columnHeaderPadding.left;
    headerOrigin.Y = headerOrigin.Y + columnHeaderPadding.top;
    int n = columns.size();
    for (int i = 0; i < n; ++i)
    {
        ListViewColumn* column = columns[i].get();
        column->Draw(graphics, headerOrigin);
        headerOrigin.X = headerOrigin.X + column->Width() + columnHeaderPadding.Horizontal();
        ListViewColumnDivider* divider = columnDividers[i].get();
        headerOrigin.X = headerOrigin.X + columnDividerPadding.left;
        divider->Draw(graphics, headerOrigin);
        headerOrigin.X = headerOrigin.X + columnDividerWidth + columnDividerPadding.right;
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
        origin.Y = origin.Y + item->GetSize().Height;
    }
}

ListViewColumn::ListViewColumn(ListView* view_, const std::string& name_, int width_) : view(view_), name(name_), width(width_), minWidth(0)
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

ListViewColumnDivider::ListViewColumnDivider(ListView* view_, ListViewColumn* column_) : 
    view(view_), column(column_), location(), startCapturePos(), startColumnWidth(0), hasMouseCapture(false)
{
}

Size ListViewColumnDivider::GetSize() const
{
    return Size(view->ColumnDividerWidth() + 4 * view->ColumnDividerPadding().Horizontal(), view->TextHeight());
}

void ListViewColumnDivider::OnLButtonDown(const Point& pos)
{
    SetCapture(view->Handle());
    startCapturePos = pos;
    startColumnWidth = column->Width();
    hasMouseCapture = true;
}

void ListViewColumnDivider::OnMouseMove(const Point& pos)
{
    column->SetWidth(std::max(column->MinWidth(), startColumnWidth + pos.X - startCapturePos.X));
}

void ListViewColumnDivider::OnLButtonUp()
{
    ReleaseCapture();
    hasMouseCapture = false;
}

void ListViewColumnDivider::Draw(Graphics& graphics, const PointF& origin)
{
    const Pen& pen = view->ColumnDividerPen();
    location = Point(origin.X - GetSize().Width / 2, origin.Y);
    PointF end = origin;
    end.Y += view->TextHeight();
    graphics.DrawLine(&pen, origin, end);
}

ListViewItem::ListViewItem(ListView* view_) : view(view_), flags(ListViewItemFlags::none), imageIndex(-1), disabledImageIndex(-1), data(nullptr)
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

void ListViewItem::SetState(ListViewItemState state)
{
    if (state != State())
    {
        if (state == ListViewItemState::disabled)
        {
            SetFlag(ListViewItemFlags::disabled);
        }
        else
        {
            ResetFlag(ListViewItemFlags::disabled);
        }
        view->Invalidate();
    }
}

void ListViewItem::SetSelected()
{ 
    if (!IsSelected())
    {
        SetFlag(ListViewItemFlags::selected);
        view->Invalidate();
    }
}

void ListViewItem::ResetSelected()
{ 
    if (IsSelected())
    {
        ResetFlag(ListViewItemFlags::selected);
        view->Invalidate();
    }
}

ListViewItemState ListViewItem::State() const
{
    if (GetFlag(ListViewItemFlags::disabled))
    {
        return ListViewItemState::disabled;
    }
    else
    {
        return ListViewItemState::enabled;
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
    if (IsSelected())
    {
        const Brush& selectedBrush = view->GetItemSelectedBrush();
        Rect rect(location, size);
        graphics.FillRectangle(&selectedBrush, rect);
    }
    PointF itemOrigin = origin;
    int imageSpace = 0;
    DrawImage(graphics, itemOrigin, imageSpace);
    for (int index = 0; index < view->ColumnCount(); ++index)
    {
        int imgSpc = 0;
        if (index == 0)
        {
            imgSpc = imageSpace;
        }
        bool clipped = false;
        Gdiplus::Region prevClip;
        if (index < textWidths.size() && textWidths[index] > view->GetColumn(index).Width())
        {
            CheckGraphicsStatus(graphics.GetClip(&prevClip));
            clipped = true;
            SizeF itemSize(view->GetColumn(index).Width() - view->EllipsisWidth() - imgSpc, view->TextHeight());
            RectF clip(itemOrigin, itemSize);
            CheckGraphicsStatus(graphics.SetClip(clip));
        }
        const Brush* brush = &view->GetItemTextBrush();
        if (State() == ListViewItemState::disabled)
        {
            brush = &view->GetDisabledItemTextBrush();
        }
        DrawString(graphics, GetColumnValue(index), view->GetFont(), itemOrigin, *brush);
        itemOrigin.X = itemOrigin.X + view->GetColumn(index).Width();
        if (clipped)
        {
            CheckGraphicsStatus(graphics.SetClip(&prevClip));
            PointF ellipsisOrigin(itemOrigin.X - view->EllipsisWidth() - imgSpc, itemOrigin.Y);
            DrawString(graphics, "...", view->GetFont(), ellipsisOrigin, *brush);
        }
        itemOrigin.X = itemOrigin.X + view->ColumnDividerPadding().Horizontal() + view->ColumnDividerWidth() - imgSpc;
        itemOrigin.X = itemOrigin.X + view->ItemColumnPadding().Horizontal();
    }
}

void ListViewItem::SetLocation(const Point& location_)
{
    location = location_;
}

void ListViewItem::Measure(Graphics& graphics)
{
    float width = 0;
    float height = 0;
    Bitmap* image = nullptr;
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        if (State() == ListViewItemState::enabled)
        {
            image = imageList->GetImage(imageIndex);
        }
        else
        {
            image = imageList->GetImage(disabledImageIndex);
        }
    }
    if (image)
    {
        int imageWidth = image->GetWidth();
        int imageHeight = image->GetHeight();
        Padding padding = view->ImagePadding();
        if (view->ColumnCount() > 0)
        {
            ListViewColumn& firstColumn = view->GetColumn(0);
            firstColumn.SetMinWidth(std::max(firstColumn.MinWidth(), static_cast<int>(imageWidth + padding.Horizontal() + view->EllipsisWidth() + 0.5f)));
        }
        height = imageHeight + padding.Vertical();
    }
    for (int index = 0; index < view->ColumnCount(); ++index)
    {
        width = width + view->GetColumn(index).Width();
        width = width + view->ColumnDividerWidth() + view->ColumnDividerPadding().Horizontal();
        height = std::max(height, view->TextHeight());
        ListViewColumn& column = view->GetColumn(index);
        column.SetMinWidth(std::max(column.MinWidth(), static_cast<int>(view->EllipsisWidth() + 0.5f)));
    }
    width = width + view->ItemPadding().Horizontal();
    size = Size(static_cast<int>(width + 0.5f), static_cast<int>(height + 0.5f));
    textWidths.clear();
    for (const std::string& columnValue : columnValues)
    {
        RectF r = MeasureString(graphics, columnValue, view->GetFont(), PointF(0, 0), view->GetStringFormat());
        textWidths.push_back(r.Width);
    }
}

void ListViewItem::DrawImage(Graphics& graphics, PointF& origin, int& imageSpace)
{
    imageSpace = 0;
    Bitmap* image = nullptr;
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        if (State() == ListViewItemState::enabled)
        {
            image = imageList->GetImage(imageIndex);
        }
        else
        {
            image = imageList->GetImage(disabledImageIndex);
        }
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
        imageSpace = imageWidth + padding.Horizontal();
        origin.X = origin.X + imageSpace;
    }
}

} // wing
