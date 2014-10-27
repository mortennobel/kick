//
// Created by Morten Nobel-Jørgensen on 27/10/14.
//

#include "event_queue.h"

#include <cassert>
#include "time.h"

using namespace std;

namespace kick{
    int EventQueue::scheduleEvent(std::function<void()> &&fn, float timeStart, float timeEnd) {
        assert(timeEnd >= timeStart);
        float time = Time::getTime();
        int id = ++queueCounter;
        if (updating){
            updatingQueue.emplace_back(std::move(fn), time+timeStart, time+timeEnd,id);
        } else {
            queue.emplace_back(std::move(fn), time+timeStart, time+timeEnd,id);
        }
        return id;
    }

    bool EventQueue::cancelEvent(int eventid) {
        auto res = std::remove_if(queue.begin(), queue.end(), [&](EventQueueEntry& e){
            return e.eventid == eventid;
        });
        queue.erase(res, queue.end());
        return res != queue.begin();
    }

    void EventQueue::run() {
        updating = true;
        float time = Time::getTime();

        for (auto e : queue){
            if (e.timeStart < time){
                e.fn();
            }
        }
        auto erase = std::remove_if(queue.begin(), queue.end(), [&](EventQueueEntry& e){
            return e.timeEnd < time;
        });
        queue.erase(erase, queue.end());
        updating = false;

        for (auto e : updatingQueue){
            queue.emplace_back(std::move(e.fn), e.timeStart, e.timeEnd,e.eventid);
        }
        updatingQueue.clear();
    }

    EventQueueEntry::EventQueueEntry(std::function<void()>&& fn,float timeStart,float timeEnd,int eventid)
    : fn(fn), timeStart(timeStart), timeEnd(timeEnd), eventid(eventid) {
    }
}