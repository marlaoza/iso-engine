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
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
    int2 texturePos : TEXCOORD1;
    int2 textureSize: TEXCOORD2;
    int2 highLightTexturePos: TEXCOORD3;
    int2 highLightTextureSize : TEXCOORD4;
    int state : TEXCOORD5;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
    float2 texturePos : TEXCOORD1;
    float2 textureSize: TEXCOORD2;
};

VSOutput main(VSInput input) {
    VSOutput output;
    
    output.pos = float4(input.pos.x/windowWidth * 2.0 - 1.0, 
                        input.pos.y/windowHeight * -2.0 + 1.0, 
                        0.5, 1.0);

    output.uv = input.uv;
    output.col = input.col;

    output.texturePos = float2(input.texturePos.x, input.texturePos.y);
    output.textureSize = float2(input.textureSize.x, input.textureSize.y);

    if(input.state == 1){
        output.texturePos = float2(input.highLightTexturePos.x, input.highLightTexturePos.y);
        output.textureSize = float2(input.highLightTextureSize.x, input.highLightTextureSize.y);
    }

    if(input.state == 4){
        output.col = float4(0.0, 0.0, 0.0, 0.0);
    }


    return output;
}