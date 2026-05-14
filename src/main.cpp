#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>

using namespace geode::prelude;

namespace {
constexpr float PI = 3.14159265358979323846f;

float easeOutBounce(float t) {
  constexpr float n1 = 7.5625f;
  constexpr float d1 = 2.75f;

  if (t < 1.0f / d1) {
    return n1 * t * t;
  }
  if (t < 2.0f / d1) {
    t -= 1.5f / d1;
    return n1 * t * t + 0.75f;
  }
  if (t < 2.5f / d1) {
    t -= 2.25f / d1;
    return n1 * t * t + 0.9375f;
  }

  t -= 2.625f / d1;
  return n1 * t * t + 0.984375f;
}

float easeIn(std::string_view family, float t) {
  if (family == "Sine")
    return 1.0f - std::cos((t * PI) / 2.0f);
  if (family == "Quad")
    return t * t;
  if (family == "Cubic")
    return t * t * t;
  if (family == "Quart")
    return t * t * t * t;
  if (family == "Quint")
    return t * t * t * t * t;
  if (family == "Expo")
    return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
  if (family == "Circ")
    return 1.0f - std::sqrt(1.0f - t * t);
  if (family == "Back") {
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
  }
  if (family == "Elastic") {
    constexpr float c4 = (2.0f * PI) / 3.0f;
    return t == 0.0f   ? 0.0f
           : t == 1.0f ? 1.0f
                       : -std::pow(2.0f, 10.0f * t - 10.0f) *
                             std::sin((t * 10.0f - 10.75f) * c4);
  }
  if (family == "Bounce")
    return 1.0f - easeOutBounce(1.0f - t);

  return t;
}

float easeOut(std::string_view family, float t) {
  if (family == "Sine")
    return std::sin((t * PI) / 2.0f);
  if (family == "Quad")
    return 1.0f - (1.0f - t) * (1.0f - t);
  if (family == "Cubic")
    return 1.0f - std::pow(1.0f - t, 3.0f);
  if (family == "Quart")
    return 1.0f - std::pow(1.0f - t, 4.0f);
  if (family == "Quint")
    return 1.0f - std::pow(1.0f - t, 5.0f);
  if (family == "Expo")
    return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
  if (family == "Circ")
    return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
  if (family == "Back") {
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
  }
  if (family == "Elastic") {
    constexpr float c4 = (2.0f * PI) / 3.0f;
    return t == 0.0f   ? 0.0f
           : t == 1.0f ? 1.0f
                       : std::pow(2.0f, -10.0f * t) *
                                 std::sin((t * 10.0f - 0.75f) * c4) +
                             1.0f;
  }
  if (family == "Bounce")
    return easeOutBounce(t);

  return t;
}

float easeInOut(std::string_view family, float t) {
  if (family == "Sine")
    return -(std::cos(PI * t) - 1.0f) / 2.0f;
  if (family == "Quad")
    return t < 0.5f ? 2.0f * t * t
                    : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
  if (family == "Cubic")
    return t < 0.5f ? 4.0f * t * t * t
                    : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
  if (family == "Quart")
    return t < 0.5f ? 8.0f * t * t * t * t
                    : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
  if (family == "Quint")
    return t < 0.5f ? 16.0f * t * t * t * t * t
                    : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
  if (family == "Expo") {
    return t == 0.0f   ? 0.0f
           : t == 1.0f ? 1.0f
           : t < 0.5f  ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
                       : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
  }
  if (family == "Circ") {
    return t < 0.5f
               ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f
               : (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) /
                     2.0f;
  }
  if (family == "Back") {
    constexpr float c1 = 1.70158f;
    constexpr float c2 = c1 * 1.525f;
    return t < 0.5f
               ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) /
                     2.0f
               : (std::pow(2.0f * t - 2.0f, 2.0f) *
                      ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) +
                  2.0f) /
                     2.0f;
  }
  if (family == "Elastic") {
    constexpr float c5 = (2.0f * PI) / 4.5f;
    return t == 0.0f   ? 0.0f
           : t == 1.0f ? 1.0f
           : t < 0.5f  ? -(std::pow(2.0f, 20.0f * t - 10.0f) *
                          std::sin((20.0f * t - 11.125f) * c5)) /
                            2.0f
                      : (std::pow(2.0f, -20.0f * t + 10.0f) *
                         std::sin((20.0f * t - 11.125f) * c5)) /
                                2.0f +
                            1.0f;
  }
  if (family == "Bounce")
    return t < 0.5f ? (1.0f - easeOutBounce(1.0f - 2.0f * t)) / 2.0f
                    : (1.0f + easeOutBounce(2.0f * t - 1.0f)) / 2.0f;

  return t;
}

float applyEasing(std::string const &easing, float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  if (easing == "Linear")
    return t;

  auto split = easing.find(' ');
  if (split == std::string::npos)
    return t;

  auto family = std::string_view(easing).substr(0, split);
  auto direction = std::string_view(easing).substr(split + 1);

  if (direction == "In")
    return easeIn(family, t);
  if (direction == "Out")
    return easeOut(family, t);
  if (direction == "In+Out" || direction == "InOut")
    return easeInOut(family, t);

  return t;
}
} // namespace

class $modify(MyCCTextInputNode, CCTextInputNode) {
  struct Fields {
    CCLabelBMFont *m_trueCaret = nullptr;
    CCLabelBMFont *m_smoothCaret = nullptr;
  };

  bool init(float width, float height, char const *placeholder,
            char const *textFont, int fontSize, char const *labelFont) {
    if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize,
                               labelFont)) {
      return false;
    }

    auto children = this->getChildren();
    if (!children)
      return true;

    for (auto *child : CCArrayExt<CCNode *>(children)) {
      if (auto *label = typeinfo_cast<CCLabelBMFont *>(child)) {
        std::string_view content = label->getString();
        if (content == "|" &&
            std::string_view(label->getFntFile()) == "chatFont.fnt") {
          m_fields->m_trueCaret = label;
          break;
        }
      }
    }

    auto trueCaret = m_fields->m_trueCaret;

    if (!trueCaret) {
      log::warn("Could not find the true caret");
      return true;
    }

    auto smoothCaret = CCLabelBMFont::create("|", "chatFont.fnt");
    smoothCaret->setColor(trueCaret->getColor());
    smoothCaret->setOpacity(trueCaret->getOpacity());

    smoothCaret->setContentSize(trueCaret->getContentSize());
    smoothCaret->setAnchorPoint(trueCaret->getAnchorPoint());
    smoothCaret->ignoreAnchorPointForPosition(
        trueCaret->isIgnoreAnchorPointForPosition());

    smoothCaret->setPosition(trueCaret->getPosition());
    smoothCaret->setScale(trueCaret->getScale());
    smoothCaret->setID("smooth-caret"_spr);

    trueCaret->setOpacity(0);

    trueCaret->getParent()->addChild(smoothCaret);
    this->schedule(schedule_selector(MyCCTextInputNode::updSmoothCaret));
    m_fields->m_smoothCaret = smoothCaret;

    return true;
  }

  void updSmoothCaret(float dt) {
    auto trueCaret = m_fields->m_trueCaret;
    auto smoothCaret = m_fields->m_smoothCaret;
    if (!trueCaret || !smoothCaret)
      return;

    CCPoint oldPos = smoothCaret->getPosition();

    smoothCaret->setColor(trueCaret->getColor());
    smoothCaret->setContentSize(trueCaret->getContentSize());
    smoothCaret->setAnchorPoint(trueCaret->getAnchorPoint());
    smoothCaret->ignoreAnchorPointForPosition(
        trueCaret->isIgnoreAnchorPointForPosition());

    float lerpSpeed = Mod::get()->getSettingValue<float>("Weight");
    auto easing = Mod::get()->getSettingValue<std::string>("Easing");
    float progress = applyEasing(easing, std::min(1.0f, dt * lerpSpeed));

    float newX = std::lerp(smoothCaret->getPositionX(),
                           trueCaret->getPositionX(), progress);
    float newY = std::lerp(smoothCaret->getPositionY(),
                           trueCaret->getPositionY(), progress);

    float baseScale = trueCaret->getScale();
    smoothCaret->setScale(baseScale);

    if (Mod::get()->getSettingValue<bool>("Stretch")) {
      float velocityX = dt > 0.0f ? (newX - oldPos.x) / dt : 0.0f;
      float stretch = 1.0f + std::abs(velocityX) * 0.1f;
      smoothCaret->setScaleX(stretch * baseScale);
    }

    smoothCaret->setPosition(newX, newY);
    smoothCaret->setVisible(trueCaret->isVisible());
  }
};
