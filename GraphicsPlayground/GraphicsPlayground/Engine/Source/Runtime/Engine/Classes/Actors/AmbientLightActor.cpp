#include "AmbientLightActor.h"
#include "Classes/Components/Light/AmbientLightComponent.h"
#include "Classes/Components/BillboardComponent.h"

AAmbientLight::AAmbientLight()
{
    AmbientLightComponent = AddComponent<UAmbientLightComponent>("UAmbientLightComponent_0");
    BillboardComponent = AddComponent<UBillboardComponent>("UBillboardComponent_0");

    RootComponent = BillboardComponent;

    BillboardComponent->SetTexture(L"Assets/Editor/Icon/AmbientLight_64x.png");
    BillboardComponent->bIsEditorBillboard = true;

    AmbientLightComponent->AttachToComponent(RootComponent);

}
