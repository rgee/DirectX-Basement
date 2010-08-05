#ifndef EVENTMANAGER
#define EVENTMANAGER

#include "HashedString.h"
#include <boost/shared_ptr.hpp>

typedef HashedString EventType;

/*
 * Base class for event data
 */
class IEventData
{
public:
    virtual const EventType& GetEventType() const = 0;
    virtual float GetTimeStamp() const = 0;
    virtual void Serialize(std::ostream &out) const = 0;
};

typedef boost:shared_ptr<IEventData> IEventDataPtr;

class BaseEventData : public IEventData
{
public:
    explicit BaseEventData(const float ptimeStamp = 0.0f)
        : timeStamp(ptimeStamp) {}
    virtual ~BaseEventData() {}
    virtual const EventType& GetEventType() const = 0;
    float GetTimeStamp() const { return timeStamp; }
    virtual void Serialize(std::ostream &out) const {}
protected:
    const float timeStamp;
}

#endif