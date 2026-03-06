struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
};

float4 main(PSInput input) : SV_Target {
    return float4(1.0, 0.0, 0.0, 1.0);
}