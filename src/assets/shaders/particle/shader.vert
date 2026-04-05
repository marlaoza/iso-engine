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

    float2 particlePos: TEXCOORD1;
    int2 gridPos: TEXCOORD2;
    float2 velocity: TEXCOORD3;
    float instanceTime: TEXCOORD4;
    float lifeTime: TEXCOORD5;
    float size: TEXCOORD6;

};

struct VSOutput {
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
    float opacity : TEXCOORD1;
};


VSOutput main(VSInput input) {
    VSOutput output;

    float2 resolution = float2(windowWidth, windowHeight);

    float age = frameTime - input.instanceTime;

    float frameCheck = frameTime;
    if(frameCheck < input.instanceTime){frameCheck += 50.0f;}
    if(frameCheck - input.instanceTime >= input.lifeTime){
        output.pos = float4(0, 0, 0, 0);
        return output;
    }

    float2 currentOrigin = input.particlePos + (input.velocity * age);

    float2 vertexOffset = input.pos * input.size;

    float2 normPos = currentOrigin + vertexOffset;

    float2 normCam = float2(camX, camY);
    
    float2 p = (normPos - normCam) * camZoom;

    float index = (float)(input.gridPos.x + input.gridPos.y);
    float depth = (index / (float)mapSize) + 0.005;


    output.pos = float4((p.x / resolution.x) * 2.0 - 1.0, 
                        (p.y / resolution.y) * -2.0 + 1.0, 
                        1.0 - depth, 1.0);


    output.uv = input.uv;
    output.opacity = 1.0 - (age / input.lifeTime);

    return output;
}