#include "Common.hlsl"


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(position, WVP);
    output.TexCoord = input.TexCoord;

    return output;
}

