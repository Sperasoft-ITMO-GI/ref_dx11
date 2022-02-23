cbuffer Cbuf
{
    matrix view;
    matrix projection;
};

struct VSIn
{
    float3 pos : Position;
    float4 color : COLOR;
};

struct VSOut
{
    float4 pos : Position;
    float4 color : COLOR;
};

struct GeoOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VSOut VSMain(VSIn IN)
{
    VSOut OUT;
    OUT.pos = mul(view, float4(IN.pos, 1.0f));
    OUT.color = normalize(IN.color);
    return OUT;
}

GeoOut CreateQuadVertex(VSOut data, float2 offset)
{
    GeoOut o = (GeoOut) 0;
    float4 vert = data.pos;
    vert.xy += offset;
    o.pos = mul(projection, vert);
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