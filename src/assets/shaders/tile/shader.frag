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
Texture2D lightmapTex : register(t1);
[[vk::binding(0, 2)]]
SamplerState lightmapSampler : register(s1);

struct PSInput {
    float4 pos : SV_POSITION;
    nointerpolation int2 gridPos : TEXCOORD1;
    float4 col : COLOR0;
    float faceID : TEXCOORD0;
    float2 screenPos : TEXCOORD2;
    float2 uv: TEXCOORD4;
    float2 lightUV : TEXCOORD5;
};

static const float bayer4x4[16] = {
     0.0/16,  8.0/16,  2.0/16, 10.0/16,
    12.0/16,  4.0/16, 14.0/16,  6.0/16,
     3.0/16, 11.0/16,  1.0/16,  9.0/16,
    15.0/16,  7.0/16, 13.0/16,  5.0/16
};

float3 ditherPosterize(float3 color, float levels, float2 pixelCoord){
    int x = (int)pixelCoord.x % 4;
    int y = (int)pixelCoord.y % 4;
    float threshold = bayer4x4[y*4 + x] - 0.5;
    float3 c = clamp(color * levels + threshold, 0.0, levels - 1.0);
    return floor(c) / (levels - 1.0);
}

float4 main(PSInput input) : SV_Target {
    float4 baseColor = input.col;
    float shadowMult = 1.0;
    float3 color = baseColor.rgb;

    if (input.faceID > 0.5 && input.faceID < 1.1) shadowMult = 0.8;
    if (input.faceID > 1.2 && input.faceID < 2.1) shadowMult = 0.5;

    float4 texColor = float4(color * shadowMult, baseColor.a);
    float4 lightColor = lightmapTex.Sample(lightmapSampler, input.lightUV);
    lightColor.rgb = ditherPosterize(lightColor.rgb, 6.0, input.pos.xy);
    texColor.rgb *= lightColor.rgb;

    return texColor;
}