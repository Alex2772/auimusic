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

#pragma once

#include <AUI/Audio/ISoundInputStream.h>
#include <AUI/Util/APimpl.h>
#include <dr_mp3.h>
#include "IFiniteMedia.h"

class MP3SoundStream: public ISoundInputStream, public IFiniteMedia {
public:
    MP3SoundStream(aui::non_null<_unique<IInputStream>> stream);
    ~MP3SoundStream() override;

    AAudioFormat info();

    size_t read(char* dst, size_t size);

    void seek(std::chrono::milliseconds offset) override;

private:
    std::unique_ptr<IInputStream> mStream;
    drmp3 mMP3;

};
