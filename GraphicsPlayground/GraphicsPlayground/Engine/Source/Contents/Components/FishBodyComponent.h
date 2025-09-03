#pragma once
#include "Engine/Classes/Components/StaticMeshComponent.h"

class UFishBodyComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UFishBodyComponent, UStaticMeshComponent)

public:
    UFishBodyComponent();
    virtual ~UFishBodyComponent() override = default;

    virtual void TickComponent(float DeltaTime) override;

    virtual void InitializeComponent() override;

protected:
};
