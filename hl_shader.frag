struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    int type: TEXCOORD1;
};

float4 main(PSInput input) : SV_Target {
    float4 color = float4(0.0, 0.0, 0.0, 0.0);

   

    if(input.type == 1) {
        float2 uv = input.uv; 
        float border = 0.1;
        bool onEdge = uv.x < border || uv.x > 1.0-border || 
                    uv.y < border || uv.y > 1.0-border;

        if(onEdge) color = float4(1.0, 1.0, 0.0, 1.0);
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

    

    return color;
}