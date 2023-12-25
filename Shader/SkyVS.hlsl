#include "Common.hlsl"




PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(position, WVP);
    output.OldPosition = mul(position, OldWVP);
    output.WorldPosition.xyz = mul(position, World).xyz;
    //output.WorldPosition.w = output.Position.z;

    //float4 normal = float4(input.Normal, 0.0f);
    //output.Normal = mul(normal, World);

    output.TexCoord = input.TexCoord;

    //output.Color = input.Color;
    //output.Occlusion = input.Occlusion;
    
    

    return output;
}

