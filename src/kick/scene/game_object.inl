#include "transform.h"

namespace kick {
    typedef std::vector<std::unique_ptr<GameObject>>::const_iterator GameObjectIter;
    
    template <typename C, typename... T>
    inline C *GameObject::addComponent(T... t){
        auto res = new C(this, t...);
        mComponents.push_back(res);
        res->activated();
        componentEvent.notifyListeners({res, ComponentUpdateStatus::Created});
        return res;
    }

    template <typename C>
    inline C* GameObject::component(){
        for (auto c : mComponents){
            C* comp = dynamic_cast<C*>(c);
            if (comp){
                return comp;
            }
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<C*> GameObject::components(){
        std::vector<C*> res;
        for (auto c : mComponents){
            C* comp = dynamic_cast<C*>(c);
            if (comp){
                res.push_back(comp);
            }
        }
        return res;
    }

    template <typename C>
    inline C* GameObject::componentInParent(){
        Transform* p = mTransform->parent();
        while (p){
            C* c = p->gameObject()->component<C>();
            if (c){
                return c;
            }
            p = p->parent();
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<C*> GameObject::componentsInParent(){
        std::vector<C*> res;
        Transform* p = mTransform->parent();
        while (p){
            std::vector<C*> c = p->gameObject()->components<C>();
            if (c.size()){
                res.insert(res.begin(), c.begin(), c.end());
            }
            p = p->parent();
        }
        return res;
    }

    template <typename C>
    inline C* GameObject::componentInChildren(){
        Transform *t = mTransform;
        std::vector<Transform*> q{t->begin(), t->end()};
        int index = 0;
        while (index < q.size()){
            t = q[index];
            C* c = t->gameObject()->component<C>();
            if (c){
                return c;
            }
            q.insert(q.end(), t->begin(), t->end());
            index++;
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<C*> GameObject::componentsInChildren(){
        std::vector<C*> res;
        Transform *t = mTransform;
        std::vector<Transform*> q{t->begin(), t->end()};
        int index = 0;
        while (index < q.size()){
            t = q[index];
            C* c = t->gameObject()->component<C>();
            if (c){
                res.push_back(c);
            }
            q.insert(q.end(), t->begin(), t->end());
            index++;
        }
        return res;
    }

}