//
//  key_input.cpp
//  UnitTest_KickCPP
//
//  Created by morten on 24/04/14.
//
//

#include "key_input.h"

namespace kick {
    std::set<Key> KeyInput::buttonDown;
    std::set<Key> KeyInput::buttonPressed;
    std::set<Key> KeyInput::buttonUp;

    void KeyInput::reset(){
        buttonDown.clear();
        buttonUp.clear();
    }

    bool KeyInput::down(Key button) {
        return buttonDown.find(button) != buttonDown.end();
    }

    bool KeyInput::anyDown() {
        return !buttonDown.empty();
    }

    bool KeyInput::pressed(Key button) {
        return buttonPressed.find(button) != buttonPressed.end();
    }

    bool KeyInput::anyPressed() {
        return !buttonPressed.empty();
    }

    bool KeyInput::up(Key button) {
        return buttonUp.find(button) != buttonUp.end();
    }

    bool KeyInput::anyUp() {
        return !buttonUp.empty();
    }

    void KeyInput::pressBegin(Key k) {
        buttonDown.insert(k);
        buttonPressed.insert(k);
    }

    void KeyInput::pressEnd(Key k) {
        buttonPressed.erase(k);
        buttonUp.insert(k);
    }

    KeyMapping::KeyMapping(std::initializer_list<Key> keys)
            : keys(keys) {

    }

    KeyMapping::KeyMapping(std::initializer_list<KeyMapping> alternatives)
    {
        for (auto & a : alternatives){
            this->alternatives.push_back(a);
        }
    }

    bool KeyInput::pressed(const KeyMapping &mapping) {
        if (mapping.alternatives.size()>0){
            for (auto & k : mapping.alternatives){
                bool res = pressed(k);
                if (res){
                    return true;
                }
            }
            return false;
        }
        for (auto & k : mapping.keys){
            if (!pressed(k)){
                return false;
            }
        }
        return true;
    }

    bool KeyInput::down(const KeyMapping &mapping) {
        if (mapping.alternatives.size()>0){
            for (auto & k : mapping.alternatives){
                bool res = down(k);
                if (res){
                    return true;
                }
            }
            return false;
        }
        if (!pressed(mapping)){
            return false;
        }
        for (auto & k : mapping.keys){
            if (down(k)){
                return true;
            }
        }
        return false;
    }


    const std::set<Key> &KeyInput::buttonsDown() { return buttonDown;}

    const std::set<Key> &KeyInput::buttonsPressed() { return buttonPressed;}

    const std::set<Key> &KeyInput::buttonsUp() {return buttonUp;}
}