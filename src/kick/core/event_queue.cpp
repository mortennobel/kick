//
// Created by Morten Nobel-Jørgensen on 27/10/14.
//

#include "event_queue.h"

#include <cassert>
#include "time.h"

using namespace std;

namespace kick{
    int EventQueue::scheduleEvent(std::function<void(int)> &&fn, float timeStart, float timeEnd) {
        assert(timeEnd >= timeStart);
        float time = Time::total();
        int id = ++queueCounter;
        if (updating){
            updatingQueue.emplace_back(std::move(fn), time+timeStart, time+timeEnd,id);
        } else {
            queue.emplace_back(std::move(fn), time+timeStart, time+timeEnd,id);
        }
        return id;
    }

    bool EventQueue::cancelEvent(int eventid) {
        auto found = std::find_if(queue.begin(), queue.end(), [&](EventQueueEntry& e){
            return e.eventid == eventid;
        });
        if (found != queue.end()){
            found->cancelled = true;
            return true;
        }
        return false;
    }

    void EventQueue::run() {
        updating = true;
        float time = Time::total();

        for (auto e : queue){
            if (e.timeStart < time && !e.cancelled){
                e.fn(e.eventid);
            }
        }
        auto erase = std::remove_if(queue.begin(), queue.end(), [&](EventQueueEntry& e){
            return e.timeEnd < time || e.cancelled;
        });
        queue.erase(erase, queue.end());
        updating = false;

        for (auto e : updatingQueue){
            queue.emplace_back(std::move(e.fn), e.timeStart, e.timeEnd,e.eventid);
        }
        updatingQueue.clear();
    }

    EventQueueEntry::EventQueueEntry(std::function<void(int)>&& fn,float timeStart,float timeEnd,int eventid)
    : fn(fn), timeStart(timeStart), timeEnd(timeEnd), eventid(eventid) {
    }
}