struct PSInput {
    float4 pos : SV_POSITION;
    nointerpolation int2 gridPos : TEXCOORD1;
    float4 col : COLOR0;
    float faceID : TEXCOORD0;
    float2 screenPos : TEXCOORD2;
    float2 uv: TEXCOORD4;
};

float4 main(PSInput input) : SV_Target {
    float4 baseColor = input.col;
    float shadowMult = 1.0;
    float3 color = baseColor.rgb;

    if (input.faceID > 0.5 && input.faceID < 1.1) shadowMult = 0.8;
    if (input.faceID > 1.2 && input.faceID < 2.1) shadowMult = 0.5;

    return float4(color * shadowMult, baseColor.a);
}