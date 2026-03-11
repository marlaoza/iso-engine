[[vk::binding(0, 0)]]
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
    int mapSize;
    int padding1;
};

struct VSInput {
    float2 pos : POSITION;
    int2 gridPos : TEXCOORD1;
    float4 col : COLOR0;
    float faceID : TEXCOORD0;
    float2 uv : TEXCOORD2;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    nointerpolation int2 gridPos : TEXCOORD1;
    float4 col : COLOR0;
    float faceID : TEXCOORD0;
    float2 screenPos : TEXCOORD2;
    float2 uv : TEXCOORD4;
};

VSOutput main(VSInput input) {
    VSOutput output;

    float2 resolution = float2(windowWidth, windowHeight);

    float2 normPos = input.pos;
    float2 normCam = float2(camX, camY);
    
    float2 p = (normPos - normCam) * camZoom;
        
    float depth = (float)(input.gridPos.x + input.gridPos.y) / (float)mapSize;

    output.pos = float4((p.x / resolution.x) * 2.0 - 1.0, 
                        (p.y / resolution.y) * -2.0 + 1.0, 
                        1.0 - depth, 1.0);

    output.gridPos = input.gridPos;
    output.col = input.col;
    output.faceID = input.faceID;
    output.screenPos = input.pos;
    output.uv = input.uv;

    return output;
}