[[vk::binding(0, 2)]]
Texture2DArray unitTextures : register(t0);
[[vk::binding(0, 2)]]
SamplerState unitSampler : register(s0);

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int direction: TEXCOORD2;
    int state: TEXCOORD3;
    int frame: TEXCOORD4;
};

float4 main(PSInput input) : SV_Target {
    float2 texturePos = float2((float)input.direction * 18.0, ((float)(input.state)*3 + (float)input.frame) * 48.0);

    float atlasW = 144.0;
    float atlasH = 144.0;

    float2 pixelPos = float2(
        texturePos.x + (input.uv.x * 18.0),
        texturePos.y + (input.uv.y * 48.0)
    );

    float2 normalizedUV = float2(pixelPos.x / atlasW, pixelPos.y / atlasH);

    float4 texColor = unitTextures.Sample(unitSampler, float3(normalizedUV, (float)input.texIndex));

    if (texColor.a < 0.1) {
        discard;
    }

    return texColor;

}