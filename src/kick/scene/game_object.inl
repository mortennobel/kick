namespace kick {
    typedef std::vector<std::unique_ptr<GameObject>>::const_iterator GameObjectIter;
    
    template <typename C>
    inline C *GameObject::addComponent(){
        auto res = new C(this);
        components.push_back(res);
        res->activated();
        fireComponentEvent(res, ComponentUpdateStatus::Added);
        return res;
    }
}