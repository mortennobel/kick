//
// Created by Morten Nobel-Jørgensen on 27/10/14.
//


#pragma once

#include <functional>
#include <vector>

namespace kick {

    struct EventQueueEntry {
        std::function<void(int)> fn;
        float timeStart;
        float timeEnd;
        int eventid;
        bool cancelled = false;
        EventQueueEntry(std::function<void(int)>&& fn, float timeStart, float timeEnd, int eventid);
    };

    class EventQueue {
    public:
        EventQueue(){}
        // schedule future event
        // timeStart and timeEnd relative to starting time (Time::total())
        // each event will at least be executed once (if timeStart is less than Time::total() and not cancelled)
        int scheduleEvent(std::function<void(int)>&& fn, float timeStart = 0, float timeEnd = 0);
        bool cancelEvent(int eventid);
        void run();
    private:
        std::vector<EventQueueEntry> queue;
        std::vector<EventQueueEntry> updatingQueue;
        int queueCounter = 0;
        bool updating = false;
        EventQueue(const EventQueue&) = delete;
        EventQueue(EventQueue&&) = delete;
    };
}


