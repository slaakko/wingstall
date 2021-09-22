// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_LIST_VIEW_INCLUDED
#define WING_LIST_VIEW_INCLUDED
#include <wing/Control.hpp>
#include <wing/Cursor.hpp>

namespace wing {    

class ImageList;

std::string DefaultListViewFontFamilyName();
float DefaultListViewFontSize();
Color DefaultListViewColumnTextColor();
Color DefaultListViewItemTextColor();
Color DefaultListViewSelectedItemBackgroundColor();
Color DefaultListViewColumnDividerColor();
Color DefaultListViewItemSelectedColor();
Padding DefaultListViewColumnHeaderPadding();
Padding DefaultListViewItemPadding();
Padding DefaultListViewItemColumnPadding();
Padding DefaultListViewColumnDividerPadding();
Padding DefaultListViewImagePadding();

class ListView;
class ListViewItem;

struct WING_API ListViewItemEventArgs
{
    ListViewItemEventArgs(ListView* view_, ListViewItem* item_) : view(view_), item(item_), control(false) {}
    ListView* view;
    ListViewItem* item;
    Point location;
    bool control;
};

using ListViewItemClickEvent = EventWithArgs<ListViewItemEventArgs>;
using ListViewItemDoubleClickEvent = EventWithArgs<ListViewItemEventArgs>;
using ListViewItemEnterEvent = EventWithArgs<ListViewItemEventArgs>;
using ListViewItemLeaveEvent = EventWithArgs<ListViewItemEventArgs>;
using ListViewItemSelectedEvent = EventWithArgs<ListViewItemEventArgs>;

struct WING_API ListViewCreateParams
{
    ListViewCreateParams();
    ListViewCreateParams& Defaults();
    ListViewCreateParams& WindowClassName(const std::string& windowClassName_);
    ListViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ListViewCreateParams& WindowStyle(int windowStyle_);
    ListViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ListViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    ListViewCreateParams& Text(const std::string& text_);
    ListViewCreateParams& Location(const Point& location_);
    ListViewCreateParams& SetSize(Size size_);
    ListViewCreateParams& SetAnchors(Anchors anchors_);
    ListViewCreateParams& SetDock(Dock dock_);
    ListViewCreateParams& AllowMultiselect(bool allow);
    ControlCreateParams controlCreateParams;
    bool allowMultiselect;
    std::string fontFamilyName;
    float fontSize;
    Color listViewColumnTextColor;
    Color listViewItemTextColor;
    Color listViewDisabledItemTextColor;
    Color listViewSelectedItemBackgroundColor;
    Color listViewColumnDividerColor;
    Color listViewItemSelectedColor;
    Padding columnHeaderPadding;
    Padding itemPadding;
    Padding itemColumnPadding;
    Padding columnDividerPadding;
    Padding imagePadding;
};

class ListViewColumn;
class ListViewColumnDivider;

enum class ListViewFlags : int
{
    none = 0, measured = 1 << 0, allowMultiselect = 1 << 1
};

inline ListViewFlags operator|(ListViewFlags left, ListViewFlags right)
{
    return ListViewFlags(int(left) | int(right));
}

inline ListViewFlags operator&(ListViewFlags left, ListViewFlags right)
{
    return ListViewFlags(int(left) & int(right));
}

inline ListViewFlags operator~(ListViewFlags flags)
{
    return ListViewFlags(~int(flags));
}

class WING_API ListView : public Control
{
public:
    ListView(ListViewCreateParams& createParams);
    bool GetListViewFlag(ListViewFlags flag) const { return (flags & flag) != ListViewFlags::none; }
    void SetListViewFlag(ListViewFlags flag) { flags = (flags | flag); }
    void ResetListViewFlag(ListViewFlags flag) { flags = (flags & ~flag); }
    bool AllowMultiselect() const { return GetListViewFlag(ListViewFlags::allowMultiselect); }
    void AddColumn(const std::string& name, int width);
    const ListViewColumn& GetColumn(int columnIndex) const;
    ListViewColumn& GetColumn(int columnIndex);
    int ColumnCount() const { return columns.size(); }
    ListViewItem* AddItem();
    const ListViewItem& GetItem(int itemIndex) const;
    ListViewItem& GetItem(int itemIndex) ;
    int ItemCount() const { return items.size(); }
    ImageList* GetImageList() const { return imageList; }
    void SetImageList(ImageList* imageList_) { imageList = imageList_; }
    const Brush& GetColumnHeaderTextBrush() const { return columnHeaderTextBrush; }
    const Brush& GetItemTextBrush() const { return itemTextBrush; }
    const Brush& GetDisabledItemTextBrush() const { return disabledItemTextBrush; }
    const Brush& GetItemSelectedBrush() const { return itemSelectedBrush; }
    const Pen& ColumnDividerPen() const { return columnDividerPen; }
    bool Measured() const { return (flags & ListViewFlags::measured) != ListViewFlags::none; }
    void SetMeasured() { flags = flags | ListViewFlags::measured; }
    const Padding& ColumnHeaderPadding() const { return columnHeaderPadding; }
    const Padding& ItemPadding() const { return itemPadding; }
    const Padding& ItemColumnPadding() const { return itemColumnPadding; }
    const Padding& ColumnDividerPadding() const { return columnDividerPadding; }
    const Padding& ImagePadding() const { return imagePadding; }
    const StringFormat& GetStringFormat() const { return stringFormat; }
    float TextHeight() const { return charHeight; }
    float ColumnDividerWidth() const { return columnDividerWidth; }
    float EllipsisWidth() const { return ellipsisWidth; }
    ListViewItem* SelectedItem() const { return selectedItem; }
    void SetSelectedItem(ListViewItem* selectedItem_);
    std::vector<ListViewItem*> GetSelectedItems() const;
    void ResetSelectedItems();
    ListViewItem* ItemAt(const Point& location) const;
    ListViewColumnDivider* ColumnDividerAt(const Point& location) const;
    ListViewItemClickEvent& ItemClick() { return itemClick; }
    ListViewItemClickEvent& ItemRightClick() { return itemRightClick; }
    ListViewItemDoubleClickEvent& ItemDoubleClick() { return itemDoubleClick; }
    ListViewItemEnterEvent& ItemEnter() { return itemEnter; }
    ListViewItemLeaveEvent& ItemLeave() { return itemLeave; }
protected:
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseDoubleClick(MouseEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseMove(MouseEventArgs& args) override;
    void SetCursor() override;
    void Measure(Graphics& graphics);
private:
    void MeasureItems(Graphics& graphics, Size& contentSize);
    void DrawColumnHeader(Graphics& graphics, PointF& origin);
    void DrawItems(Graphics& graphics, PointF& origin);
    ListViewFlags flags;
    std::vector<std::unique_ptr<ListViewColumn>> columns;
    std::vector<std::unique_ptr<ListViewColumnDivider>> columnDividers;
    std::vector<std::unique_ptr<ListViewItem>> items;
    ImageList* imageList;
    SolidBrush columnHeaderTextBrush;
    SolidBrush itemTextBrush;
    SolidBrush disabledItemTextBrush;
    SolidBrush itemSelectedBrush;
    Pen columnDividerPen;
    StringFormat stringFormat;
    float charWidth;
    float charHeight;
    float columnDividerWidth;
    float ellipsisWidth;
    Padding columnHeaderPadding;
    Padding itemPadding;
    Padding itemColumnPadding;
    Padding columnDividerPadding;
    Padding imagePadding;
    Cursor& columnSizeCursor;
    Cursor arrowCursor;
    ListViewItem* mouseDownItem;
    ListViewItem* mouseEnterItem;
    ListViewItem* selectedItem;
    ListViewColumnDivider* mouseDownColumnDivider;
    ListViewItemClickEvent itemClick;
    ListViewItemClickEvent itemRightClick;
    ListViewItemDoubleClickEvent itemDoubleClick;
    ListViewItemEnterEvent itemEnter;
    ListViewItemLeaveEvent itemLeave;
};

class WING_API ListViewColumn
{
public:
    ListViewColumn(ListView* view_, const std::string& name_, int width_);
    void Draw(Graphics& graphics, const PointF& origin);
    const std::string& Name() const { return name; }
    int Width() const { return width; }
    void SetWidth(int width_);
    int MinWidth() const { return minWidth; }
    void SetMinWidth(int minWidth_) { minWidth = minWidth_; }
private:
    ListView* view;
    std::string name;
    int width;
    int minWidth;
};

class WING_API ListViewColumnDivider
{
public:
    ListViewColumnDivider(ListView* view_, ListViewColumn* column_);
    bool HasMouseCapture() const { return hasMouseCapture; }
    void OnLButtonDown(const Point& pos);
    void OnMouseMove(const Point& pos);
    void OnLButtonUp();
    void Draw(Graphics& graphics, const PointF& origin);
    Point Location() const { return location; }
    Size GetSize() const;
private:
    ListView* view;
    ListViewColumn* column;
    Point location;
    Point startCapturePos;
    int startColumnWidth;
    bool hasMouseCapture;
};

enum class ListViewItemFlags : int
{
    none = 0, disabled = 1 << 0, selected = 1 << 1
};

inline ListViewItemFlags operator|(ListViewItemFlags left, ListViewItemFlags right)
{
    return ListViewItemFlags(int(left) | int(right));
}

inline ListViewItemFlags operator&(ListViewItemFlags left, ListViewItemFlags right)
{
    return ListViewItemFlags(int(left) & int(right));
}

inline ListViewItemFlags operator~(ListViewItemFlags flags)
{
    return ListViewItemFlags(~int(flags));
}

enum class ListViewItemState : int
{
    enabled, disabled
};

class WING_API ListViewItem
{
public:
    ListViewItem(ListView* view_);
    void SetColumnValue(int columnIndex, const std::string& columnValue);
    std::string GetColumnValue(int columnIndex) const;
    bool GetFlag(ListViewItemFlags flag) const { return (flags & flag) != ListViewItemFlags::none; }
    void SetFlag(ListViewItemFlags flag) { flags = flags | flag; }
    void ResetFlag(ListViewItemFlags flag) { flags = flags & ~flag; }
    ListViewItemState State() const;
    void SetState(ListViewItemState state);
    bool IsSelected() const { return GetFlag(ListViewItemFlags::selected); }
    void SetSelected();
    void ResetSelected();
    int ImageIndex() const { return imageIndex; }
    void SetImageIndex(int imageIndex_);
    int DisabledImageIndex() const { return disabledImageIndex; }
    void SetDisabledImageIndex(int disabledImageIndex_);
    void Draw(Graphics& graphics, const PointF& origin);
    void Measure(Graphics& graphics);
    const Point& Location() const { return location; }
    void SetLocation(const Point& location_);
    const Size& GetSize() const { return size; }
    ListView* View() const { return view; }
    void SetData(void* data_) { data = data_; }
    void* Data() const { return data; }
private:
    void DrawImage(Graphics& graphics, PointF& origin, int& imageSpace);
    ListView* view;
    ListViewItemFlags flags;
    std::vector<std::string> columnValues;
    std::vector<float> textWidths;
    int imageIndex;
    int disabledImageIndex;
    void* data;
    Point location;
    Size size;
};

} // wing

#endif // WING_LIST_VIEW_INCLUDED
