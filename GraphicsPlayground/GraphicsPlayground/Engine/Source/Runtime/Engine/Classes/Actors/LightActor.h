#pragma once
#include "Classes/GameFramework/Actor.h"

class UDirectionalLightComponent;
class UPointLightComponent;
class USpotLightComponent;
class UDirectionalLightComponent;
class UAmbientLightComponent;
class UBillboardComponent;

class ALight :public AActor
{
    DECLARE_CLASS(ALight, AActor)
public:
    ALight();
    virtual ~ALight();
protected:

};
