namespace kick{
    template <typename E>
    inline EventListener<E>::EventListener(){
    };

    template <typename E>
    inline EventListener<E>::EventListener(Event<E> *e, int listenerId)
    :AbstractEventListener(e, listenerId)
    {
    }

    template <typename E>
    inline EventListener<E>::EventListener(EventListener&& other){
        listenerId = other.listenerId;
        ae = other.ae;
        other.ae = nullptr;
    }
    template <typename E>
    inline EventListener<E>& EventListener<E>::operator=( EventListener<E>&& rhs ) {
        if (this != &rhs){
            destroyListener();
            listenerId = rhs.listenerId;
            ae = rhs.ae;
            rhs.ae = nullptr;
        }
        return *this;
    };

    template <typename E>
    inline SyncValue<E>::SyncValue(){
    }

    template <typename E>
    inline E& SyncValue<E>::getValue(){
        return value;
    }

    template <typename E>
    inline void SyncValue<E>::setValue(const E& e){
        value = e;
    }

    template <typename E>
    inline SyncValue<E>::SyncValue(SyncValue&& other){
        listenerId = other.listenerId;
        ae = other.ae;
        value = std::move(other.value);
        other.ae = nullptr;
    }

    template <typename E>
    inline SyncValue<E>& SyncValue<E>::operator=( SyncValue<E>&& rhs ) {
        if (this != &rhs){
            destroyListener();
            listenerId = rhs.listenerId;
            ae = rhs.ae;
            value = std::move(rhs.value);
            rhs.ae = nullptr;
        }
        return *this;
    };

}