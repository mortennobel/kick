#include "transform.h"

namespace kick {
    typedef std::vector<std::unique_ptr<GameObject>>::const_iterator GameObjectIter;
    
    template <typename C>
    inline C *GameObject::addComponent(){
        auto res = new C(this);
        components.push_back(res);
        res->activated();
        componentEvent.notifyListeners({res, ComponentUpdateStatus::Created});
        return res;
    }

    template <typename C>
    inline C* GameObject::getComponent(){
        for (auto c : components){
            C* comp = dynamic_cast<C*>(c);
            if (comp){
                return comp;
            }
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<C*> GameObject::getComponents(){
        std::vector<C*> res;
        for (auto c : components){
            C* comp = dynamic_cast<C*>(c);
            if (comp){
                res.push_back(comp);
            }
        }
        return res;
    }

    template <typename C>
    inline C* GameObject::getComponentInParent(){
        Transform* p = transform->getParent();
        while (p){
            C* c = p->getGameObject()->getComponent<C>();
            if (c){
                return c;
            }
            p = p->getParent();
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<C*> GameObject::getComponentsInParent(){
        std::vector<C*> res;
        Transform* p = transform->getParent();
        while (p){
            std::vector<C*> c = p->getGameObject()->getComponents<C>();
            if (c.size()){
                res.insert(res.begin(), c.begin(), c.end());
            }
            p = p->getParent();
        }
        return res;
    }

    template <typename C>
    inline C* GameObject::getComponentInChildren(){
        Transform *t = transform;
        std::vector<Transform*> q{t->begin(), t->end()};
        int index = 0;
        while (index < q.size()){
            t = q[index];
            C* c = t->getGameObject()->getComponent<C>();
            if (c){
                return c;
            }
            q.insert(q.end(), t->begin(), t->end());
            index++;
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<C*> GameObject::getComponentsInChildren(){
        std::vector<C*> res;
        Transform *t = transform;
        std::vector<Transform*> q{t->begin(), t->end()};
        int index = 0;
        while (index < q.size()){
            t = q[index];
            C* c = t->getGameObject()->getComponent<C>();
            if (c){
                res.push_back(c);
            }
            q.insert(q.end(), t->begin(), t->end());
            index++;
        }
        return res;
    }

}