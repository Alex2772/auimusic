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

#include "MP3SoundStream.h"

#include <dr_mp3.h>
#include <AUI/IO/ISeekableInputStream.h>

static size_t read(void* pUserData, void* pBufferOut, size_t bytesToRead) {
    auto stream = reinterpret_cast<IInputStream*>(pUserData);
    return stream->read((char*)pBufferOut, bytesToRead);
}

static drmp3_bool32 seek(void* pUserData, int offset, drmp3_seek_origin origin) {
    auto stream = reinterpret_cast<IInputStream*>(pUserData);
    if (auto seekable = dynamic_cast<ISeekableInputStream*>(stream)) {
        seekable->seek(offset, [&] {
            switch (origin) {
                case drmp3_seek_origin_start:
                    return ASeekDir::BEGIN;
                case drmp3_seek_origin_current:
                    return ASeekDir::CURRENT;
                case drmp3_seek_origin_end:
                    return ASeekDir::END;
            }
        }());
        return true;
    }
    return false;
}

static drmp3_bool32 tell(void* pUserData, drmp3_int64* pCursor) {
    auto stream = reinterpret_cast<IInputStream*>(pUserData);
    if (auto seekable = dynamic_cast<ISeekableInputStream*>(stream)) {
        *pCursor = seekable->tell();
        return true;
    }
    return false;
}

MP3SoundStream::MP3SoundStream(aui::non_null<_unique<IInputStream>> stream) : mStream(std::move(stream.value)) {
    if (!drmp3_init(&mMP3, ::read, ::seek, ::tell, nullptr, mStream.get(), nullptr)) {
        throw AException("Failed to initialize MP3 decoder");
    }
    if (mMP3.totalPCMFrameCount + 1 == 0) {
        if (!drmp3_get_mp3_and_pcm_frame_count(&mMP3, nullptr, &mMP3.totalPCMFrameCount)) {
            return;
        }
    }
    duration = std::chrono::milliseconds(mMP3.totalPCMFrameCount * 1000 / mMP3.sampleRate);
}

MP3SoundStream::~MP3SoundStream() {
    drmp3_uninit(&mMP3);
}

AAudioFormat MP3SoundStream::info() {
    return AAudioFormat {
        .channelCount = static_cast<AChannelFormat>(mMP3.channels),
        .sampleRate = static_cast<unsigned int>(mMP3.sampleRate),
        .sampleFormat = ASampleFormat::F32,
    };
}

size_t MP3SoundStream::read(char *dst, size_t size) {
    position = std::chrono::milliseconds(mMP3.currentPCMFrame * 1000 / mMP3.sampleRate);
    return drmp3_read_pcm_frames_f32(&mMP3, size / sizeof(float) / mMP3.channels, (float*)dst) * sizeof(float) * mMP3.channels;
}


void MP3SoundStream::seek(std::chrono::milliseconds offset) {
    drmp3_seek_to_pcm_frame(&mMP3, offset.count() * mMP3.sampleRate / 1000);
}
