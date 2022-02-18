cbuffer Cbuf
{
    matrix position_transform;
	float4 color;
};

struct VSOut
{
    float3 pos : Position;
    float4 color : COLOR;
};

struct GeoOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

struct VSIn
{
    float3 pos : Position;
    float4 color : COLOR;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = IN.pos;
    OUT.color = normalize(IN.color);
    return OUT;
}

GeoOut CreateQuadVertex(VSOut data, float2 offset)
{
    GeoOut o = (GeoOut) 0;
    float3 vert = data.pos;
    vert.xz += offset;
    o.pos = mul(position_transform, float4(vert, 1.0f));
    o.color = data.color;
    return o;
}

[maxvertexcount(4)]
void GSMain(point VSOut IN[1], inout TriangleStream<GeoOut> tristream)
{
    tristream.Append(CreateQuadVertex(IN[0], float2(1, -1)));
    tristream.Append(CreateQuadVertex(IN[0], float2(1, 1)));
    tristream.Append(CreateQuadVertex(IN[0], float2(-1, -1)));
    tristream.Append(CreateQuadVertex(IN[0], float2(-1, 1)));
    tristream.RestartStrip();

}

float4 PSMain(GeoOut IN) : SV_Target
{
    float4 result;

#ifdef DEFAULT
	result = IN.color;
#endif

    return result;
}