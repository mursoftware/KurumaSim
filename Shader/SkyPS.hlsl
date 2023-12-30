
#include "Common.hlsl"
#include "Noise.hlsl"



Texture2D<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);


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
    
    
    //Rayleigh scattering
	float atomThicknessRatio = 0.05;
	float atomDensityEye = atomThicknessRatio + pow(1.0 - eyeVector.y, 10) * (1.0 - atomThicknessRatio);
	float atomDensityLight = atomThicknessRatio + pow(1.0 - LightDirection.y, 10) * (1.0 - atomThicknessRatio);
	float3 scatteringLight = sunLight * exp(-atomDensityLight * atomDensityEye * wavelength4inv * 0.5);
	float rayleighPhase = (0.5 + dle * dle);
      
	output.Color.rgb = scatteringLight * atomDensityEye * wavelength4inv * rayleighPhase * 0.03;
    
    
    
    
   
    
    //cloud
	float atomDensityCloud = atomThicknessRatio + pow(1.0 - CloudHeight, 10) * (1.0 - atomThicknessRatio);
	float3 cloudColor = sunLight * exp(-atomDensityLight * atomDensityCloud * wavelength4inv * 0.5);
	float2 cloudPosition = input.WorldPosition.xz * 700.0 / input.WorldPosition.y;
  
	float noise = fbm2(cloudPosition * 0.001, 5, Time * 0.1);
	float cloud = gain(0.9, saturate(noise + CloudDensity));
	cloud *= saturate(1.0 / (length(cloudPosition) * 0.0005));
    
	output.Color.rgb = lerp(output.Color.rgb, (cloudColor) * 0.2, cloud);
    
    
    
    
    
    
    //Mie scattering
	float g = 0.990f;
	float g2 = g * g;
	float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + dle * dle) / pow((1.0 + g2 - 2.0 * g * dle), 0.5); 
	//miePhase = pow(saturate(dle), 200);
	miePhase += step(0.99985, dle);
    
	float fog = (1.0 - exp(-1000.0 * Fog));// * saturate(exp(-input.WorldPosition.y * 0.01));
	output.Color.rgb = output.Color.rgb * (1.0 - fog) + ScatteringLight / (2.0 * PI) * fog;
	output.Color.rgb += ScatteringLight * miePhase;
    
    
   
    

    //horizon 
    output.Color.rgb *= smoothstep(-10.0, 10.0, input.WorldPosition.y);
	
    
    
  
 //   //fog
 //   {
	//	float3 envLight;
 //       {
	//		float2 iblTexCoord;
	//		iblTexCoord.x = 0.0;
	//		iblTexCoord.y = 0.0;

	//		envLight = textureIBLStatic.Sample(sampler2, iblTexCoord).rgb;
	//	}


	//	float3 dirLight;
	//	dirLight = ScatteringLight / PI;


	//	float3 fogColor = float3(0.9, 0.9, 0.9) * 1.0;
	//	float fog = (1.0 - exp(-1000.0 * Fog));// * saturate(1.0 - eyeVector.y / (Fog * 100.0));

	//	output.Color.rgb = output.Color.rgb * (1.0 - fog) + fogColor * (envLight + dirLight) * fog;
	//}
    
    
       
    
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
