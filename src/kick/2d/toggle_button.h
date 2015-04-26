//
// Created by morten on 19/07/14.
//


#pragma once
#include "kick/2d/button.h"

namespace kick {

    class ToggleButton;

    class ToggleButtonGroup {
    public:
        ToggleButton *selectedButton = nullptr;
        std::vector<ToggleButton*> buttons;
    };

    class ToggleButton : public Button {
        std::shared_ptr<ToggleButtonGroup> mGroup;
        std::string mSelectedSprite;
    public:
        ToggleButton(GameObject *gameObject);

        std::shared_ptr<ToggleButtonGroup> group();

        void setGroup(std::shared_ptr<ToggleButtonGroup> group);

        bool selected();

        void select();

        void deselect();

        std::string selectedSprite() const;

        void setSelectedSprite(std::string const &selectedSprite);

        std::function<void(ToggleButton*)> const &onChange() const;
        void setOnChange(std::function<void(ToggleButton*)> const &onChange);
    protected:
        virtual void updateTextureAndTxtColor();

        virtual void invokeClick() override;
        std::function<void(ToggleButton*)> mOnChange = [](Button*){};
    };
}
