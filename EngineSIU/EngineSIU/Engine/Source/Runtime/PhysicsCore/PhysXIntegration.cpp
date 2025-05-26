#include "PhysXIntegration.h"

inline void MySimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    PxRigidActor* actorA = pairHeader.actors[0];
    PxRigidActor* actorB = pairHeader.actors[1];

    void* dataA = actorA->userData;
    void* dataB = actorB->userData;

    for (PxU32 i = 0; i < nbPairs; ++i)
    {
        const PxContactPair& cp = pairs[i];
        // cp.shapes[0], cp.contactCount, cp.flags, etc...
    }
}
