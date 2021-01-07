// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#include "SSVOpenHexagon/Core/HexagonGame.hpp"
#include "SSVOpenHexagon/Components/CWall.hpp"
#include "SSVOpenHexagon/Components/CCustomWall.hpp"
#include "SSVOpenHexagon/Utils/Color.hpp"
#include "SSVOpenHexagon/Utils/Ticker.hpp"

#include "SSVOpenHexagon/Global/Config.hpp"

#include <SSVStart/Utils/SFML.hpp>

#include <SSVUtils/Core/Common/Frametime.hpp>
#include <SSVUtils/Core/Utils/Math.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

using namespace hg::Utils;

namespace hg
{

CPlayer::CPlayer(const sf::Vector2f& mPos, const float swapCooldown) noexcept
    : startPos{mPos}, pos{mPos}, hue{0}, angle{0}, lastAngle{0},
      size{Config::getPlayerSize()}, speed{Config::getPlayerSpeed()},
      focusSpeed{Config::getPlayerFocusSpeed()}, dead{false},
      justSwapped{false}, swapTimer{swapCooldown},
      swapBlinkTimer{swapCooldown / 6.f}, deadEffectTimer{80.f, false}
{
}

inline constexpr unsigned int playerSides{3};

void CPlayer::drawCommon(HexagonGame& mHexagonGame)
{
    sf::Color colorMain{!deadEffectTimer.isRunning()
                            ? mHexagonGame.getColorPlayer()
                            : ssvs::getColorFromHSV(hue / 360.f, 1.f, 1.f)};

    const float triangleWidth = mHexagonGame.getInputFocused() ? -1.5f : 3.f;

    vertexPositions[0] = ssvs::getOrbitRad(pos, angle, size);
    vertexPositions[1] = ssvs::getOrbitRad(
        pos, angle - ssvu::toRad(100.f), size + triangleWidth);
    vertexPositions[2] = ssvs::getOrbitRad(
        pos, angle + ssvu::toRad(100.f), size + triangleWidth);

    if(!swapTimer.isRunning() && !dead)
    {
        colorMain = ssvs::getColorFromHSV(
            (swapBlinkTimer.getCurrent() * 15) / 360.f, 1, 1);
    }

    mHexagonGame.playerTris.reserve_more(playerSides);
    mHexagonGame.playerTris.batch_unsafe_emplace_back(
        colorMain, vertexPositions[0], vertexPositions[1], vertexPositions[2]);
}

void CPlayer::draw(HexagonGame& mHexagonGame, const sf::Color& mCapColor)
{
    drawPivot(mHexagonGame, mCapColor);
    if(deadEffectTimer.isRunning())
    {
        drawDeathEffect(mHexagonGame);
    }
    drawCommon(mHexagonGame);
}

void CPlayer::draw3D(HexagonGame& mHexagonGame, const sf::Color& mWallColor,
    const sf::Color& mCapColor)
{
    drawPivot3D(mHexagonGame, mWallColor, mCapColor);
    if(deadEffectTimer.isRunning())
    {
        drawDeathEffect3D(mHexagonGame, mWallColor);
    }
    drawCommon(mHexagonGame);

    mHexagonGame.wallQuads3D.reserve_more(playerSides * 4);
    const sf::Vector2f& offset3D{mHexagonGame.get3DOffset()};

    unsigned int next;
    for(unsigned int i{0}; i < playerSides; ++i)
    {
        next = (i + 1) % playerSides;
        mHexagonGame.wallQuads3D.batch_unsafe_emplace_back(
            mWallColor, vertexPositions[i], vertexPositions[next],
            sf::Vector2f{vertexPositions[next].x + offset3D.x, vertexPositions[next].y + offset3D.y},
            sf::Vector2f{vertexPositions[i].x + offset3D.x, vertexPositions[i].y + offset3D.y});
    }
}

void CPlayer::drawPivot(HexagonGame& mHexagonGame, const sf::Color& mCapColor)
{
    const unsigned int sides(mHexagonGame.getSides());
    const float div{ssvu::tau / sides * 0.5f};
    const sf::Color colorMain{mHexagonGame.getColorMain()};

    // This value is needed by all walls in the 3D drawing mode, so add
    // the thickness value even if it needs to be subtracted in the next lines.
    pivotRadius = radius * 0.75f + pivotBorderThickness;

/*  Unused
    const sf::Color colorB{Config::getBlackAndWhite()
                               ? sf::Color::Black
                               : mHexagonGame.getColor(1)};
    const sf::Color colorDarkened{Utils::getColorDarkened(colorMain, 1.4f)};
*/

    float sAngle;
    sf::Vector2f p1, p2, p3, p4;
    mHexagonGame.wallQuads.reserve_more(4 * sides);
    mHexagonGame.capTris.reserve(3 * sides);

    for(unsigned int i{0}; i < sides; ++i)
    {
        sAngle = div * 2.f * i;
        p1 = ssvs::getOrbitRad(startPos, sAngle - div, pivotRadius - pivotBorderThickness);
        p2 = ssvs::getOrbitRad(startPos, sAngle + div, pivotRadius - pivotBorderThickness);
        p3 = ssvs::getOrbitRad(startPos, sAngle + div, pivotRadius);
        p4 = ssvs::getOrbitRad(startPos, sAngle - div, pivotRadius);

        mHexagonGame.wallQuads.batch_unsafe_emplace_back(
            colorMain, p1, p2, p3, p4);
        mHexagonGame.capTris.batch_unsafe_emplace_back(
            mCapColor, p1, p2, startPos);
    }
}

void CPlayer::drawPivot3D(HexagonGame& mHexagonGame, const sf::Color& mWallColor,
    const sf::Color& mCapColor)
{
    const unsigned int sides(mHexagonGame.getSides());
    const float div{ssvu::tau / sides * 0.5f};
    pivotRadius = mHexagonGame.getRadius() * 0.75f + pivotBorderThickness;
    const sf::Color colorMain{mHexagonGame.getColorMain()};

    float sAngle;
    sf::Vector2f p1, p2, p3;
    mHexagonGame.capQuads.reserve(4 * sides);
    mHexagonGame.capTris.reserve(3 * sides);
    mHexagonGame.wallQuads3D.reserve_more(4 * sides);
    const sf::Vector2f& offset3D{mHexagonGame.get3DOffset()};

    for(unsigned int i{0}; i < sides; ++i)
    {
        sAngle = div * 2.f * i;
        p1 = ssvs::getOrbitRad(startPos, sAngle - div, pivotRadius - pivotBorderThickness);
        p2 = ssvs::getOrbitRad(startPos, sAngle + div, pivotRadius - pivotBorderThickness);
        p3 = ssvs::getOrbitRad(startPos, sAngle + div, pivotRadius);
        pivotVertexes[i] =
            ssvs::getOrbitRad(startPos, sAngle - div, pivotRadius);

        mHexagonGame.capQuads.batch_unsafe_emplace_back(
            colorMain, p1, p2, p3, pivotVertexes[i]);
        mHexagonGame.capTris.batch_unsafe_emplace_back(
            mCapColor, p1, p2, startPos);
        mHexagonGame.wallQuads3D.batch_unsafe_emplace_back(
            mWallColor, p3, pivotVertexes[i],
            sf::Vector2f{pivotVertexes[i].x + offset3D.x, pivotVertexes[i].y + offset3D.y},
            sf::Vector2f{p3.x + offset3D.x, p3.y + offset3D.y});
    }
}

void CPlayer::drawDeathEffect(HexagonGame& mHexagonGame)
{
    const unsigned int sides(mHexagonGame.getSides());
    const float div{ssvu::tau / mHexagonGame.getSides() * 0.5f};
    const float dRadius{hue / 8.f};
    const float thickness{hue / 20.f};
    const sf::Color colorMain{
        ssvs::getColorFromHSV((360.f - hue) / 360.f, 1.f, 1.f)};

    float sAngle;
    sf::Vector2f p1, p2, p3, p4;
    mHexagonGame.wallQuads.reserve_more(4 * sides);

    for(unsigned int i{0}; i < sides; ++i)
    {
        sAngle = div * 2.f * i;
        p1 = ssvs::getOrbitRad(pos, sAngle - div, radius);
        p2 = ssvs::getOrbitRad(pos, sAngle + div, radius);
        p3 = ssvs::getOrbitRad(pos, sAngle + div, radius + thickness);
        p4 = ssvs::getOrbitRad(pos, sAngle - div, radius + thickness);

        mHexagonGame.wallQuads.batch_unsafe_emplace_back(
            colorMain, p1, p2, p3, p4);
    }
}

void CPlayer::drawDeathEffect3D(HexagonGame& mHexagonGame, const sf::Color& mWallColors)
{
    const unsigned int sides(mHexagonGame.getSides());
    const float div{ssvu::tau / mHexagonGame.getSides() * 0.5f};
    const float radius{hue / 8.f};
    const float thickness{hue / 20.f};
    const sf::Color colorMain{
        ssvs::getColorFromHSV((360.f - hue) / 360.f, 1.f, 1.f)};

    float sAngle;
    sf::Vector2f p1, p2, p3, p4;
    mHexagonGame.wallQuads.reserve_more(4 * sides);
    mHexagonGame.wallQuads3D.reserve_more(4 * 2 * sides);
    const sf::Vector2f& offset3D{mHexagonGame.get3DOffset()};

    for(unsigned int i{0}; i < sides; ++i)
    {
        sAngle = div * 2.f * i;
        p1 = ssvs::getOrbitRad(pos, sAngle - div, radius);
        p2 = ssvs::getOrbitRad(pos, sAngle + div, radius);
        p3 = ssvs::getOrbitRad(pos, sAngle + div, radius + thickness);
        p4 = ssvs::getOrbitRad(pos, sAngle - div, radius + thickness);

        mHexagonGame.wallQuads.batch_unsafe_emplace_back(
            colorMain, p1, p2, p3, p4);
        mHexagonGame.wallQuads3D.batch_unsafe_emplace_back(
            mWallColors, p1, p2,
            sf::Vector2f{p2.x + offset3D.x, p2.y + offset3D.y},
            sf::Vector2f{p1.x + offset3D.x, p1.y + offset3D.y});
        mHexagonGame.wallQuads3D.batch_unsafe_emplace_back(
            mWallColors, p3, p4,
            sf::Vector2f{p4.x + offset3D.x, p4.y + offset3D.y},
            sf::Vector2f{p3.x + offset3D.x, p3.y + offset3D.y});
    }
}

void CPlayer::playerSwap(HexagonGame& mHexagonGame, bool mPlaySound)
{
    angle += ssvu::pi;
    mHexagonGame.runLuaFunctionIfExists<void>("onCursorSwap");

    if(mPlaySound)
    {
        mHexagonGame.getAssets().playSound(
            mHexagonGame.getLevelStatus().swapSound);
    }
}

void CPlayer::kill(HexagonGame& mHexagonGame)
{
    deadEffectTimer.restart();

    if(!Config::getInvincible() && !mHexagonGame.getLevelStatus().tutorialMode)
    {
        dead = true;
    }

    mHexagonGame.death();
}

inline constexpr float collisionPadding{0.025f};

[[nodiscard]] bool CPlayer::push(const HexagonGame& mHexagonGame,
    const CWall& wall, const sf::Vector2f& mCenterPos, const ssvu::FT mFT)
{
    if(dead)
    {
        return false;
    }

    int movement{mHexagonGame.getInputMovement()};
    const sf::Vector2f preCollisionPos{pos};

    // First of all, if it's a rotating wall push player in the direction the
    // wall is rotating by the appropriate amount, but only if the direction
    // of the rotation is different from the direction player is moving.
    const SpeedData& curveData{wall.getCurve()};
    const int speedSign{ssvu::getSign(curveData.speed)};

    if(curveData.speed != 0.f && speedSign != movement)
    {
        wall.moveVertexAlongCurve(pos, mCenterPos, mFT);

        // Calculate angle, add a little padding, and readjust the position.
        angle = ssvs::getRad(pos) + speedSign * collisionPadding;
        updatePosition(mHexagonGame, mFT);
    }

    // Compensate for the player movement.
    movement = -movement;
    const float currentSpeed{mHexagonGame.getPlayerSpeedMult() *
        (mHexagonGame.getInputFocused() ? focusSpeed : speed)};
    angle += ssvu::toRad(currentSpeed * movement * mFT) + movement * collisionPadding;
    updatePosition(mHexagonGame, mFT);

    // Try to find a close enough safe position, if this does not exist
    // player must be killed.
    sf::Vector2f testPos{pos};
    if(!checkWallCollisionEscape<CWall>(wall, testPos))
    {
        return true;
    }

    // If player survived add a little padding.
    pos = testPos;
    pos += ssvs::getNormalized(pos - preCollisionPos) * collisionPadding;
    lastAngle = angle = ssvs::getRad(pos);
    updatePosition(mHexagonGame, mFT);
    return false;
}

[[nodiscard]] bool CPlayer::push(const HexagonGame& mHexagonGame,
    const CCustomWall& wall, const ssvu::FT mFT)
{
    (void)mFT; // Currently unused.

    if(dead)
    {
        return false;
    }

    const sf::Vector2f preCollisionPos{pos};

    // We save ourselves the complications of player movement by reverting
    // the position of player to its previous state.
    angle = lastAngle;
    updatePosition(mHexagonGame, mFT);

    // Try to find a close enough safe position, if this does not exist
    // player must be killed.
    sf::Vector2f testPos{pos};
    if(!checkWallCollisionEscape<CCustomWall>(wall, testPos))
    {
        return true;
    }

    // If player survived add a little padding.
    pos = testPos;
    pos += ssvs::getNormalized(pos - preCollisionPos) * collisionPadding;
    lastAngle = angle = ssvs::getRad(pos);
    updatePosition(mHexagonGame, mFT);
    return false;
}

void CPlayer::update(HexagonGame& mHexagonGame, const ssvu::FT mFT)
{
    if(deadEffectTimer.isRunning())
    {
        deadEffectTimer.update(mFT);

        if(++hue > 360.f)
        {
            hue = 0.f;
        }
        if(dead)
        {
            return;
        }
    }

    swapBlinkTimer.update(mFT);

    if(mHexagonGame.getLevelStatus().swapEnabled && swapTimer.update(mFT))
    {
        swapTimer.stop();
    }

    lastAngle = angle;
}

void CPlayer::updateInput(HexagonGame& mHexagonGame, const ssvu::FT mFT)
{
    const int movement{mHexagonGame.getInputMovement()};

    const float currentSpeed =
        mHexagonGame.getPlayerSpeedMult() *
        (mHexagonGame.getInputFocused() ? focusSpeed : speed);

    angle += ssvu::toRad(currentSpeed * movement * mFT);

    if(mHexagonGame.getLevelStatus().swapEnabled &&
        mHexagonGame.getInputSwap() && !swapTimer.isRunning())
    {
        playerSwap(mHexagonGame, true /* mPlaySound */);
        swapTimer.restart(mHexagonGame.getSwapCooldown());
        swapBlinkTimer.restart(mHexagonGame.getSwapCooldown() / 6.f);
        justSwapped = true;
    }
    else
    {
        justSwapped = false;
    }
}

void CPlayer::updatePosition(
    const HexagonGame& mHexagonGame, const ssvu::FT mFT)
{
    (void)mFT; // Currently unused.

    pos = ssvs::getOrbitRad(startPos, angle, mHexagonGame.getRadius());
}

void CPlayer::updateCollisionValues(
    const HexagonGame& mHexagonGame, const ssvu::FT mFT)
{
    (void)mFT; // Currently unused.

    radius = mHexagonGame.getRadius();
    maxSafeDistance = mHexagonGame.getPlayerSpeedMult() * speed;
    maxSafeDistance *= maxSafeDistance;
    maxSafeDistance += maxSafeDistance;
}

[[nodiscard]] bool CPlayer::getJustSwapped() const noexcept
{
    return justSwapped;
}

} // namespace hg
