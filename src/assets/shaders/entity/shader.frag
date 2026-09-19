[[vk::binding(0, 3)]]
cbuffer SceneData : register(b0) {
    float windowWidth;
    float windowHeight;
    float mouseX;
    float mouseY;
    float camX;
    float camY;
    float camZoom;
    float frameTime;
    int selectedX;
    int selectedY;
    int boardWidth;
    int boardHeight;
    int canvasWidth;
    int canvasHeight;
};


[[vk::binding(0, 2)]]
Texture2DArray unitTextures : register(t0);
[[vk::binding(0, 2)]]
SamplerState unitSampler : register(s0);

[[vk::binding(1, 2)]]
Texture2D lightmapTex : register(t1);
[[vk::binding(1, 2)]]
SamplerState lightmapSampler : register(s1);

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int2 frameSize: TEXCOORD2;
    int direction: TEXCOORD3;
    int frame: TEXCOORD4;
    nointerpolation float2 gridPos : TEXCOORD5; 
};

float3 posterize(float3 color, float levels){
    float3 c = clamp(color * levels, 0.0, levels - 1.0);
    return floor(c) / (levels - 1.0);
}

float4 main(PSInput input) : SV_Target {
    int2 pixelCoords = int2(
        (input.direction * input.frameSize.x) + (input.uv.x * (input.frameSize.x - 0.001)),
        (input.frame * input.frameSize.y) + (input.uv.y * (input.frameSize.y - 0.001))
    );
    float4 texColor = unitTextures.Load(int4(pixelCoords.x, pixelCoords.y, input.texIndex, 0));

    float2 texturePos = float2((float)input.direction * (float)input.frameSize.x, (float)input.frame * (float)input.frameSize.y);

    if (texColor.a < 0.1) {discard;}

    float4 lightColor = lightmapTex.Sample(lightmapSampler, input.gridPos);
    lightColor.rgb = posterize(lightColor.rgb, 12.0);
    texColor.rgb *= lightColor.rgb;

    return texColor;

}