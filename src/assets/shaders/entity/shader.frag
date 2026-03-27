[[vk::binding(0, 2)]]
Texture2DArray unitTextures : register(t0);
[[vk::binding(0, 2)]]
SamplerState unitSampler : register(s0);

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int2 sheetSize: TEXCOORD2;
    int2 frameSize: TEXCOORD3;
    int direction: TEXCOORD5;
    int frame: TEXCOORD6;
};

float4 main(PSInput input) : SV_Target {
    float2 texturePos = float2((float)input.direction * (float)input.frameSize.x, (float)input.frame * (float)input.frameSize.y);

    float atlasW = (float)input.sheetSize.x;
    float atlasH = (float)input.sheetSize.y;

    float2 pixelPos = float2(
        texturePos.x + (input.uv.x * (float)input.frameSize.x),
        texturePos.y + (input.uv.y * (float)input.frameSize.y)
    );

    float2 normalizedUV = float2(pixelPos.x / atlasW, pixelPos.y / atlasH);

    float4 texColor = unitTextures.Sample(unitSampler, float3(normalizedUV, (float)input.texIndex));

    if (texColor.a < 0.1) {
        discard;
    }

    return texColor;

}