// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#pragma once

#include "SSVOpenHexagon/Utils/Ticker.hpp"
#include "SSVOpenHexagon/Utils/Utils.hpp"

#include <SSVUtils/Core/Common/Frametime.hpp>
#include <SSVStart/Utils/Vector2.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace hg
{

class HexagonGame;
class CWall;
class CCustomWall;

class CPlayer
{
private:
    sf::Vector2f startPos;
    sf::Vector2f pos;

    std::array<sf::Vector2f, 3> vertexPositions;
    std::vector<sf::Vector2f> pivotVertexes;

    float hue;
    float angle;
    float lastAngle;
    float size;
    float speed;
    float focusSpeed;

    bool dead;
    bool justSwapped;

    float radius;
    float maxSafeDistance;

    Ticker swapTimer;
    Ticker swapBlinkTimer;
    Ticker deadEffectTimer;

    float pivotRadius;
    static constexpr float pivotBorderThickness{5.f};

    void drawCommon(HexagonGame& mHexagonGame);
    void drawPivot(HexagonGame& mHexagonGame, const sf::Color& mCapColor);
    void drawPivot3D(HexagonGame& mHexagonGame, const sf::Color& mWallColor,
        const sf::Color& mCapColor);
    void drawDeathEffect(HexagonGame& mHexagonGame);
    void drawDeathEffect3D(HexagonGame& mHexagonGame, const sf::Color& mWallColors);

    template <typename Wall>
    bool checkWallCollisionEscape(const Wall& wall, sf::Vector2f& mPos)
    {
        // To find the closest wall side we intersect the circumference of the possible
        // player positions with the sides of the wall. We use the intersection closest
        // to the player's position as post collision target.
        // If an escape route could not be found player is killed.

        bool saved{false};
        sf::Vector2f vec1, vec2;
        float tempDistance, safeDistance{maxSafeDistance};
        const unsigned int vxIncrement{wall.isCustomWall() ? 1u : 2u};
        const std::array<sf::Vector2f, 4>& wVertexes{wall.getVertexPositions()};

        const auto getLineCircleIntersection = [this](sf::Vector2f& i1,
            sf::Vector2f& i2, const sf::Vector2f& p1,
            const sf::Vector2f& p2) -> unsigned int
        {
            const float dx{p2.x - p1.x};
            const float dy{p2.y - p1.y};
            const float a{dx * dx + dy * dy};
            const float b{2.f * (dx * p1.x + dy * p1.y)};
            const float c{p1.x * p1.x + p1.y * p1.y - radius * radius};
            const float delta{b * b - 4.f * a * c};

            // No intersections.
            if(delta < 0.f)
            {
                return 0u;
            }

            float t;
            const float twoA{2.f * a};
            
            // One intersection.
            if(delta < hg::Utils::epsilon)
            {
                t = -b / twoA;
                i1 = {p1.x + t * dx, p1.y + t * dy};
                return 1u;
            }

            // Two intersections.
            const float sqrtDelta{hg::Utils::fastSqrt(delta)};
            t = (-b + sqrtDelta) / twoA;
            i1 = {p1.x + t * dx, p1.y + t * dy};
            t = (-b - sqrtDelta) / twoA;
            i2 = {p1.x + t * dx, p1.y + t * dy};
            return 2u;
        };

        const auto assignResult = [&]() {
            if((tempDistance = ssvs::getMagSquared(vec1 - mPos)) < safeDistance)
            {
                mPos = vec1;
                saved = true;
                safeDistance = tempDistance;
            }
        };

        for(unsigned int i{0u}, j{3u}; i < 4u; i += vxIncrement, j = i - 1)
        {
            switch(getLineCircleIntersection(vec1, vec2, wVertexes[i], wVertexes[j]))
            {
                case 1u:
                    assignResult();
                    break;

                case 2u:
                    if(ssvs::getMagSquared(vec1 - mPos) >
                        ssvs::getMagSquared(vec2 - mPos))
                    {
                        vec1 = vec2;
                    }
                    assignResult();
                    break;

                default:
                    break;
            }
        }

        return saved;
    }

public:

    CPlayer(const sf::Vector2f& mPos, const float swapCooldown) noexcept;

    [[gnu::always_inline, nodiscard]]
    const sf::Vector2f& getPosition() const noexcept
    {
        return pos;
    }

    [[gnu::always_inline, nodiscard]] float getPlayerAngle() const noexcept
    {
        return angle;
    }

    void setPlayerAngle(const float newAng) noexcept
    {
        angle = newAng;
    }

    [[gnu::always_inline, nodiscard]]
    const std::vector<sf::Vector2f>& getPivotVertexes() const noexcept
    {
        return pivotVertexes;
    }

    [[gnu::always_inline, nodiscard]] float getPivotRadius() const noexcept
    {
        return pivotRadius;
    }

    void playerSwap(HexagonGame& mHexagonGame, bool mPlaySound);

    void kill(HexagonGame& mHexagonGame);

    void update(HexagonGame& mHexagonGame, const ssvu::FT mFT);
    void updateInput(HexagonGame& mHexagonGame, const ssvu::FT mFT);
    void updatePosition(const HexagonGame& mHexagonGame, const ssvu::FT mFT);
    void updateCollisionValues(const HexagonGame& mHexagonGame, const ssvu::FT mFT);

    void draw(HexagonGame& mHexagonGame, const sf::Color& mCapColor);
    void draw3D(HexagonGame& mHexagonGame, const sf::Color& mWallColor,
        const sf::Color& mCapColor);

    void setSides(unsigned int mSideNumber) noexcept
    {
        pivotVertexes.resize(mSideNumber);
    }

    [[nodiscard]] bool push(const HexagonGame& mHexagonGame, const CWall& wall,
        const sf::Vector2f& mCenterPos, ssvu::FT mFT);

    [[nodiscard]] bool push(const HexagonGame& mHexagonGame,
        const hg::CCustomWall& wall, ssvu::FT mFT);

    [[nodiscard]] bool getJustSwapped() const noexcept;
};

} // namespace hg
