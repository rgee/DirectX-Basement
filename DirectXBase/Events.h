#ifndef EVENTS
#define EVENTS

#include "EventManager.h"

typedef unsigned int ActorId;
struct EvtData_Destroy_Actor : public BaseEventData
{
    static const EventType evtType;
    virtual const EventType& GetEventType() const { return evtType; }
    
    explicit EvtData_Destroy_Actor(ActorId id)
        : m_id(id) {}
    explicit EvtData_Destroy_Actor(std::istream& in)
    { in >> m_id; }

    virtual ~EvtData_Destroy_Actor() { }

    virtual void Serialize(std::ostream& out) const
    { out << m_id; }

    ActorId m_id;
};

const EventType EvtData_Destroy_Actor::evtType("destroy_actor");

#endif