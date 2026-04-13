struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    int type: TEXCOORD1;
    nointerpolation uint edges : TEXCOORD2;
};

float4 main(PSInput input) : SV_Target {
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
    float border = 0.1;
    bool onEdge = false;

    if(input.type == 1) {
        onEdge = input.uv.x < border || input.uv.x > 1.0-border || 
                    input.uv.y < border || input.uv.y > 1.0-border;

        if(onEdge) color = float4(1.0, 1.0, 0.0, 1.0);
        return color;
    }

    if(input.type == 2){
        color = float4(0.0, 1.0, 0.0, 0.5); //verdin
    }

    if(input.type == 3){
        color = float4(0.0, 0.0, 1.0, 0.5); //azulzin
    }

    if(input.type == 4){
        color = float4(0.9, 0.0, 1.0, 0.5); //roxin
    }

    if(input.type == 5){
        color = float4(1.0, 0.0, 0.0, 0.5); //vermei
    }

    if(input.edges > 0){

        //top right
        if ((input.edges & (1 << 0)) != 0) {
            onEdge = onEdge || (input.uv.y < border && input.uv.x < border);
        }
        //top
        if ((input.edges & (1 << 1)) != 0) {
            onEdge = onEdge || input.uv.y < border;
        }
        //top left
        if ((input.edges & (1 << 2)) != 0) {
            onEdge = onEdge ||(input.uv.y < border && input.uv.x > 1.0-border);
        }

        
        //right
        if ((input.edges & (1 << 3)) != 0) {
            onEdge = onEdge || input.uv.x < border;
        }
        //left
        if ((input.edges & (1 << 4)) != 0) {
            onEdge = onEdge || input.uv.x > 1.0-border;
        }


        //bottom right
        if ((input.edges & (1 << 5)) != 0) {
            onEdge = onEdge || (input.uv.y > 1.0-border && input.uv.x < border);
        }
        //bottom
        if ((input.edges & (1 << 6)) != 0) {
            onEdge = onEdge || input.uv.y > 1.0-border;
        }
        //bottom left
        if ((input.edges & (1 << 7)) != 0) {
            onEdge = onEdge || (input.uv.y > 1.0-border && input.uv.x > 1.0-border);
        }

        if(onEdge) color = float4(color.r * 0.5, color.g * 0.5, color.b * 0.5, 1.0);
    }

    return color;
}