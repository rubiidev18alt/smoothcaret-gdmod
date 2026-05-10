#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

using namespace geode::prelude;

class $modify(MyCCTextInputNode, CCTextInputNode) {
	struct Fields {
        CCLabelBMFont* m_trueCaret = nullptr;
		CCLabelBMFont* m_smoothCaret = nullptr;
    };

    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont) {
        if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize, labelFont)) {
            return false;
        }

        auto children = this->getChildren();
        if (!children) return true;

        for (auto* child : CCArrayExt<CCNode*>(children)) {
            if (auto* label = typeinfo_cast<CCLabelBMFont*>(child)) {
                std::string_view content = label->getString();
                if (content == "|") {
                    m_fields->m_trueCaret = label;
					break;
					
                }
            }
        }

		auto trueCaret = m_fields->m_trueCaret;

		auto smoothCaret = CCLabelBMFont::create("|", "chatFont.fnt");
		smoothCaret->setColor(trueCaret->getColor());
		smoothCaret->setOpacity(trueCaret->getOpacity());

		smoothCaret->setContentSize(trueCaret->getContentSize());
		smoothCaret->setAnchorPoint({0.5f, 0.5f});
		smoothCaret->setPosition({2, -1});
		smoothCaret->setScale(trueCaret->getScale());
		

		smoothCaret->setID("smooth-caret"_spr);
		
		trueCaret->setOpacity(0); // so you cant see it but then i can check later if its visible, and then steal it >:3


		this->addChild(smoothCaret);
		this->schedule(schedule_selector(MyCCTextInputNode::updSmoothCaret));
		m_fields->m_smoothCaret = smoothCaret;

        return true;
    }

	void updSmoothCaret(float dt) {
		auto trueCaret = m_fields->m_trueCaret;
		auto smoothCaret = m_fields->m_smoothCaret;
		if (!trueCaret || !smoothCaret) return;

		CCPoint oldPos = smoothCaret->getPosition();
    	CCPoint targetPos = trueCaret->getPosition();

		float lerpSpeed = Mod::get()->getSettingValue<float>("Weight"); 
    
		float newX = std::lerp(smoothCaret->getPositionX(), trueCaret->getPositionX(), std::min(1.0f, dt * lerpSpeed));
		float newY = std::lerp(smoothCaret->getPositionY(), trueCaret->getPositionY(), std::min(1.0f, dt * lerpSpeed));

		float velocityX = (newX - oldPos.x) / dt;

		float stretchIntensity = 0.1; 
   		float stretch = 1.0f + std::abs(velocityX) * stretchIntensity;

		float baseScale = trueCaret->getScale();

		if (Mod::get()->getSettingValue<bool>("Stretch")) {
			smoothCaret->setScaleX(stretch * baseScale);
		}
		
	

		smoothCaret->setPosition(newX, newY);
		smoothCaret->setVisible(trueCaret->isVisible());

	}
};