//
//  key_input.cpp
//  UnitTest_KickCPP
//
//  Created by morten on 24/04/14.
//
//

#include "key_input.h"

namespace kick {
    KeyInput::KeyInput(){}
    
    void KeyInput::reset(){
        buttonDown.clear();
        buttonUp.clear();
    }

    bool KeyInput::down(Key button) const {
        return buttonDown.find(button) != buttonDown.end();
    }

    bool KeyInput::anyDown() const {
        return !buttonDown.empty();
    }

    bool KeyInput::pressed(Key button) const {
        return buttonPressed.find(button) != buttonPressed.end();
    }

    bool KeyInput::anyPressed() const {
        return !buttonPressed.empty();
    }

    bool KeyInput::up(Key button) const {
        return buttonUp.find(button) != buttonUp.end();
    }

    bool KeyInput::anyUp() const {
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
            : keys{keys} {

    }

    KeyMapping::KeyMapping(std::initializer_list<KeyMapping> alternatives)
    {
        for (auto & a : alternatives){
            this->alternatives.push_back(a);
        }
    }

    bool KeyInput::pressed(const KeyMapping &mapping) const {
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

    bool KeyInput::down(const KeyMapping &mapping) const {
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


}