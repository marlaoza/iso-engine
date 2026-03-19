[[vk::binding(0, 2)]]
Texture2D textTexture : register(t0);
[[vk::binding(0, 2)]]
SamplerState textSampler : register(s0);

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
};

float4 main(PSInput input) : SV_Target {
    float4 mainColor = textTexture.Sample(textSampler, input.uv);
    float4 finalColor = mainColor * input.col;
    return finalColor;
}

