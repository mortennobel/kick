//
//  Event.h
//  TmpProject
//
//  Created by morten on 8/18/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#pragma once

#include <iostream>
#include <functional>
#include "event_listener.h"
#include <vector>
#include <utility>

#define GET_LISTENER_FN(x) std::get<0>(x)
#define GET_LISTENER_ID(x) std::get<1>(x)
#define GET_LISTENER_SORTKEY(x) std::get<2>(x)

namespace kick {
    class AbstractEvent{
    public:
        virtual bool removeListener(int id) = 0;
    };
    
    template <typename E>
    class Event : public AbstractEvent{
    public:
        EventListener<E> createListener(std::function<void (E)> listener, int sortPriority = 0);
        
        void registerSyncValue(SyncValue<E>& syncValue);
        
        void notifyListeners(E e);
        
        bool removeListener(int id);
    protected:
        std::vector<std::tuple<std::function< void (E)>, int, int>> listeners;
        friend class EventListener<E>;
        int eventListenerId = 0;
    };
}

#include "event.inl"
