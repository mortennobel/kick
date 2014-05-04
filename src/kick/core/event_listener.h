//
//  EventListener.h
//  TmpProject
//
//  Created by morten on 8/18/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#pragma once

#include <iostream>

namespace kick {
    class AbstractEvent;
    
    class AbstractEventListener{
    public:
        AbstractEventListener(){};
        AbstractEventListener(AbstractEvent *ae, int listenerId);
        AbstractEventListener(const AbstractEventListener&) = delete;
        AbstractEventListener& operator=( const AbstractEventListener& rhs ) = delete;
        
        virtual ~AbstractEventListener();
        friend class AbstractEvent;
    protected:
        void destroyListener();
        int listenerId = -1;
        AbstractEvent *ae = nullptr;
    };
    
    template <typename E>
    class Event;
    
    template <typename E>
    class EventListener : public AbstractEventListener {
    public:
        EventListener();

        EventListener(Event<E> *e, int listenerId);
        
        EventListener(const EventListener&) = delete;
        EventListener& operator=( const EventListener& rhs ) = delete;

        EventListener(EventListener&& other);
        EventListener& operator=( EventListener&& rhs );
        friend class Event<E>;
    };
    
    template <typename E>
    class SyncValue : public AbstractEventListener {
    public:
        SyncValue();
        
        E& getValue();
        
        void setValue(const E& e);
        
        SyncValue(const SyncValue&) = delete;
        SyncValue& operator=( const SyncValue& rhs ) = delete;
        
        SyncValue(SyncValue&& other);
        
        SyncValue& operator=( SyncValue&& rhs );
        friend class Event<E>;
    private:
        E value;
    };
}

#include "event_listener.inl"
