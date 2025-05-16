#include <range/v3/all.hpp>
#include <AUI/View/AForEachUI.h>
#include "MainWindow.h"
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/ALabel.h>
#include <AUI/View/AButton.h>
#include <AUI/Platform/APlatform.h>
#include <AUI/View/ADrawableView.h>
#include <AUI/View/AProgressBar.h>
#include <AUI/View/ASlider.h>
#include <AUI/View/ASpacerFixed.h>
#include <AUI/View/AScrollArea.h>
#include <AUI/View/ASpinnerV2.h>
#include <AUI/View/AText.h>
#include <AUI/Audio/IAudioPlayer.h>
#include <range/v3/action/sort.hpp>
#include <AUI/Platform/AMessageBox.h>
#include <AUI/Audio/ABadFormatException.h>
#include "fluent_icons.h"
#include "model/State.h"
#include "metadata.h"
#include "MP3SoundStream.h"

static constexpr auto LOG_TAG = "MainWindow";

using namespace declarative;

static auto timeFormatter(std::chrono::milliseconds position) {
    return "{:%M:%S}"_format(std::chrono::duration_cast<std::chrono::seconds>(position));
}

_<AView> MainWindow::playlistView() {
    if (mState.songs->empty()) {
        return Centered {
            Vertical {
              SpacerFixed { 50_dp },
              Label { "No songs found" } with_style {
                ATextAlign::CENTER,
                FontSize { 14_pt },
              },
              AText::fromString("Add songs to your music folder:") with_style { ATextAlign::CENTER },
              AText::fromString(mState.path) with_style { ATextAlign::CENTER },
              _new<AButton>("Open Music Folder") let {
                      AObject::connect(it->clicked, AObject::GENERIC_OBSERVER, [this] {
                          APlatform::openUrl(mState.path);
                      });
                  },
            },
        };
    }

    return Vertical {
        SpacerFixed { 50_dp },
        AScrollArea::Builder()
            .withContents(
                AUI_DECLARATIVE_FOR(i, *mState.songs, AVerticalLayout) {
                return Label { i->title } with_style { ATextOverflow::ELLIPSIS } let {
                    connect(it->clicked, [this, i] { mState.currentSong = i; });
                };
            })
            .build() with_style {
              ScrollbarAppearance {
                ScrollbarAppearance::ALWAYS,
                ScrollbarAppearance::NEVER,
              },
            },
    };
}

_<AView> MainWindow::playerView() {
    return Centered::Expanding {
        _new<ADrawableView>() with_style {
          Expanding(),
        } let { connect(mState.currentThumbnail, slot(it)::setDrawable); },

        _new<AView>() with_style {
          Expanding(),
          BackgroundSolid { AColor::BLACK.transparentize(0.5f) },
          Backdrop { Backdrop::GaussianBlur { 64_dp } },
        },

        Vertical {
          SpacerFixed { 50_dp },
          Centered {
            Centered {
              _new<ADrawableView>() with_style { Expanding() } let { connect(mState.currentThumbnail, slot(it)::setDrawable); },
            } with_style {
              FixedSize { 128_dp },
              BackgroundSolid { AColor::GRAY },
              BorderRadius { 8_dp },
              BoxShadow { 0, 4_dp, 32_dp, AColor::BLACK.transparentize(0.5f) },
            },
          },

          SpacerFixed { 32_dp },

          _new<ASlider>() let {
              connect(mState.currentPosition.readProjected([this](std::chrono::milliseconds m) -> aui::float_within_0_1 {
                  auto duration = mState.currentDuration->count();
                  if (duration == 0) {
                      return 0;
                  }
                  return float(m.count()) / float(duration);
              }), slot(it)::setValue);
          },
          Horizontal {
            Label {} with_style { ATextAlign::CENTER } let {
                connect(mState.currentPosition.readProjected(timeFormatter), slot(it)::setText);
            },
            SpacerExpanding {},
            Label {} with_style { ATextAlign::CENTER } let {
                connect(mState.currentDuration.readProjected(timeFormatter), slot(it)::setText);
            },
          },

          SpacerFixed { 16_dp },

          Label {} with_style { ATextAlign::CENTER, FontSize(14_pt), ATextOverflow::ELLIPSIS } &
              mState.currentSong.readProjected([](const _<Song>& song) { return song ? song->title : " "; }),
          Label {} with_style { ATextAlign::CENTER },
          Label {} with_style { ATextAlign::CENTER },

          SpacerFixed { 16_dp },

          Centered {
            Horizontal {
              Label { fluent_icons::ic_fluent_previous_48_filled } with_style {
                Font { ":img/FluentSystemIcons-Filled.ttf" },
                FontSize { 32_dp },
              },
              Label { fluent_icons::ic_fluent_play_circle_48_filled } with_style {
                Font { ":img/FluentSystemIcons-Filled.ttf" },
                FontSize { 48_dp },
              } let {connect(it->clicked, me::togglePlay); },
              Label { fluent_icons::ic_fluent_next_48_filled } with_style {
                Font { ":img/FluentSystemIcons-Filled.ttf" },
                FontSize { 32_dp },
              },
            } with_style { LayoutSpacing { 8_dp } },
          },
        } with_style {
          MinSize { 250_dp },
          Padding { 32_dp, 64_dp },
        },
    } with_style {
        BoxShadowInner { 0, 64_dp, 64_dp, AColor::BLACK.transparentize(0.9f) },
    };
}

MainWindow::MainWindow() : AWindow("AUImusic", 400_dp, 600_dp) {
    setExtraStylesheet(AStylesheet {
      {
        t<AView>(),
        TextColor { AColor::WHITE },
      },
      {
        t<AButton>(),
        BackgroundSolid { 0x88000000_argb },
        Border { nullptr },
      } });

    present(_new<ASpinnerV2>() with_style {
      FixedSize { 32_dp, 32_dp },
      BackgroundImage { {}, 0xff3c3c43_argb },
    });

    connect(mState.isPlaying.changed, [this](bool isPlaying) {
        if (mState.currentPlayer == nullptr) {
            return;
        }
        if (isPlaying) {
            mState.currentPlayer->play();
        } else {
            mState.currentPlayer->pause();
        }
    });

    connect(mState.currentSong.changed, [this](const _<Song>& currentSong) {
        mState.isPlaying = false;
        if (mState.currentPlayer != nullptr) {
            mState.currentPlayer = nullptr;
        }
        mState.currentStream = nullptr;
        try {
            auto stream = open(currentSong->location);
            if (auto finiteMedia = _cast<IFiniteMedia>(stream)) {
                mState.currentStream = std::move(finiteMedia);
            }
            mState.currentPlayer = IAudioPlayer::fromStream(std::move(stream));
            mState.isPlaying = true;
        } catch (const AException& e) {
            AMessageBox::show(this, "Failed to play song", e.getMessage());
            ALogger::err(LOG_TAG) << "Failed to play song: " << e;
        }

        if (*currentSong->thumbnail != nullptr) {
            return;
        }
        auto copy = *currentSong;
        mAsync << async mutable {
            metadata::populate(copy);
            ui_thread { **mState.currentSong = copy; };
        };
    });

    loadPlaylist();
}
void MainWindow::loadPlaylist() {
    auto lookIn = mState.path;
    mAsync << async {
        AVector<_<Song>> songs;
        for (const auto& p : lookIn.listDir(AFileListFlags::RECURSIVE | AFileListFlags::REGULAR_FILES)) {
            if (!p.endsWith(".mp3")) {
                // we support mp3 only at the moment.
                // skips desktop.ini on windows.
                continue;
            }
            songs << aui::ptr::manage(new Song {
              .location = p,
              .title = p.filenameWithoutExtension(),
            });
        }

        ranges::sort(songs, [](const auto& a, const auto& b) { return a->title < b->title; });

        ui_thread {
            mState.songs = std::move(songs);
            present(Horizontal::Expanding {
              playlistView() with_style {
                FixedSize { 200_dp, {} },
                BackgroundSolid { AColor::BLACK.transparentize(0.7f) },
              },
              playerView(),
            });
        };

        return songs;
    };
}

void MainWindow::present(_<AView> view) {
    setContents(Centered { std::move(view) } with_style {
      BackgroundSolid { 0x2c2c2c_rgb },
      Padding { 0 },
      Margin { 0 },
    });
}

_<ISoundInputStream> MainWindow::open(const AUrl& url) {
    try {
        return ISoundInputStream::fromUrl(url);
    } catch (const aui::audio::ABadFormatException& e) {
    }

    try {
        return _new<MP3SoundStream>(url.open());
    } catch (const aui::audio::ABadFormatException& e) {
    }

    throw AException("unrecognized format: " + url.full());
}

void MainWindow::togglePlay() {
    mState.isPlaying = !mState.isPlaying;
}
