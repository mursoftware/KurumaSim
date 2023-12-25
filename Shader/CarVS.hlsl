#include "Common.hlsl"


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(position, WVP);
    output.OldPosition = mul(position, OldWVP);
    output.WorldPosition.xyz = mul(position, World).xyz;
    output.WorldPosition.w = output.Position.z;
    
    float4 normal = float4(input.Normal, 0.0f);
    output.Normal = mul(normal, World);
    
    output.TexCoord = input.TexCoord;
    
    output.Color = input.Color;
    //output.Occlusion = input.Occlusion;

    output.ShadowPosition[0] = mul(position, ShadowWVP[0]);
    //output.ShadowPosition[1] = mul(position, ShadowWVP[1]);
    //output.ShadowPosition[2] = mul(position, ShadowWVP[2]);
      
   
    
    //fog
    {
		float3 fogColor;
              
		float3 sunLight = LightColor;
		float3 wavelength = float3(0.630, 0.532, 0.467);
		float3 wavelength4inv = 1.0 / pow(wavelength, 4);

		float3 eyeVector = output.WorldPosition.xyz - CameraPosition.xyz;
		float len = length(eyeVector);
		eyeVector /= len;
		float dle = dot(LightDirection, eyeVector);

        
        //Rayleigh scattering
		float atomDensityEye = 1.0 + pow(1.0 - 0.01, 10) * 10.0;
		float atomDensityLight = 1.0 + pow(1.0 - LightDirection.y, 10) * 10.0;
		float3 scatteringLight = sunLight * exp(-atomDensityLight * atomDensityEye * wavelength4inv * 0.01);
    
		float rayleighPhase = 0.5 + dle * dle;
      
		fogColor.rgb = scatteringLight * atomDensityEye * wavelength4inv * rayleighPhase * 0.01;
    
    
        //Mie scattering
		float g = 0.990f;
		float g2 = g * g;
		float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + dle * dle) / pow((1.0 + g2 - 2.0 * g * dle), 0.5);
		miePhase = pow(saturate(dle), 200);
		miePhase += step(0.99995, dle);
    
		fogColor.rgb += scatteringLight * miePhase;
    
        
		//output.Color.rgb = output.Color.rgb * exp(-len * Fog * 0.001) + fogColor * (1.0 - exp(-len * Fog * 0.001)) * saturate(output.Color.g + 0.1);
        
        
		output.Fog.rgb = fogColor;
		output.Fog.a = exp(-len * Fog * 0.001);
    
	}
    
    
    
    return output;
}

