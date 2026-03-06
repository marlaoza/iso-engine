[[vk::binding(0, 2)]]
Texture2D uiAtlas : register(t0);

[[vk::binding(1, 2)]]
SamplerState uiSampler : register(s0);

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
    float2 texturePos : TEXCOORD1;
    float2 textureSize: TEXCOORD2;
};

float4 main(PSInput input) : SV_Target {
    float4 baseColor = input.col;

    float atlasW = 320.0;
    float atlasH = 320.0;

    float2 atlasUV = float2(
        (input.texturePos.x + input.uv.x * input.textureSize.x) / atlasW,
        (input.texturePos.y + input.uv.y * input.textureSize.y) / atlasH
    );

    float4 texColor = uiAtlas.Sample(uiSampler, atlasUV);
    return texColor * baseColor.a;
}

