//
// Created by morten on 19/07/14.
//


#pragma once
#include "kick/2d/button.h"

namespace kick {

    class ToggleButton;

    class ToggleButtonGroup {
        std::vector<ToggleButton*> buttons;
    public:
        ToggleButton *selectedButton = nullptr;
        int buttonCount();
        friend class ToggleButton;
    };

    class ToggleButton : public Button {
        std::shared_ptr<ToggleButtonGroup> mGroup;
        std::string mSelectedSprite;
    public:
        ToggleButton(GameObject *gameObject, std::shared_ptr<Canvas> canvas);
        virtual ~ToggleButton();
        std::shared_ptr<ToggleButtonGroup> group();

        void setGroup(std::shared_ptr<ToggleButtonGroup> group);

        bool selected();

        void setSelected(bool selectedState);
        void toggleSelected();

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
