struct PSInput {
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
    float opacity : TEXCOORD1;
};

float4 main(PSInput input) : SV_Target {
    float dist = distance(input.uv, float2(0.5, 0.5));
    float alpha = (1.0 - smoothstep(0.0, 0.5, dist)) * input.opacity;
    return float4(1.0, 0.8, 0.2, alpha);
}