// AUImusic, a music player based on AUI Framework
// Copyright (C) 2025 Alex2772
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0

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
