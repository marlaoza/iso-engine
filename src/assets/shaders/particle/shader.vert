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
    float4 startColor: TEXCOORD6;
    float4 endColor: TEXCOORD7;
    float startSize: TEXCOORD8;
    float endSize: TEXCOORD9;
};

struct VSOutput {
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
    float4 color : COLOR0;
};


VSOutput main(VSInput input) {
    VSOutput output;

    float2 resolution = float2(windowWidth, windowHeight);

    float age = frameTime - input.instanceTime;

    float t = saturate(age / input.lifeTime);

    if (age < 0 || age > input.lifeTime) {
        output.pos = float4(0, 0, 0, 0);
        return output;
    }

    float currentSize = lerp(input.startSize, input.endSize, t);

    output.color = lerp(input.startColor, input.endColor, t);

    float2 currentOrigin = input.particlePos + (input.velocity * age);

    float indexXOffset = 0.0;
    if(currentOrigin.x - input.particlePos.x > 18.0){
        indexXOffset = (currentOrigin.x - input.particlePos.x) % 18.0;
    }else if(input.particlePos.x - currentOrigin.x > 18.0){
        indexXOffset = (currentOrigin.x - input.particlePos.x) % 18.0;
    }

    float indexYOffset = 0.0;
    if(currentOrigin.y - input.particlePos.y > 18.0){
        indexYOffset = (currentOrigin.y - input.particlePos.y) % 18.0;
    }else if(input.particlePos.y - currentOrigin.y > 18.0){
        indexYOffset = (currentOrigin.y - input.particlePos.y) % 18.0;
    }

    float2 vertexOffset = input.pos * currentSize;

    float2 normPos = currentOrigin + vertexOffset;

    float2 normCam = float2(camX, camY);
    
    float2 p = (normPos - normCam) * camZoom;

    float index = ((float)input.gridPos.x + indexXOffset) + ((float)input.gridPos.y + indexYOffset) + 0.05;
    float depth = (index / (float)mapSize) + 0.005;


    output.pos = float4((p.x / resolution.x) * 2.0 - 1.0, 
                        (p.y / resolution.y) * -2.0 + 1.0, 
                        1.0 - depth, 1.0);


    output.uv = input.uv;

    return output;
}