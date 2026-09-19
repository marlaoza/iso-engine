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
    int boardWidth;
    int boardHeight;
    int canvasWidth;
    int canvasHeight;
};

struct VSInput {
    float2 pos: POSITION;
    float2 uv: TEXCOORD0;
};


struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input) {
    VSOutput output;
    float2 resolution = float2(windowWidth, windowHeight);
    output.pos = float4((input.pos.x / resolution.x) * 2.0 - 1.0,
                         (input.pos.y / resolution.y) * -2.0 + 1.0,
                         0.0, 1.0);
    output.uv = input.uv;
    return output;
}