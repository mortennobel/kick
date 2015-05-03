//
// Created by morten on 18/01/15.
//

#include "toggle_button.h"
#include <algorithm>

namespace kick {

    ToggleButton::ToggleButton(GameObject *gameObject, std::shared_ptr<Canvas> canvas)
            :Button{gameObject, canvas}//, mGroup{new ToggleButtonGroup{}}
    {
        setGroup(std::make_shared<ToggleButtonGroup>());
    }

    ToggleButton::~ToggleButton() {
        setGroup(std::shared_ptr<ToggleButtonGroup>()); // clean up group
    }

    std::shared_ptr<ToggleButtonGroup> ToggleButton::group() {
        return mGroup;
    }

    void ToggleButton::setGroup(std::shared_ptr<ToggleButtonGroup> group) {
        if (mGroup != group){
            if (mGroup){
                auto pos = std::find(mGroup->buttons.begin(),mGroup->buttons.end(), this);
                if (pos != mGroup->buttons.end()){
                    mGroup->buttons.erase(pos);
                }
                if (mGroup->selectedButton == this){
                    mGroup->selectedButton = nullptr;
                }
            }
            mGroup = group;
            if (mGroup){
                mGroup->buttons.push_back(this);
                if (mGroup->buttonCount()==1){
                    mGroup->buttons[0]->setSelected(true);
                }
            }
        }
    }

    bool ToggleButton::selected() {
        return this == mGroup->selectedButton;
    }

    void ToggleButton::toggleSelected(){
        setSelected(!selected());
    }

    void ToggleButton::setSelected(bool selectedState) {
        if (selectedState){
            bool doToggle = mGroup->buttons.size() == 1;
            if (selected()) {
                return;
            }
            if (mGroup->selectedButton) {
                mGroup->selectedButton->setSelected(false);
            }
            mGroup->selectedButton = this;

        }
        if (!selectedState){
            if (!selected()){
                return;
            }
            mGroup->selectedButton = nullptr;
        }
        updateTextureAndTxtColor();
        mOnChange(this);
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
        if (mGroup->buttonCount()==1){
            toggleSelected();
        } else {
            setSelected(true);
        }
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
        Button::updateTextureAndTxtColor();
    }

    int ToggleButtonGroup::buttonCount() {
        return buttons.size();
    }
};