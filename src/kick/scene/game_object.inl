#include "transform.h"

namespace kick {
    typedef std::vector<std::unique_ptr<GameObject>>::const_iterator GameObjectIter;
    
    template <typename C, typename... T>
    inline std::shared_ptr<C> GameObject::addComponent(T... t){
        auto res = std::make_shared<C>(this, t...);
        mComponents.push_back(res);
        res->activated();
        componentEvent.notifyListeners({res, ComponentUpdateStatus::Created});
        return res;
    }

    template <typename C>
    inline std::shared_ptr<C> GameObject::component(){
        for (auto c : mComponents){
            auto comp = std::dynamic_pointer_cast<C>(c);
            if (comp){
                return comp;
            }
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<std::shared_ptr<C>> GameObject::components(){
        std::vector<std::shared_ptr<C>> res;
        for (auto c : mComponents){
            std::shared_ptr<C> comp = std::dynamic_pointer_cast<C>(c);
            if (comp){
                res.push_back(comp);
            }
        }
        return res;
    }

    template <typename C>
    inline std::shared_ptr<C> GameObject::componentInParent(){
        auto p = mTransform->parent();
        while (p){
            std::shared_ptr<C> c = p->gameObject()->component<C>();
            if (c){
                return c;
            }
            p = p->parent();
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<std::shared_ptr<C>> GameObject::componentsInParent(){
        std::vector<std::shared_ptr<C>> res;
        auto p = mTransform->parent();
        while (p){
            std::vector<std::shared_ptr<C>> c = p->gameObject()->components<std::shared_ptr<C>>();
            if (c.size()){
                res.insert(res.begin(), c.begin(), c.end());
            }
            p = p->parent();
        }
        return res;
    }

    template <typename C>
    inline std::shared_ptr<C> GameObject::componentInChildren(){
        auto t = mTransform;
        std::vector<std::shared_ptr<Transform>> q{t->begin(), t->end()};
        int index = 0;
        while (index < q.size()){
            t = q[index];
            auto c = t->gameObject()->component<C>();
            if (c){
                return c;
            }
            q.insert(q.end(), t->begin(), t->end());
            index++;
        }
        return nullptr;
    }

    template <typename C>
    inline std::vector<std::shared_ptr<C>> GameObject::componentsInChildren(){
        std::vector<std::shared_ptr<C>> res;
        auto t = mTransform;
        std::vector<std::shared_ptr<Transform>> q{t->begin(), t->end()};
        int index = 0;
        while (index < q.size()){
            t = q[index];
            std::shared_ptr<C> c = t->gameObject()->component<C>();
            if (c){
                res.push_back(c);
            }
            q.insert(q.end(), t->begin(), t->end());
            index++;
        }
        return res;
    }

}