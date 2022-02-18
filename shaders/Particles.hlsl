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
    o.pos.xy += offset;
    o.pos = mul(position_transform, o.pos);
    return o;
}

[maxvertexcount(4)]
void GSMain(point VSOut IN[1], inout TriangleStream<GeoOut> tristream)
{
    //tristream.Append(CreateQuadVertex(IN[0], float2(1, -1)));
    //tristream.Append(CreateQuadVertex(IN[0], float2(1, 1)));
    //tristream.Append(CreateQuadVertex(IN[0], float2(-1, -1)));
    //tristream.Append(CreateQuadVertex(IN[0], float2(-1, 1)));
    float3 v[4];
    v[0] = float3(IN[0].pos + float3(1.0f, 0.0f, -1.0f));
    v[1] = float3(IN[0].pos + float3(1.0f, 0.0f, 1.0f));
    v[2] = float3(IN[0].pos + float3(-1.0f, 0.0f, -1.0f));
    v[3] = float3(IN[0].pos + float3(-1.0f, 0.0f, 1.0f));
    
    
    GeoOut OUT = (GeoOut) 0;
    OUT.pos = mul(position_transform, float4(v[0], 1.0f));
    OUT.color = IN[0].color;
    tristream.Append(OUT);
    OUT.pos = mul(position_transform, float4(v[1], 1.0f));
    tristream.Append(OUT);
    OUT.pos = mul(position_transform, float4(v[2], 1.0f));
    tristream.Append(OUT);
    OUT.pos = mul(position_transform, float4(v[3], 1.0f));
    tristream.Append(OUT);
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