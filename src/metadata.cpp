//
// Created by alex2772 on 5/5/25.
//

#include <taglib/fileref.h>

#include "metadata.h"
#include <AUI/Image/AImage.h>

void metadata::populate(Song& song) {
#if AUI_PLATFORM_WIN
    TagLib::FileRef f(aui::win32::toWchar(song.location));
#else
    TagLib::FileRef f(song.location.toStdString().c_str());
#endif
    auto picture = f.complexProperties("PICTURE");
    for (const auto& map : picture) {
        for (const auto&[k, v] : map) {
            if (k == "data") {
                bool ok = false;
                auto vector = v.toByteVector(&ok);
                if (!ok) {
                    break;
                }

                song.thumbnail = AImage::fromBuffer(AByteBufferView(vector.data(), vector.size()));
            }
        }
    }
}
