#include <d3d11.h>
#include "RenderMisc.h"

struct FLinePrimitiveBatchArgs
{
    FGridParameters GridParam;
    ID3D11Buffer* VertexBuffer;
    int BoundingBoxCount;
    int ConeCount;
    int ConeSegmentCount;
    int OBBCount;
};