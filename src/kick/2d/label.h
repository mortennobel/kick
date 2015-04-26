//
// Created by morten on 30/06/14.
//


#pragma once

#include "kick/2d/component2d.h"
#include "kick/scene/game_object.h"
#include "kick/2d/font.h"
#include "kick/math/bounds2.h"
#include "kick/core/event_listener.h"


namespace kick {
    class Font;
    class Mesh;

    class Label : public Component2D {
    public:
        Label(GameObject *gameObject);
        std::shared_ptr<Font> font() const;
        void setFont(std::shared_ptr<Font>& font);
        virtual void render(EngineUniforms *engineUniforms);
        std::string const &text() const;
        void setText(std::string const &text);
        virtual Bounds2 bounds() const override;
        virtual Shader*shader() const override;
        Material *material() const;
        void setMaterial(Material *material);

        virtual int renderOrder();

        virtual void setBounds(Bounds2 bounds);

        glm::vec2 anchor() const;
        void setAnchor(glm::vec2 anchor);
    private:
        void updateVertexBuffer();

        EventListener<Font*> mEventListener;
        Bounds2 mBounds;
        std::shared_ptr<Font> mFont = nullptr;
        Mesh *mMesh = nullptr;
        std::shared_ptr<MeshData> mMeshData;
        std::string mText;
        glm::vec2 mAnchor{0,0};
        Material*mMaterial = nullptr;
    };
}


