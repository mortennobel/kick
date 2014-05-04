namespace kick{
template <typename E>
inline EventListener<E> Event<E>::createListener(std::function<void (E)> listener, int sortPriority){
    auto insertPos = listeners.begin();
    int v = 0;
    while (insertPos != listeners.end() && GET_LISTENER_SORTKEY(*insertPos) < sortPriority){
        insertPos++;
        v++;
    }
    listeners.emplace(insertPos, listener, eventListenerId, sortPriority);
    
    eventListenerId++;
    return EventListener<E>(this, eventListenerId-1);
}

template <typename E>
inline void Event<E>::registerSyncValue(SyncValue<E>& syncValue){
    if (syncValue.listenerId > -1){
        removeListener(syncValue.listenerId);
    }
    std::function<void (E)> updateFunction = [&](E e){
        syncValue.value = e;
    };
    listeners.emplace_back(updateFunction, eventListenerId, 0);
    syncValue.listenerId = eventListenerId;
    syncValue.ae = this;
    eventListenerId++;
}

template <typename E>
inline void Event<E>::notifyListeners(E e){
    for (auto & l : listeners){
        auto& func = GET_LISTENER_FN(l);
        func(e);
    }
}

template <typename E>
inline bool Event<E>::removeListener(int id){
    auto iter = listeners.begin();
    while (iter != listeners.end()){
        if (GET_LISTENER_ID(*iter) == id){
            listeners.erase(iter);
            return true;
        }
        iter++;
    }
    return false;
}
}