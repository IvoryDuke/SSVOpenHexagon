// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#pragma once

#include "SSVOpenHexagon/Global/Version.hpp"
#include "SSVOpenHexagon/Core/Joystick.hpp"
#include "SSVOpenHexagon/Utils/Casts.hpp"

#include <vector>
#include <string>
#include <array>

namespace ssvs
{
class GameWindow;
} // namespace ssvs

namespace ssvs::Input
{
class Trigger;
} // namespace ssvs::Input

namespace hg::Config
{

inline constexpr GameVersion GAME_VERSION{2, 0, 4};

void loadConfig(const std::vector<std::string>& mOverridesIds);
void resetConfigToDefaults();
void resetBindsToDefaults();
void saveConfig();

[[nodiscard]] bool isEligibleForScore();

void recalculateSizes();
void setFullscreen(ssvs::GameWindow& mWindow, bool mFullscreen);

void refreshWindowSize(unsigned int mWidth, unsigned int mHeight);
void setCurrentResolution(
    ssvs::GameWindow& mWindow, unsigned int mWidth, unsigned int mHeight);
void setCurrentResolutionAuto(ssvs::GameWindow& mWindow);

void setVsync(ssvs::GameWindow& mWindow, bool mValue);
void setLimitFPS(ssvs::GameWindow& mWindow, bool mValue);
void setMaxFPS(ssvs::GameWindow& mWindow, unsigned int mValue);
void setAntialiasingLevel(ssvs::GameWindow& mWindow, unsigned int mValue);

void setOnline(bool mOnline);
void setOfficial(bool mOfficial);
void setDebug(bool mDebug);
void setNoRotation(bool mNoRotation);
void setNoBackground(bool mNoBackground);
void setBlackAndWhite(bool mBlackAndWhite);
void setNoSound(bool mNoSound);
void setNoMusic(bool mNoMusic);
void setPulse(bool mPulse);
void set3D(bool m3D);
void setInvincible(bool mInvincible);
void setAutoRestart(bool mAutoRestart);
void setSoundVolume(float mVolume);
void setMusicVolume(float mVolume);
void setFlash(bool mFlash);
void setMusicSpeedDMSync(bool mValue);
void setShowFPS(bool mValue);
void setServerLocal(bool mValue);
void setServerVerbose(bool mValue);
void setMouseVisible(bool mValue);
void setMusicSpeedMult(float mValue);
void setDrawTextOutlines(bool mX);
void setDarkenUnevenBackgroundChunk(bool mX);
void setRotateToStart(bool mX);
void setJoystickDeadzone(float mX);
void setTextPadding(float mX);
void setTextScaling(float mX);
void setTimescale(float mX);
void setShowKeyIcons(bool mX);
void setKeyIconsScale(float mX);
void setFirstTimePlaying(bool mX);
void setSaveLocalBestReplayToFile(bool mX);
void setShowLevelInfo(bool mX);
void setShowTimer(bool mX);
void setShowStatusText(bool mX);

[[nodiscard]] bool getOnline();
[[nodiscard]] bool getOfficial();
[[nodiscard]] const std::string& getUneligibilityReason();
[[nodiscard]] float getSizeX();
[[nodiscard]] float getSizeY();
[[nodiscard]] float getSpawnDistance();
[[nodiscard]] float getZoomFactor();
[[nodiscard]] int getPixelMultiplier();
[[nodiscard]] float getPlayerSpeed();
[[nodiscard]] float getPlayerFocusSpeed();
[[nodiscard]] float getPlayerSize();
[[nodiscard]] bool getNoRotation();
[[nodiscard]] bool getNoBackground();
[[nodiscard]] bool getBlackAndWhite();
[[nodiscard]] bool getNoSound();
[[nodiscard]] bool getNoMusic();
[[nodiscard]] float getSoundVolume();
[[nodiscard]] float getMusicVolume();
[[nodiscard]] bool getLimitFPS();
[[nodiscard]] bool getVsync();
[[nodiscard]] bool getAutoZoomFactor();
[[nodiscard]] bool getFullscreen();
[[nodiscard]] bool getWindowedAutoResolution();
[[nodiscard]] bool getFullscreenAutoResolution();
[[nodiscard]] unsigned int getFullscreenWidth();
[[nodiscard]] unsigned int getFullscreenHeight();
[[nodiscard]] unsigned int getWindowedWidth();
[[nodiscard]] unsigned int getWindowedHeight();
[[nodiscard]] unsigned int getWidth();
[[nodiscard]] unsigned int getHeight();
[[nodiscard]] bool getShowMessages();
[[nodiscard]] bool getRotateToStart();

[[nodiscard, gnu::const]] inline constexpr GameVersion getVersion()
{
    return GAME_VERSION;
}

[[nodiscard]] const std::string& getVersionString();

[[nodiscard]] bool getDebug();
[[nodiscard]] bool getPulse();
[[nodiscard]] bool getBeatPulse();
[[nodiscard]] bool getInvincible();
[[nodiscard]] bool get3D();
[[nodiscard]] unsigned int get3DMaxDepth();
[[nodiscard]] float get3DMultiplier();
[[nodiscard]] bool getAutoRestart();
[[nodiscard]] bool getFlash();
[[nodiscard]] bool getShowTrackedVariables();
[[nodiscard]] bool getMusicSpeedDMSync();
[[nodiscard]] unsigned int getMaxFPS();
[[nodiscard]] bool getShowFPS();
[[nodiscard]] unsigned int getAntialiasingLevel();
[[nodiscard]] bool getServerLocal();
[[nodiscard]] bool getServerVerbose();
[[nodiscard]] bool getMouseVisible();
[[nodiscard]] float getMusicSpeedMult();
[[nodiscard]] bool getDrawTextOutlines();
[[nodiscard]] bool getDarkenUnevenBackgroundChunk();
[[nodiscard]] float getJoystickDeadzone();
[[nodiscard]] float getTextPadding();
[[nodiscard]] float getTextScaling();
[[nodiscard]] float getTimescale();
[[nodiscard]] bool getShowKeyIcons();
[[nodiscard]] float getKeyIconsScale();
[[nodiscard]] bool getFirstTimePlaying();
[[nodiscard]] bool getSaveLocalBestReplayToFile();
[[nodiscard]] bool getShowLevelInfo();
[[nodiscard]] bool getShowTimer();
[[nodiscard]] bool getShowStatusText();

// keyboard binds

enum class Tid : int
{
    Unknown = -1,

    RotateCCW = 0,
    RotateCW,
    Focus,
    Select,
    Exit,
    ForceRestart,
    Restart,
    Replay,
    Screenshot,
    Swap,
    Up,
    Down,
    NextPack,
    PreviousPack,
    LuaConsole,
    Pause,

    TriggersCount
};

void keyboardBindsSanityCheck();

[[nodiscard]] std::string getKeyboardBindNames(const Tid bindID);

using TriggerGetter = ssvs::Input::Trigger& (*)();
extern const std::array<TriggerGetter, toSizeT(Tid::TriggersCount)>
    triggerGetters;

void rebindTrigger(
    ssvs::Input::Trigger& trig, const int key, const int btn, int index);

void clearTriggerBind(ssvs::Input::Trigger& trig, const int index);

// joystick binds

void joystickBindsSanityCheck();

[[nodiscard]] const std::string getJoystickBindNames(
    const Joystick::Jid bindID);

using JoystickTriggerGetter = unsigned int (*)();
extern const std::array<JoystickTriggerGetter,
    toSizeT(Joystick::Jid::JoystickBindsCount)>
    joystickTriggerGetters;

void loadAllJoystickBinds();

using JoystickTriggerSetter = void (*)(const unsigned int button);
extern const std::array<JoystickTriggerSetter,
    toSizeT(Joystick::Jid::JoystickBindsCount)>
    joystickTriggerSetters;

ssvs::Input::Trigger& getTrigger(const Tid tid);

} // namespace hg::Config
