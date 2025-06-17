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

#include <AUI/Common/AVector.h>
#include <AUI/Util/AImageDrawable.h>
#include "Song.h"
#include "IFiniteMedia.h"

struct State {
    AProperty<AVector<_<Song>>> songs;
    APath path = APath::getDefaultPath(APath::HOME) / "Music";
    AProperty<_<Song>> currentSong;
    _<IAudioPlayer> currentPlayer;
    AProperty<_<IFiniteMedia>> currentStream;
    AProperty<bool> isPlaying = false;

    APropertyPrecomputed<_<IDrawable>> currentThumbnail = [&]() -> _<IDrawable> {
        if (*currentSong == nullptr) {
            return nullptr;
        }
        auto thumbnail = *(*currentSong)->thumbnail;
        if (thumbnail == nullptr) {
            return nullptr;
        }
        return _new<AImageDrawable>(std::move(thumbnail));
    };

    APropertyPrecomputed<std::chrono::milliseconds> currentPosition = [&]() -> std::chrono::milliseconds {
        if (currentStream == nullptr) {
            return std::chrono::milliseconds(0);
        }
        return (*currentStream)->position;
    };

    APropertyPrecomputed<std::chrono::milliseconds> currentDuration = [&]() -> std::chrono::milliseconds {
      if (currentStream == nullptr) {
          return std::chrono::milliseconds(0);
      }
      return (*currentStream)->duration;
    };
};
