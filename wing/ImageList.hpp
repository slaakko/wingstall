// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_IMAGE_LIST_INCLUDED
#define WING_IMAGE_LIST_INCLUDED
#include <wing/Graphics.hpp>
#include <map>
#include <vector>

namespace wing {

class WING_API ImageList
{
public:
    ImageList();
    void AddImage(const std::string& imageName);
    void AddDisabledImage(const std::string& imageName);
    void AddImage(const std::string& imageName, Bitmap* bitmap);
    int GetImageIndex(const std::string& imageName) const;
    int GetDisabledImageIndex(const std::string& imageName) const;
    Bitmap* GetImage(int imageIndex) const;
private:
    std::map<std::string, int> imageIndexMap;
    std::vector<std::unique_ptr<Bitmap>> images;
};

} // wing

#endif // WING_IMAGE_LIST_INCLUDED
