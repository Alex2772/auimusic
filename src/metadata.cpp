//
// Created by alex2772 on 5/5/25.
//

#include <dr_mp3.h>
#include <id3v2lib.h>
#include "metadata.h"
#include <AUI/Logging/ALogger.h>
#include <AUI/Image/AImage.h>

void metadata::populate(Song& song) {
    auto tag = aui::ptr::make_unique_with_deleter(ID3v2_read_tag(song.location.toStdString().c_str()), ID3v2_Tag_free);
    if (!tag) {
        return;
    }
    if (auto frame = ID3v2_Tag_get_album_cover_frame(tag.get())) {
        song.thumbnail = AImage::fromBuffer(AByteBufferView(frame->data->data, frame->data->picture_size));
    }
}
