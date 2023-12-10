
#include "Common.hlsl"
#include "Noise.hlsl"



Texture2D<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
SamplerState sampler0 : register(s0);


[earlydepthstencil]

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;


	//float4 color = textureDif.Sample(sampler0, input.TexCoord);
	//output.Color = color;
    
 
    
    
    
     
	float3 sunLight = LightColor;
	float3 wavelength = float3(0.630, 0.532, 0.467);
	float3 wavelength4inv = 1.0 / pow(wavelength, 4);

    float3 eyeVector = normalize(input.WorldPosition.xyz - CameraPosition.xyz);
    float dle = dot(LightDirection, eyeVector);
    
    
    //ÉåÉCÉäÅ[éUóê
	float atomThicknessRatio = 0.05;
	float atomDensityEye = atomThicknessRatio + pow(1.0 - eyeVector.y, 20) * (1.0 - atomThicknessRatio);
	float atomDensityLight = atomThicknessRatio + pow(1.0 - LightDirection.y, 20) * (1.0 - atomThicknessRatio);
	float3 scatteringLight = sunLight * exp(-atomDensityLight * atomDensityEye * wavelength4inv * 1.0);
    
	float rayleighPhase = (1.0 + dle * dle) * 0.5;
      
	output.Color.rgb = scatteringLight * atomDensityEye * wavelength4inv * rayleighPhase * 0.2;
    
    
    
    
   
    
    //â_
	float atomDensityCloud = atomThicknessRatio + pow(1.0 - CloudHeight, 20) * (1.0 - atomThicknessRatio);
	float3 cloudColor = sunLight * exp(-atomDensityLight * atomDensityCloud * wavelength4inv * 1.0);
	float2 cloudPosition = input.WorldPosition.xz * 700.0 / input.WorldPosition.y;
  
	float noise = fbm2(cloudPosition * 0.001, 5, Time * 0.1);
	float cloud = gain(0.99, saturate(noise + CloudDensity));
	cloud *= saturate(1.0 / (length(cloudPosition) * 0.0005));
    
	output.Color.rgb = lerp(output.Color.rgb, cloudColor * 0.1, cloud);
    
    
    
    
    
    
    //É~Å[éUóê
	float g = 0.990f;
	float g2 = g * g;
	float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + dle * dle) / pow((1.0 + g2 - 2.0 * g * dle), 0.5); 
	//miePhase = pow(saturate(dle), 200);
	miePhase += step(0.99995, dle) * 2.0;
    
	output.Color.rgb *= exp(-atomDensityEye * 1.0);
	output.Color.rgb += scatteringLight * miePhase;
    
    
   
    

    //ínïΩê¸
    output.Color.rgb *= smoothstep(-10.0, 10.0, input.WorldPosition.y);
	
    
    
    output.Color.a = 1.0;



    float2 oldPosition = input.OldPosition.xy;
    oldPosition /= input.OldPosition.w;
    oldPosition.y = -oldPosition.y;
    oldPosition = (oldPosition + 1.0) * 0.5;

    float2 newPosition = input.Position.xy;
    newPosition.x /= SSBufferSize.x;
    newPosition.y /= SSBufferSize.y;

    output.Velocity.xy = (newPosition - oldPosition);
    //output.Velocity.z = input.WorldPosition.w;
	//output.Velocity.z = input.Position.z;
    output.Velocity.a = 1.0;

    return output;

}
