namespace kick{
    template <typename C, typename... T>
    inline std::shared_ptr<C> Canvas::addComponent(T... t){
        auto go = createGameObject();
        go->transform()->setParent(transform());
        auto c = go->addComponent<C>(std::dynamic_pointer_cast<Canvas>(shared_from_this()), t...);

        registerComponent2D(c);

        return c;
    }
}