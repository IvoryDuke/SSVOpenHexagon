// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#include "SSVOpenHexagon/Global/Config.hpp"
#include "SSVOpenHexagon/Utils/String.hpp"
#include "SSVOpenHexagon/Utils/Concat.hpp"
#include "SSVOpenHexagon/SSVUtilsJson/SSVUtilsJson.hpp"
#include "SSVOpenHexagon/Global/UtilsJson.hpp"
#include "SSVOpenHexagon/Utils/Casts.hpp"
#include "SSVOpenHexagon/Core/Joystick.hpp"

#include <SSVStart/Utils/Input.hpp>
#include <SSVStart/Input/Input.hpp>
#include <SSVStart/GameSystem/GameWindow.hpp>

#include <iostream>
#include <fstream>

#define X_LINKEDVALUES_BINDS_JOYSTICK                         \
    X(joystickSelect, unsigned int, "j_select")               \
    X(joystickExit, unsigned int, "j_exit")                   \
    X(joystickFocus, unsigned int, "j_focus")                 \
    X(joystickSwap, unsigned int, "j_swap")                   \
    X(joystickForceRestart, unsigned int, "j_force_restart")  \
    X(joystickRestart, unsigned int, "j_restart")             \
    X(joystickReplay, unsigned int, "j_replay")               \
    X(joystickScreenshot, unsigned int, "j_screenshot")       \
    X(joystickNextPack, unsigned int, "j_next")               \
    X(joystickPreviousPack, unsigned int, "j_previous")       \
    X(joystickAddToFavorites, unsigned int, "j_add_favorite") \
    X(joystickFavoritesMenu, unsigned int, "j_favorite_menu")

#define X_LINKEDVALUES_BINDS_TRIGGERS                               \
    X(triggerRotateCCW, ssvs::Input::Trigger, "t_rotate_ccw")       \
    X(triggerRotateCW, ssvs::Input::Trigger, "t_rotate_cw")         \
    X(triggerFocus, ssvs::Input::Trigger, "t_focus")                \
    X(triggerSelect, ssvs::Input::Trigger, "t_select")              \
    X(triggerExit, ssvs::Input::Trigger, "t_exit")                  \
    X(triggerForceRestart, ssvs::Input::Trigger, "t_force_restart") \
    X(triggerRestart, ssvs::Input::Trigger, "t_restart")            \
    X(triggerReplay, ssvs::Input::Trigger, "t_replay")              \
    X(triggerScreenshot, ssvs::Input::Trigger, "t_screenshot")      \
    X(triggerSwap, ssvs::Input::Trigger, "t_swap")                  \
    X(triggerUp, ssvs::Input::Trigger, "t_up")                      \
    X(triggerDown, ssvs::Input::Trigger, "t_down")                  \
    X(triggerNextPack, ssvs::Input::Trigger, "t_next")              \
    X(triggerPreviousPack, ssvs::Input::Trigger, "t_previous")      \
    X(triggerLuaConsole, ssvs::Input::Trigger, "t_lua_console")     \
    X(triggerPause, ssvs::Input::Trigger, "t_pause")

#define X_LINKEDVALUES_BINDS      \
    X_LINKEDVALUES_BINDS_JOYSTICK \
    X_LINKEDVALUES_BINDS_TRIGGERS

#define X_LINKEDVALUES                                                     \
    X(online, bool, "online")                                              \
    X(official, bool, "official")                                          \
    X(noRotation, bool, "no_rotation")                                     \
    X(noBackground, bool, "no_background")                                 \
    X(noSound, bool, "no_sound")                                           \
    X(noMusic, bool, "no_music")                                           \
    X(blackAndWhite, bool, "black_and_white")                              \
    X(pulseEnabled, bool, "pulse_enabled")                                 \
    X(_3DEnabled, bool, "3D_enabled")                                      \
    X(_3DMultiplier, float, "3D_multiplier")                               \
    X(_3DMaxDepth, unsigned int, "3D_max_depth")                           \
    X(invincible, bool, "invincible")                                      \
    X(autoRestart, bool, "auto_restart")                                   \
    X(soundVolume, float, "sound_volume")                                  \
    X(musicVolume, float, "music_volume")                                  \
    X(flashEnabled, bool, "flash_enabled")                                 \
    X(zoomFactor, float, "zoom_factor")                                    \
    X(pixelMultiplier, int, "pixel_multiplier")                            \
    X(playerSpeed, float, "player_speed")                                  \
    X(playerFocusSpeed, float, "player_focus_speed")                       \
    X(playerSize, float, "player_size")                                    \
    X(limitFPS, bool, "limit_fps")                                         \
    X(vsync, bool, "vsync")                                                \
    X(autoZoomFactor, bool, "auto_zoom_factor")                            \
    X(fullscreen, bool, "fullscreen")                                      \
    X(windowedAutoResolution, bool, "windowed_auto_resolution")            \
    X(fullscreenAutoResolution, bool, "fullscreen_auto_resolution")        \
    X(fullscreenWidth, unsigned int, "fullscreen_width")                   \
    X(fullscreenHeight, unsigned int, "fullscreen_height")                 \
    X(windowedWidth, unsigned int, "windowed_width")                       \
    X(windowedHeight, unsigned int, "windowed_height")                     \
    X(showMessages, bool, "show_messages")                                 \
    X(debug, bool, "debug")                                                \
    X(beatPulse, bool, "beatpulse_enabled")                                \
    X(showTrackedVariables, bool, "show_tracked_variables")                \
    X(musicSpeedDMSync, bool, "music_speed_dm_sync")                       \
    X(maxFPS, unsigned int, "max_fps")                                     \
    X(antialiasingLevel, unsigned int, "antialiasing_level")               \
    X(showFPS, bool, "show_fps")                                           \
    X(serverLocal, bool, "server_local")                                   \
    X(serverVerbose, bool, "server_verbose")                               \
    X(mouseVisible, bool, "mouse_visible")                                 \
    X(musicSpeedMult, float, "music_speed_mult")                           \
    X(drawTextOutlines, bool, "draw_text_outlines")                        \
    X(darkenUnevenBackgroundChunk, bool, "darken_uneven_background_chunk") \
    X(rotateToStart, bool, "rotate_to_start")                              \
    X(joystickDeadzone, float, "joystick_deadzone")                        \
    X(textPadding, float, "text_padding")                                  \
    X(textScaling, float, "text_scaling")                                  \
    X(timescale, float, "timescale")                                       \
    X(showKeyIcons, bool, "show_key_icons")                                \
    X(keyIconsScale, float, "key_icons_scale")                             \
    X(firstTimePlaying, bool, "first_time_playing")                        \
    X(saveLocalBestReplayToFile, bool, "save_local_best_replay_to_file")   \
    X(showLevelInfo, bool, "show_level_info")                              \
    X(showTimer, bool, "show_timer")                                       \
    X(showStatusText, bool, "show_status_text")                            \
    X_LINKEDVALUES_BINDS

namespace hg::Config
{

[[nodiscard]] static ssvuj::Obj& root() noexcept
{
    static ssvuj::Obj res = [] {
        if(ssvufs::Path{"config.json"}.exists<ssvufs::Type::File>())
        {
            ssvu::lo("hg::Config::root()")
                << "User-defined `config.json` file found\n";

            return ssvuj::getFromFile("config.json");
        }

        if(ssvufs::Path{"default_config.json"}.exists<ssvufs::Type::File>())
        {
            ssvu::lo("hg::Config::root()")
                << "User `config.json` file not found, looking for "
                   "default\n";

            ssvu::lo("hg::Config::root()")
                << "Default `default_config.json` file found\n";

            return ssvuj::getFromFile("default_config.json");
        }

        ssvu::lo("hg::Config::root()")
            << "FATAL ERROR: No suitable config file found\n";

        std::abort();
    }();

    return res;
}

#define X(name, type, key)                                 \
    [[nodiscard]] static auto& name() noexcept             \
    {                                                      \
        static auto res = ::ssvuj::LinkedValue<type>(key); \
        return res;                                        \
    }
X_LINKEDVALUES
#undef X

static void fixupMissingTriggers()
{
    const auto doIt = [](ssvs::Input::Trigger& trig) {
        auto& combos = trig.getCombos();

        if(!combos.empty())
        {
            return;
        }

        combos.resize(4);

        for(ssvs::Input::Combo& c : combos)
        {
            c.clearBind(); // mark as unbound
        }
    };

#define X(name, type, key) doIt(name());
    X_LINKEDVALUES_BINDS_TRIGGERS
#undef X
}

static void syncAllFromObj()
{
#define X(name, type, key) name().syncFrom(root());
    X_LINKEDVALUES
#undef X

    fixupMissingTriggers();
}

static void syncAllToObj()
{
#define X(name, type, key) name().syncTo(root());
    X_LINKEDVALUES
#undef X
}

static void resetAllFromObj()
{
#define X(name, type, key) \
    name().syncFrom(ssvuj::getFromFile("default_config.json"));
    X_LINKEDVALUES
#undef X
}

static void resetBindsFromObj()
{
#define X(name, type, key) \
    name().syncFrom(ssvuj::getFromFile("default_config.json"));
    X_LINKEDVALUES_BINDS
#undef X
}

#undef X_LINKEDVALUES
#undef X_LINKEDVALUES_BINDS
#undef X_LINKEDVALUES_BINDS_TRIGGERS
#undef X_LINKEDVALUES_BINDS_JOYSTICK

float sizeX{1500}, sizeY{1500};
constexpr float spawnDistance{1600};
std::string uneligibilityReason;

void applyAutoWindowedResolution()
{
    auto d(sf::VideoMode::getDesktopMode());
    windowedWidth() = d.width;
    windowedHeight() = d.height;
}

void applyAutoFullscreenResolution()
{
    auto d(sf::VideoMode::getDesktopMode());
    fullscreenWidth() = d.width;
    fullscreenHeight() = d.height;
}

void loadConfig(const std::vector<std::string>& mOverridesIds)
{
    ssvu::lo("::loadConfig") << "loading config\n";

    for(const ssvufs::Path& p :
        ssvufs::getScan<ssvufs::Mode::Single, ssvufs::Type::File,
            ssvufs::Pick::ByExt>("ConfigOverrides/", ".json"))
    {
        if(ssvu::contains(mOverridesIds, p.getFileNameNoExtensions()))
        {
            const auto overrideRoot(ssvuj::getFromFile(p));
            for(auto itr(std::begin(overrideRoot));
                itr != std::end(overrideRoot); ++itr)
            {
                root()[ssvuj::getKey(itr)] = *itr;
            }
        }
    }

    syncAllFromObj();

    if(getWindowedAutoResolution())
    {
        applyAutoWindowedResolution();
    }

    if(getFullscreenAutoResolution())
    {
        applyAutoFullscreenResolution();
    }

    recalculateSizes();
}

void resetConfigToDefaults()
{
    ssvu::lo("::resetConfigToDefaults") << "resetting configs\n";

    if(!ssvufs::Path{"default_config.json"}.exists<ssvufs::Type::File>())
    {
        ssvu::lo("hg::Config::resetConfigToDefaults()")
            << "`default_config.json` file not found, config reset "
               "aborted\n";
        return;
    }

    resetAllFromObj();

    if(getWindowedAutoResolution())
    {
        applyAutoWindowedResolution();
    }

    if(getFullscreenAutoResolution())
    {
        applyAutoFullscreenResolution();
    }

    recalculateSizes();
}

void resetBindsToDefaults()
{
    ssvu::lo("::resetBindsToDefaults") << "resetting binds to defaults\n";

    if(!ssvufs::Path{"default_config.json"}.exists<ssvufs::Type::File>())
    {
        ssvu::lo("hg::Config::resetBindsToDefaults()")
            << "`default_config.json` file not found, config reset "
               "aborted\n";
        return;
    }

    resetBindsFromObj();
}

void saveConfig()
{
    ssvu::lo("::saveConfig") << "saving config\n";
    syncAllToObj();
    ssvuj::writeToFile(root(), "config.json");
}

bool isEligibleForScore()
{
    if(!getOfficial())
    {
        uneligibilityReason = "official mode off";
        return false;
    }

    if(getDebug())
    {
        uneligibilityReason = "debug mode on";
        return false;
    }

    if(!getAutoZoomFactor())
    {
        uneligibilityReason = "modified zoom factor";
        return false;
    }

    if(getPlayerSpeed() != 9.45f)
    {
        uneligibilityReason = "player speed modified";
        return false;
    }

    if(getPlayerFocusSpeed() != 4.625f)
    {
        uneligibilityReason = "player focus speed modified";
        return false;
    }

    if(getPlayerSize() != 7.3f)
    {
        uneligibilityReason = "player size modified";
        return false;
    }

    if(getInvincible())
    {
        uneligibilityReason = "invincibility on";
        return false;
    }

    if(getNoRotation())
    {
        uneligibilityReason = "rotation off";
        return false;
    }

    return true;
}

void recalculateSizes()
{
    sizeX = sizeY = std::max(getWidth(), getHeight()) * 1.3f;
    if(!getAutoZoomFactor())
    {
        return;
    }

    const float factorX(1024.f / ssvu::toFloat(getWidth()));
    const float factorY(768.f / ssvu::toFloat(getHeight()));
    zoomFactor() = std::max(factorX, factorY);
}

void setFullscreen(ssvs::GameWindow& mWindow, bool mFullscreen)
{
    fullscreen() = mFullscreen;

    mWindow.setSize(getWidth(), getHeight());
    mWindow.setFullscreen(getFullscreen());

    recalculateSizes();
}

void refreshWindowSize(unsigned int mWidth, unsigned int mHeight)
{
    windowedWidth() = mWidth;
    windowedHeight() = mHeight;
}

void setCurrentResolution(
    ssvs::GameWindow& mWindow, unsigned int mWidth, unsigned int mHeight)
{
    if(fullscreen())
    {
        fullscreenAutoResolution() = false;
        fullscreenWidth() = mWidth;
        fullscreenHeight() = mHeight;
    }
    else
    {
        windowedAutoResolution() = false;
        windowedWidth() = mWidth;
        windowedHeight() = mHeight;
    }

    mWindow.setSize(getWidth(), getHeight());
    mWindow.setFullscreen(getFullscreen());

    recalculateSizes();
}

void setCurrentResolutionAuto(ssvs::GameWindow& mWindow)
{
    if(fullscreen())
    {
        fullscreenAutoResolution() = true;
        applyAutoFullscreenResolution();
    }
    else
    {
        windowedAutoResolution() = true;
        applyAutoWindowedResolution();
    }

    mWindow.setSize(getWidth(), getHeight());
    mWindow.setFullscreen(getFullscreen());
    recalculateSizes();
}

void setVsync(ssvs::GameWindow& mWindow, bool mValue)
{
    vsync() = mValue;
    mWindow.setVsync(vsync());
}

void setLimitFPS(ssvs::GameWindow& mWindow, bool mValue)
{
    limitFPS() = mValue;
    mWindow.setFPSLimited(mValue);
}

void setMaxFPS(ssvs::GameWindow& mWindow, unsigned int mValue)
{
    maxFPS() = mValue;
    mWindow.setMaxFPS(mValue);
}

void setAntialiasingLevel(ssvs::GameWindow& mWindow, unsigned int mValue)
{
    antialiasingLevel() = mValue;
    mWindow.setAntialiasingLevel(mValue);
}

void setOnline(bool mOnline)
{
    online() = mOnline;
}

void setOfficial(bool mOfficial)
{
    official() = mOfficial;
}

void setDebug(bool mDebug)
{
    debug() = mDebug;
}

void setNoRotation(bool mNoRotation)
{
    noRotation() = mNoRotation;
}

void setNoBackground(bool mNoBackground)
{
    noBackground() = mNoBackground;
}

void setBlackAndWhite(bool mBlackAndWhite)
{
    blackAndWhite() = mBlackAndWhite;
}

void setNoSound(bool mNoSound)
{
    noSound() = mNoSound;
}

void setNoMusic(bool mNoMusic)
{
    noMusic() = mNoMusic;
}

void setPulse(bool mPulse)
{
    pulseEnabled() = mPulse;
}

void set3D(bool m3D)
{
    _3DEnabled() = m3D;
}

void setInvincible(bool mInvincible)
{
    invincible() = mInvincible;
}

void setAutoRestart(bool mAutoRestart)
{
    autoRestart() = mAutoRestart;
}

void setSoundVolume(float mVolume)
{
    soundVolume() = mVolume;
}

void setMusicVolume(float mVolume)
{
    musicVolume() = mVolume;
}

void setFlash(bool mFlash)
{
    flashEnabled() = mFlash;
}

void setMusicSpeedDMSync(bool mValue)
{
    musicSpeedDMSync() = mValue;
}

void setShowFPS(bool mValue)
{
    showFPS() = mValue;
}

void setServerLocal(bool mValue)
{
    serverLocal() = mValue;
}

void setServerVerbose(bool mValue)
{
    serverVerbose() = mValue;
}

void setMouseVisible(bool mValue)
{
    mouseVisible() = mValue;
}

void setMusicSpeedMult(float mValue)
{
    musicSpeedMult() = mValue;
}

void setDrawTextOutlines(bool mX)
{
    drawTextOutlines() = mX;
}

void setDarkenUnevenBackgroundChunk(bool mX)
{
    darkenUnevenBackgroundChunk() = mX;
}

void setRotateToStart(bool mX)
{
    rotateToStart() = mX;
}

void setJoystickDeadzone(float mX)
{
    joystickDeadzone() = mX;
}

void setTextPadding(float mX)
{
    textPadding() = mX;
}

void setTextScaling(float mX)
{
    textScaling() = mX;
}

void setTimescale(float mX)
{
    timescale() = mX;
}

void setShowKeyIcons(bool mX)
{
    showKeyIcons() = mX;
}

void setKeyIconsScale(float mX)
{
    keyIconsScale() = mX;
}

void setFirstTimePlaying(bool mX)
{
    firstTimePlaying() = mX;
}

void setSaveLocalBestReplayToFile(bool mX)
{
    saveLocalBestReplayToFile() = mX;
}

void setShowLevelInfo(bool mX)
{
    showLevelInfo() = mX;
}

void setShowTimer(bool mX)
{
    showTimer() = mX;
}

void setShowStatusText(bool mX)
{
    showStatusText() = mX;
}

[[nodiscard]] bool getOnline()
{
    return online();
}

[[nodiscard]] bool getOfficial()
{
    return official();
}

[[nodiscard]] const std::string& getUneligibilityReason()
{
    return uneligibilityReason;
}

[[nodiscard]] float getSizeX()
{
    return sizeX;
}

[[nodiscard]] float getSizeY()
{
    return sizeY;
}

[[nodiscard]] float getSpawnDistance()
{
    return spawnDistance;
}

[[nodiscard]] float getZoomFactor()
{
    return zoomFactor();
}

[[nodiscard]] int getPixelMultiplier()
{
    return pixelMultiplier();
}

[[nodiscard]] float getPlayerSpeed()
{
    return getOfficial() ? 9.45f : playerSpeed();
}

[[nodiscard]] float getPlayerFocusSpeed()
{
    return getOfficial() ? 4.625f : playerFocusSpeed();
}

[[nodiscard]] float getPlayerSize()
{
    return getOfficial() ? 7.3f : playerSize();
}

[[nodiscard]] bool getNoRotation()
{
    return getOfficial() ? false : noRotation();
}

[[nodiscard]] bool getNoBackground()
{
    return getOfficial() ? false : noBackground();
}

[[nodiscard]] bool getBlackAndWhite()
{
    return getOfficial() ? false : blackAndWhite();
}

[[nodiscard]] bool getNoSound()
{
    return noSound();
}

[[nodiscard]] bool getNoMusic()
{
    return noMusic();
}

[[nodiscard]] float getSoundVolume()
{
    return soundVolume();
}

[[nodiscard]] float getMusicVolume()
{
    return musicVolume();
}

[[nodiscard]] bool getLimitFPS()
{
    return limitFPS();
}

[[nodiscard]] bool getVsync()
{
    return vsync();
}

[[nodiscard]] bool getAutoZoomFactor()
{
    return getOfficial() ? true : autoZoomFactor();
}

[[nodiscard]] bool getFullscreen()
{
    return fullscreen();
}

[[nodiscard]] const std::string& getVersionString()
{
    static std::string result{Utils::concat(
        GAME_VERSION.major, '.', GAME_VERSION.minor, '.', GAME_VERSION.micro)};

    return result;
}

[[nodiscard]] bool getWindowedAutoResolution()
{
    return windowedAutoResolution();
}

[[nodiscard]] bool getFullscreenAutoResolution()
{
    return fullscreenAutoResolution();
}

[[nodiscard]] unsigned int getFullscreenWidth()
{
    return fullscreenWidth();
}

[[nodiscard]] unsigned int getFullscreenHeight()
{
    return fullscreenHeight();
}

[[nodiscard]] unsigned int getWindowedWidth()
{
    return windowedWidth();
}

[[nodiscard]] unsigned int getWindowedHeight()
{
    return windowedHeight();
}

[[nodiscard]] unsigned int getWidth()
{
    return getFullscreen() ? getFullscreenWidth() : getWindowedWidth();
}

[[nodiscard]] unsigned int getHeight()
{
    return getFullscreen() ? getFullscreenHeight() : getWindowedHeight();
}

[[nodiscard]] bool getShowMessages()
{
    return showMessages();
}

[[nodiscard]] bool getDebug()
{
    return getOfficial() ? false : debug();
}

[[nodiscard]] bool getPulse()
{
    return getOfficial() ? true : pulseEnabled();
}

[[nodiscard]] bool getBeatPulse()
{
    return getOfficial() ? true : beatPulse();
}

[[nodiscard]] bool getInvincible()
{
    return getOfficial() ? false : invincible();
}

[[nodiscard]] bool get3D()
{
    return _3DEnabled();
}

[[nodiscard]] float get3DMultiplier()
{
    return _3DMultiplier();
}

[[nodiscard]] unsigned int get3DMaxDepth()
{
    return _3DMaxDepth();
}

[[nodiscard]] bool getAutoRestart()
{
    return autoRestart();
}

[[nodiscard]] bool getFlash()
{
    return flashEnabled();
}

[[nodiscard]] bool getShowTrackedVariables()
{
    return showTrackedVariables();
}

[[nodiscard]] bool getMusicSpeedDMSync()
{
    return musicSpeedDMSync();
}

[[nodiscard]] unsigned int getMaxFPS()
{
    return maxFPS();
}

[[nodiscard]] unsigned int getAntialiasingLevel()
{
    return antialiasingLevel();
}

[[nodiscard]] bool getShowFPS()
{
    return showFPS();
}

[[nodiscard]] bool getServerLocal()
{
    return serverLocal();
}

[[nodiscard]] bool getServerVerbose()
{
    return serverVerbose();
}

[[nodiscard]] bool getMouseVisible()
{
    return mouseVisible();
}

[[nodiscard]] float getMusicSpeedMult()
{
    return musicSpeedMult();
}

[[nodiscard]] bool getDrawTextOutlines()
{
    return drawTextOutlines();
}

[[nodiscard]] bool getDarkenUnevenBackgroundChunk()
{
    return darkenUnevenBackgroundChunk();
}

[[nodiscard]] bool getRotateToStart()
{
    return rotateToStart();
}

[[nodiscard]] float getJoystickDeadzone()
{
    return joystickDeadzone();
}

[[nodiscard]] float getTextPadding()
{
    return textPadding();
}

[[nodiscard]] float getTextScaling()
{
    return textScaling();
}

[[nodiscard]] float getTimescale()
{
    return getOfficial() ? 1.f : timescale();
}

[[nodiscard]] bool getShowKeyIcons()
{
    return showKeyIcons();
}

[[nodiscard]] float getKeyIconsScale()
{
    return keyIconsScale();
}

[[nodiscard]] bool getFirstTimePlaying()
{
    return firstTimePlaying();
}

[[nodiscard]] bool getSaveLocalBestReplayToFile()
{
    return saveLocalBestReplayToFile();
}

[[nodiscard]] bool getShowLevelInfo()
{
    return showLevelInfo();
}

[[nodiscard]] bool getShowTimer()
{
    return getOfficial() ? true : showTimer();
}

[[nodiscard]] bool getShowStatusText()
{
    return getOfficial() ? true : showStatusText();
}

//***********************************************************
//
// KEYBOARD/MOUSE BINDS
//
//***********************************************************

//**************************************************
// Game start check

inline constexpr int maxBinds{4};

void resizeTrigger(ssvs::Input::Trigger& trig) noexcept
{
    std::vector<ssvs::Input::Combo>& combos{trig.getCombos()};

    // Remove empty slots to agglomerate all binds
    // close to each other
    auto it{combos.begin()};
    while(it != combos.end())
    {
        if(it->isUnbound())
        {
            combos.erase(it);
            continue;
        }
        ++it;
    }
    // if the config has more binds than are supported
    while(combos.size() > maxBinds)
    {
        combos.pop_back();
    }
    // if the config has less binds fill the
    // spots with unbound combos
    while(combos.size() < maxBinds)
    {
        combos.emplace_back(ssvs::Input::Combo({ssvs::KKey::Unknown}));
    }
}

void keyboardBindsSanityCheck()
{
    resizeTrigger(triggerRotateCCW());
    resizeTrigger(triggerRotateCW());
    resizeTrigger(triggerFocus());
    resizeTrigger(triggerSelect());
    resizeTrigger(triggerExit());
    resizeTrigger(triggerForceRestart());
    resizeTrigger(triggerRestart());
    resizeTrigger(triggerReplay());
    resizeTrigger(triggerScreenshot());
    resizeTrigger(triggerSwap());
    resizeTrigger(triggerUp());
    resizeTrigger(triggerDown());
}

//**************************************************
// Get trigger names

std::string bindToHumanReadableName(std::string s)
{
    if(s.starts_with('k'))
    {
        return s.substr(1);
    }

    if(s == "bLeft")
    {
        return "LMB";
    }

    if(s == "bRight")
    {
        return "RMB";
    }

    if(s == "bMiddle")
    {
        return "MMB";
    }

    return s;
}


const std::array<TriggerGetter, toSizeT(Tid::TriggersCount)> triggerGetters{
    []() -> ssvs::Input::Trigger& { return triggerRotateCCW(); },
    []() -> ssvs::Input::Trigger& { return triggerRotateCW(); },
    []() -> ssvs::Input::Trigger& { return triggerFocus(); },
    []() -> ssvs::Input::Trigger& { return triggerSelect(); },
    []() -> ssvs::Input::Trigger& { return triggerExit(); },
    []() -> ssvs::Input::Trigger& { return triggerForceRestart(); },
    []() -> ssvs::Input::Trigger& { return triggerRestart(); },
    []() -> ssvs::Input::Trigger& { return triggerReplay(); },
    []() -> ssvs::Input::Trigger& { return triggerScreenshot(); },
    []() -> ssvs::Input::Trigger& { return triggerSwap(); },
    []() -> ssvs::Input::Trigger& { return triggerUp(); },
    []() -> ssvs::Input::Trigger& { return triggerDown(); },
    []() -> ssvs::Input::Trigger& { return triggerNextPack(); },
    []() -> ssvs::Input::Trigger& { return triggerPreviousPack(); },
    []() -> ssvs::Input::Trigger& { return triggerLuaConsole(); },
    []() -> ssvs::Input::Trigger& { return triggerPause(); }};

[[nodiscard]] std::string getKeyboardBindNames(const Tid bindID)
{
    int j;
    std::string bindNames;

    const auto combos = triggerGetters.at(toSizeT(bindID))().getCombos();

    for(const auto& c : combos)
    {
        if(c.isUnbound())
        {
            break;
        }

        const auto keyBind{c.getKeys()};
        for(j = 0; j <= ssvs::KKey::KeyCount; ++j)
        {
            if(!keyBind[j])
            {
                continue;
            }

            if(!bindNames.empty())
            {
                bindNames += ", ";
            }

            // names are shifted compared to the Key enum
            bindNames +=
                bindToHumanReadableName(ssvs::getKKeyName(ssvs::KKey(j - 1)));
            break;
        }

        const auto btnBinds{c.getBtns()};
        for(j = 0; j <= ssvs::MBtn::ButtonCount; ++j)
        {
            if(!btnBinds[j])
            {
                continue;
            }

            if(!bindNames.empty())
            {
                bindNames += ", ";
            }

            // same as with keys
            bindNames +=
                bindToHumanReadableName(ssvs::getMBtnName(ssvs::MBtn(j - 1)));
            break;
        }
    }

    Utils::uppercasify(bindNames);
    return bindNames;
}

//**************************************************
// Add new key binds

void rebindTrigger(
    ssvs::Input::Trigger& trig, const int key, const int btn, int index)
{
    // if both slots are taken replace the first one
    if(index >= maxBinds)
    {
        index = 0;
        trig.getCombos().at(index).clearBind();
    }

    if(key > -1)
    {
        trig.getCombos().at(index).addKey(ssvs::KKey(key));
    }
    else
    {
        trig.getCombos().at(index).addBtn(ssvs::MBtn(btn));
    }
}

//**************************************************
// Unbind key

void clearTriggerBind(ssvs::Input::Trigger& trig, const int index)
{
    trig.getCombos().at(index).clearBind();
}

//***********************************************************
//
// JOYSTICK BINDS
//
//***********************************************************

//**********************************************
// Get binds names

const std::array<JoystickTriggerGetter,
    toSizeT(Joystick::Jid::JoystickBindsCount)>
    joystickTriggerGetters{//
        []() -> unsigned int { return joystickSelect(); },
        []() -> unsigned int { return joystickExit(); },
        []() -> unsigned int { return joystickFocus(); },
        []() -> unsigned int { return joystickSwap(); },
        []() -> unsigned int { return joystickForceRestart(); },
        []() -> unsigned int { return joystickRestart(); },
        []() -> unsigned int { return joystickReplay(); },
        []() -> unsigned int { return joystickScreenshot(); },
        []() -> unsigned int { return joystickNextPack(); },
        []() -> unsigned int { return joystickPreviousPack(); },
        []() -> unsigned int { return joystickAddToFavorites(); },
        []() -> unsigned int { return joystickFavoritesMenu(); }};

const std::string getJoystickBindNames(const Joystick::Jid bindID)
{
    static constexpr std::array<std::array<std::string_view, 2>, 12>
        buttonsNames{{{"A", "SQUARE"}, {"B", "CROSS"}, {"X", "CIRCLE"},
            {"Y", "TRIANGLE"}, {"LB", "L1"}, {"RB", "R1"}, {"BACK", "L2"},
            {"START", "R2"}, {"LEFT STICK", "SELECT"}, {"RIGHT STICK", "START"},
            {"LT", "LEFT STICK"}, {"RT", "RIGHT STICK"}}};

    std::string bindName;
    const unsigned int value{joystickTriggerGetters[toSizeT(bindID)]()};

    if(value == 33)
    {
        bindName = "";
    }
    else
    {
        constexpr unsigned int msVendorId{0x045E};
        constexpr unsigned int sonyVendorId{0x54C};
        const unsigned int vendorId{
            sf::Joystick::isConnected(0)
                ? sf::Joystick::getIdentification(0).vendorId
                : 0};

        switch(vendorId)
        {
            case msVendorId:
                bindName = value >= 12 ? "" : buttonsNames[value][0];
                break;
            case sonyVendorId:
                bindName = value >= 12 ? "" : buttonsNames[value][1];
                break;
            default: bindName = ssvu::toStr(value); break;
        }
    }

    return bindName;
}

//**********************************************
// Get bind

void loadAllJoystickBinds()
{
    for(std::size_t i{0u}; i < Config::joystickTriggerGetters.size(); ++i)
    {
        Joystick::setJoystickBind(Config::joystickTriggerGetters[i](), i);
    }
}

//**********************************************
// Set bind

const std::array<JoystickTriggerSetter,
    toSizeT(Joystick::Jid::JoystickBindsCount)>
    joystickTriggerSetters{//
        [](const unsigned int btn) { joystickSelect() = btn; },
        [](const unsigned int btn) { joystickExit() = btn; },
        [](const unsigned int btn) { joystickFocus() = btn; },
        [](const unsigned int btn) { joystickSwap() = btn; },
        [](const unsigned int btn) { joystickForceRestart() = btn; },
        [](const unsigned int btn) { joystickRestart() = btn; },
        [](const unsigned int btn) { joystickReplay() = btn; },
        [](const unsigned int btn) { joystickScreenshot() = btn; },
        [](const unsigned int btn) { joystickNextPack() = btn; },
        [](const unsigned int btn) { joystickPreviousPack() = btn; },
        [](const unsigned int btn) { joystickAddToFavorites() = btn; },
        [](const unsigned int btn) { joystickFavoritesMenu() = btn; }};

ssvs::Input::Trigger& getTrigger(const Tid tid)
{
    return triggerGetters[toSizeT(tid)]();
}

} // namespace hg::Config
