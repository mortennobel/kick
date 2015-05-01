//
// Created by morten on 18/01/15.
//

#include "toggle_button.h"


namespace kick {

    ToggleButton::ToggleButton(GameObject *gameObject, std::shared_ptr<Canvas> canvas)
            :Button{gameObject, canvas}, mGroup{new ToggleButtonGroup{}} {
        mGroup->buttons.push_back(this);

    }

    std::shared_ptr<ToggleButtonGroup> ToggleButton::group() {
        return mGroup;
    }

    void ToggleButton::setGroup(std::shared_ptr<ToggleButtonGroup> group) {
        if (mGroup != group){
            auto pos = std::find(mGroup->buttons.begin(),mGroup->buttons.end(), this);
            if (pos != mGroup->buttons.end()){
                mGroup->buttons.erase(pos);
            }
            if (mGroup->selectedButton == this){
                mGroup->selectedButton = nullptr;
            }
            mGroup = group;
            mGroup->buttons.push_back(this);
        }
    }

    bool ToggleButton::selected() {
        return this == mGroup->selectedButton;
    }

    void ToggleButton::select() {
        bool doToggle = mGroup->buttons.size() == 1;
        if (doToggle && selected()) {
            deselect();
        } else {
            if (selected()) {
                return;
            }
            if (mGroup->selectedButton) {
                mGroup->selectedButton->deselect();
            }
            mGroup->selectedButton = this;
        }
        updateTextureAndTxtColor();
    }

    void ToggleButton::deselect() {
        if (!selected()){
            return;
        }
        mGroup->selectedButton = nullptr;
        updateTextureAndTxtColor();
    }

    void ToggleButton::updateTextureAndTxtColor() {
        if (selected()){
            setSpriteName(mSelectedSprite);
            mTextComponent->material()->setUniform("mainColor", mPressedColor);
        } else {
            Button::updateTextureAndTxtColor();
        }
    }

    void ToggleButton::invokeClick() {
        select();
        Button::invokeClick();
    }

    std::function<void(ToggleButton*)> const &ToggleButton::onChange() const {
        return mOnChange;
    }

    void ToggleButton::setOnChange(std::function<void(ToggleButton*)> const &onChange) {
        this->mOnChange = onChange;
    }

    std::string ToggleButton::selectedSprite() const {
        return mSelectedSprite;
    }

    void ToggleButton::setSelectedSprite(std::string const &selectedSprite) {
        mSelectedSprite = selectedSprite;
    }
};