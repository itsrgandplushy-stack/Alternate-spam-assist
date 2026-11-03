#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <algorithm>

using namespace geode::prelude;

namespace {
    constexpr float UPWARD_TARGET_SPEED = 7.0f;

    bool settingBool(char const* key) {
        return Mod::get()->getSettingValue<bool>(key);
    }

    float settingFloat(char const* key) {
        return Mod::get()->getSettingValue<float>(key);
    }
}

class $modify(PureAltSpamPlayer, PlayerObject) {
    void update(float dt) {
        PlayerObject::update(dt);

        // Only affect the wave gamemode. No automatic taps are generated:
        // the player's own touch input remains in control.
        if (!settingBool("enabled") || !m_isDart || m_isDead || m_isLocked) {
            return;
        }

        const bool straight = settingBool("straight-assist");
        const bool upward = settingBool("up-assist");

        // If both are selected, straight assist takes priority.
        if (!straight && !upward) {
            return;
        }

        const float strength = std::clamp(settingFloat("strength"), 0.05f, 1.0f);
        const float target = straight
            ? 0.0f
            : (m_isUpsideDown ? -UPWARD_TARGET_SPEED : UPWARD_TARGET_SPEED);

        // Frame-time-scaled smoothing. Smaller strengths preserve more of
        // the player's natural wave control; larger strengths guide harder.
        const float blend = std::clamp(strength * dt * 60.0f, 0.0f, 1.0f);
        m_yVelocity += (target - m_yVelocity) * blend;
    }
};
