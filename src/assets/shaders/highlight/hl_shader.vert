[[vk::binding(0, 0)]]
cbuffer SceneData : register(b0) {
    float windowWidth;
    float windowHeight;
    float mouseX;
    float mouseY;
    float camX;
    float camY;
    float camZoom;
    float padding0;
    int selectedX;
    int selectedY;
    int mapSize;
    int padding1;
};

struct VSInput {
    float2 pos : POSITION;
    float2 uv : TEXCOORD1;
    int type: TEXCOORD2;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    int type: TEXCOORD1;
};

VSOutput main(VSInput input) {
    VSOutput output;

    float2 resolution = float2(windowWidth, windowHeight);

    float2 normPos = input.pos;
    float2 normCam = float2(camX, camY);
    
    float2 p = (normPos - normCam) * camZoom;
    
    output.pos = float4((p.x / resolution.x) * 2.0 - 1.0, 
                        (p.y / resolution.y) * -2.0 + 1.0, 
                        0.5, 1.0);

    output.uv = input.uv;
    output.type = input.type;


    return output;
}