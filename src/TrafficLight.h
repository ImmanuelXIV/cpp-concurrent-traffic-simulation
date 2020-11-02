#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;
enum class TrafficLightPhase{red, green};


template <class T>
class MessageQueue
{
public:
    void send (T &&msg);
    T receive();

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};


class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(TrafficLightPhase newPhase);

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _mqueue;
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
};

#endif