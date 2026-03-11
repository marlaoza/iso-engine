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
    float2 pos : POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int direction: TEXCOORD2;
    int state: TEXCOORD3;
    int2 gridPos : TEXCOORD5;


};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv: TEXCOORD0;
    int texIndex: TEXCOORD1;
    int direction: TEXCOORD2;
    int state: TEXCOORD3;
    int frame: TEXCOORD4;

};

VSOutput main(VSInput input) {
    VSOutput output;

    float2 resolution = float2(windowWidth, windowHeight);

    float2 normPos = input.pos;
    float2 normCam = float2(camX, camY);
    
    float2 p = (normPos - normCam) * camZoom;

    float depth = ( (float)(input.gridPos.x + input.gridPos.y) / (float)mapSize ) + 0.005;

    output.pos = float4((p.x / resolution.x) * 2.0 - 1.0, 
                        (p.y / resolution.y) * -2.0 + 1.0, 
                        1.0 - depth, 1.0);

    output.texIndex = input.texIndex;
    output.uv = input.uv;
    output.direction = input.direction;
    output.state = input.state;
    output.frame = frameTime % 3;

    return output;
}