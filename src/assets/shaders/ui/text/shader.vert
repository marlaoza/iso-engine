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
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
    int state : TEXCOORD3;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
};

VSOutput main(VSInput input) {
    VSOutput output;
    
    output.pos = float4(input.pos.x/windowWidth * 2.0 - 1.0, 
                        input.pos.y/windowHeight * -2.0 + 1.0, 
                        0.5, 1.0);

    output.uv = input.uv;
    output.col = input.col;

    if(input.state == 4){
        output.col = float4(0.0, 0.0, 0.0, 0.0);
    }


    return output;
}