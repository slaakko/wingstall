// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_BUFFER_INCLUDED
#define WING_BUFFER_INCLUDED
#include <wing/Graphics.hpp>

namespace wing {

class Buffer
{
public:
    Buffer(const Size& size_, Graphics& graphics_);
    const Size& GetSize() const { return size; }
    Graphics& BitmapGraphics() { return bmgraphics; }
    Bitmap& GetBitmap() { return bitmap; }
    void Draw(Graphics& graphics);
private:
    Size size;
    Bitmap bitmap;
    Graphics bmgraphics;
};

} // wing

#endif // WING_BUFFER_INCLUDED
