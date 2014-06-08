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
    std::vector<C*> GameObject::getComponents(){
        std::vector<C*> res;
        for (auto c : components){
            C* comp = dynamic_cast<C*>(c);
            if (comp){
                res.push_back(comp);
            }
        }
        return res;
    }
}