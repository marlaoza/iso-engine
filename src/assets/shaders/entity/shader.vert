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
    int padding0;
};

struct VSInput {
    float2 pos: POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int2 frameSize: TEXCOORD2;
    int frameCount: TEXCOORD3;
    int direction: TEXCOORD4;
    int speed: TEXCOORD5;
    int2 gridPos: TEXCOORD6;
    int indexOffset: TEXCOORD7; 

};

struct VSOutput {
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int2 frameSize: TEXCOORD2;
    int direction: TEXCOORD3;
    int frame: TEXCOORD4;

};

VSOutput main(VSInput input) {
    VSOutput output;

    float2 resolution = float2(windowWidth, windowHeight);

    float2 normPos = input.pos;
    float2 normCam = float2(camX, camY);
    
    float2 p = (normPos - normCam) * camZoom;

    float index = (float)(input.gridPos.x + input.gridPos.y + input.indexOffset);
    float depth = (index / (float)mapSize) + 0.005;


    output.pos = float4((p.x / resolution.x) * 2.0 - 1.0, 
                        (p.y / resolution.y) * -2.0 + 1.0, 
                        1.0 - depth, 1.0);

    output.texIndex = input.texIndex;
    output.frameSize = input.frameSize;

    output.uv = input.uv;
    output.direction = input.direction;
    output.frame = (frameTime * input.speed) % input.frameCount;

    return output;
}