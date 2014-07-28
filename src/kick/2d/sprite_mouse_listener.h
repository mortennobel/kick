//
// Created by morten on 27/07/14.
//


#pragma once

namespace kick{
    class SpriteMouseListener {
    public:
        virtual void down(int button) {}

        virtual void pressed(int button) {}

        virtual void up(int button) {}

        virtual void over() {}

        virtual void out() {}
    };
}


