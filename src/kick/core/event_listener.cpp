//
//  EventListener.cpp
//  TmpProject
//
//  Created by morten on 8/18/13.
//  Copyright (c) 2013 morten. All rights reserved.
//

#include "kick/core/event_listener.h"
#include "kick/core/event.h"

namespace kick {
    
    AbstractEventListener::AbstractEventListener(AbstractEvent *ae, int listenerId)
    :ae(ae), listenerId(listenerId){
    }
    
    AbstractEventListener::~AbstractEventListener(){
        destroyListener();
    }
    
    void AbstractEventListener::destroyListener(){
        if (ae){
            ae->removeListener(listenerId);
        }
    }
}