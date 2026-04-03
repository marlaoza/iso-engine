[[vk::binding(0, 2)]]
Texture2DArray unitTextures : register(t0);
[[vk::binding(0, 2)]]
SamplerState unitSampler : register(s0);

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int2 frameSize: TEXCOORD2;
    int direction: TEXCOORD3;
    int frame: TEXCOORD4;
};

float4 main(PSInput input) : SV_Target {
    int2 pixelCoords = int2(
        (input.direction * input.frameSize.x) + (input.uv.x * (input.frameSize.x - 0.001)),
        (input.frame * input.frameSize.y) + (input.uv.y * (input.frameSize.y - 0.001))
    );
    float4 texColor = unitTextures.Load(int4(pixelCoords.x, pixelCoords.y, input.texIndex, 0));

    float2 texturePos = float2((float)input.direction * (float)input.frameSize.x, (float)input.frame * (float)input.frameSize.y);

    if (texColor.a < 0.1) {discard;}

    return texColor;

}