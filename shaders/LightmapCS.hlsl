#define HLSL

#include "shader_defines.h"

RWTexture2D<unorm float4> output : register(u0);

[numthreads(2, 2 , 1)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID)
{
    output[dispatchThreadID.xy] = (unorm float4)float4(1.0f, 0.0, 0.0, 0.0);
}