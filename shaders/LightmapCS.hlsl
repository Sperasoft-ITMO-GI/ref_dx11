#define HLSL

#include "shader_defines.h"

RWTexture2D<unorm float4> output : register(u0);

//[numthreads(8, 8 , 1)]
//void CSMain(int3 id : SV_DispatchThreadID)
//{
//    if ((id.x >= (int) (0.64 * 128) && id.x <= (int) (0.69 * 128)) && (id.y >= (int) (0.66 * 128) && id.y <= (int) (0.68 * 128)))
//    {
//        output[id.xy] = (unorm float4)float4(0.0f, 1.0, 0.0, 0.0);
//    }   
//}

[numthreads(2, 2, 1)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID)
{
    output[dispatchThreadID.xy] = (unorm float4)float4(1.0f, 0.0, 0.0, 0.0);
}