#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include "TrafficLight.h"
#include <iostream>

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // Using std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pulling them from the queue using move semantics. 
    std::unique_lock<std::mutex> mLock(_mutex);
    _cond.wait(mLock, [this] {return !_queue.empty(); });
    
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // Using std::lock_guard<std::mutex> as well as _condition.notify_one() to add a new message to the queue 
    std::lock_guard<std::mutex> mLock(_mutex);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */
 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        auto msg = _mqueue.receive();
        if (msg == TrafficLightPhase::green) {
            return;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase newPhase)
{
  _currentPhase = newPhase;
}

void TrafficLight::simulate()
{  
  threads.emplace_back([this](){
    cycleThroughPhases();
  });
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // toggle the current phase of the traffic light between red and green and send an update
    // to the message queue using move semantics. The cycle duration is a random value between 4 and 6 seconds. 
    // Using std::this_thread::sleep_for to wait 1ms between two cycles. 
    while (true) {
      int sleep = (rand() % (6000 + 1 - 4000)) + 4000;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
      
      if (getCurrentPhase()==TrafficLightPhase::red){
        setCurrentPhase(TrafficLightPhase::green);
      }
      else {
        setCurrentPhase(TrafficLightPhase::red);
      }
      
      // sends an update method to the message queue using move semantics.
      _mqueue.send(std::move(getCurrentPhase()));
      
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
