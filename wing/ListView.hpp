// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_LIST_VIEW_INCLUDED
#define WING_LIST_VIEW_INCLUDED
#include <wing/Control.hpp>

namespace wing {    

class ImageList;

std::string DefaultListViewFontFamilyName();
float DefaultListViewFontSize();
Color DefaultListViewColumnTextColor();
Color DefaultListViewItemTextColor();
Color DefaultListViewSelectedItemBackgroundColor();
Padding DefaultListViewColumnHeaderPadding();
Padding DefaultListViewItemPadding();
Padding DefaultListViewImagePadding();

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
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color listViewColumnTextColor;
    Color listViewItemTextColor;
    Color listViewDisabledItemTextColor;
    Color listViewSelectedItemBackgroundColor;
    Padding columnHeaderPadding;
    Padding itemPadding;
    Padding imagePadding;
};

class ListViewColumn;
class ListViewItem;

enum class ListViewFlags : int
{
    none = 0, measured = 1 << 0
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
    bool Measured() const { return (flags & ListViewFlags::measured) != ListViewFlags::none; }
    void SetMeasured() { flags = flags | ListViewFlags::measured; }
    const Padding& ColumnHeaderPadding() const { return columnHeaderPadding; }
    const Padding& ItemPadding() const { return itemPadding; }
    const Padding& ImagePadding() const { return imagePadding; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void Measure(Graphics& graphics);
private:
    void DrawColumnHeader(Graphics& graphics, PointF& origin);
    void DrawItems(Graphics& graphics, PointF& origin);
    ListViewFlags flags;
    std::vector<std::unique_ptr<ListViewColumn>> columns;
    std::vector<std::unique_ptr<ListViewItem>> items;
    ImageList* imageList;
    SolidBrush columnHeaderTextBrush;
    SolidBrush itemTextBrush;
    SolidBrush disabledItemTextBrush;
    StringFormat stringFormat;
    float charWidth;
    float charHeight;
    Padding columnHeaderPadding;
    Padding itemPadding;
    Padding imagePadding;
};

class WING_API ListViewColumn
{
public:
    ListViewColumn(ListView* view_, const std::string& name_, int width_);
    void Draw(Graphics& graphics, const PointF& origin);
    const std::string& Name() const { return name; }
    int Width() const { return width; }
    void SetWidth(int width_);
private:
    ListView* view;
    std::string name;
    int width;
};

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
    ListViewItemState State() const { return state; }
    void SetState(ListViewItemState state_);
    int ImageIndex() const { return imageIndex; }
    void SetImageIndex(int imageIndex_);
    int DisabledImageIndex() const { return disabledImageIndex; }
    void SetDisabledImageIndex(int disabledImageIndex_);
    void Draw(Graphics& graphics, const PointF& origin);
private:
    void DrawImage(Graphics& graphics, PointF& origin);
    ListView* view;
    ListViewItemState state;
    std::vector<std::string> columnValues;
    int imageIndex;
    int disabledImageIndex;
};

} // wing

#endif // WING_LIST_VIEW_INCLUDED
