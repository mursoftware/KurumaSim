
#include "Common.hlsl"

TextureCube<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);

//Texture2D<float4> textureARM : register(t3);
//Texture2D<float4> textureNor : register(t4);
Texture2D<float> textureShadow[3] : register(t5);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);



[earlydepthstencil]


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

   

	float3 normal = normalize(input.Normal.xyz);
	float3 position = input.WorldPosition.xyz;
	float ao = pow(input.Color.r, 4);
	
    
	float4 clearColor = Material.ClearColor;
	float clearCoat = Material.ClearCoat;
	float clearSpecular = Material.ClearSpecular;
	float clearRoughness = Material.ClearRoughness;

   	float4 baseColor = Material.BaseColor;
	float baseMetallic = Material.BaseMetallic;
	float baseSpecular = Material.BaseSpecular;
	float baseRoughness = Material.BaseRoughness;
    
	float4 emissionColor = Material.EmissionColor;
  

	float3 eyeVector = normalize(CameraPosition.xyz - position.xyz);
	float3 eyeRefVector = -reflect(eyeVector, normal.xyz);
	float nde = saturate(dot(normal.xyz, eyeRefVector));
	float ndl = saturate(dot(normal.xyz, LightDirection.xyz));
	float rdl = saturate(dot(eyeRefVector.xyz, LightDirection.xyz));
	float3 halfVector = normalize(eyeVector + LightDirection.xyz);
	float ndh = saturate(dot(normal.xyz, halfVector));
	float edh = saturate(dot(eyeVector, halfVector));


 
	float3 binormal, tangent;
	float binormalLen;
	
	binormal = cross(normal, float3(0.0, 0.0, 1.0));
	binormalLen = length(binormal);
	if (binormalLen > 0.001)
		binormal /= binormalLen;
	else
	{
		binormal = -cross(normal, float3(1.0, 0.0, 0.0));
		binormal = normalize(binormal);
	}
	tangent = cross(normal, binormal);

	
	float aniso = fmod(floor((input.TexCoord.z + 10.0) * 100.0/* + (input.TexCoord.y + 10.0) * 100.0*/ + floor((input.TexCoord.x + 10.0) * 600.0) * 0.5), 2);
	float3 anisoVector = lerp(binormal, tangent, aniso);


   
	output.Color.rgb = 0.0;
    
     
    
    //Sun Light
    {
		float3 light = ScatteringLight;
		

		float shadow = 0.0;
		
		//Shadow
		{
			float2 shadowTexCoord;
			shadowTexCoord.x = input.ShadowPosition[0].x * 0.5f + 0.5f;
			shadowTexCoord.y = -input.ShadowPosition[0].y * 0.5f + 0.5f;

			//for (int i = 0; i < 4; i++)
			{
				float2 offset = PoissonSamples[(int) (Random(float4(shadowTexCoord.xyy, TemporalFrame)) * 64)] * 0.001;
				float shadowColorTex = textureShadow[0].SampleLevel(sampler1, shadowTexCoord + offset, 0).r;
				if (shadowColorTex + 0.001 < input.ShadowPosition[0].z)
					shadow += 1.0; // / 4.0;
			}  
			
			light -= light * shadow;
		}
		
		
		
		
		float sunSize = 0.0001;

        
        //Clear Layer Specular
        {
			float x0 = (1 - rdl) - sunSize;
			float x1 = (1 - rdl) + sunSize;
			
			float3 fresnel = Fresnel(clearSpecular, edh, clearRoughness);
			
			float d0, d1, d;
			float a = 1.0 / (clearRoughness * clearRoughness * 0.1 + 0.00001);
			d0 = 1.0 / (1.0 + exp(-x0 * a));
			d1 = 1.0 / (1.0 + exp(-x1 * a));
			d = d1 - d0;
			
			
			//Weaken specularity on surfaces with high curvature
			float r = (length(ddx(normal) + ddy(normal))) * 300.0 + 1.0;
			d /= r;

          
			output.Color.rgb += light * 1000.0 * (1.0 - shadow) * fresnel * d * clearCoat;

			light -= light * fresnel * d * clearCoat;
		}
        
       
		//Base Layer Specular
        {
			float anisoRef = length(cross(anisoVector, halfVector));
			float ref = lerp(rdl, anisoRef, Material.Anisotropic);
			
			float anisoFres = length(cross(anisoVector, eyeVector));
			float fres = lerp(edh, anisoFres, Material.Anisotropic);
		
			
			float x0 = (1 - ref) - sunSize;
			float x1 = (1 - ref) + sunSize;

			
			float3 f0 = lerp(baseSpecular, baseColor.rgb, baseMetallic);
			float roughness = lerp(baseRoughness, 0.0, baseMetallic);
			float3 fresnel = Fresnel(f0, fres, roughness);
			
                       			
			float d0, d1, d;
			float a = 1.0 / (baseRoughness * baseRoughness * 0.1 + 0.00001);
			d0 = 1.0 / (1.0 + exp(-x0 * a));
			d1 = 1.0 / (1.0 + exp(-x1 * a));
			d = d1 - d0;
			
			//Weaken specularity on surfaces with high curvature
			float r = (length(ddx(normal) + ddy(normal))) * 10.0 + 1.0;
			d /= r;

			
			output.Color.rgb += light * 1000.0 * (1.0 - shadow) * fresnel * d;

			light -= light * fresnel * d;

		}
        
        
        //Base Layer Diffuse
        {
			output.Color.rgb += light * ndl * (1.0 - baseMetallic) * baseColor.rgb / PI;
		}
       
	}
    
    
    
    

    //Environment Light
    {
		float lightRatio = ao;
        
        
        //Clear Layer Specular
        {
			float fresnel = Fresnel(clearSpecular, nde, clearRoughness);
            
			output.Color.rgb += textureEnv.SampleLevel(sampler1, eyeRefVector, clearRoughness * 10.0 - 0.0).rgb * lightRatio * fresnel * clearCoat;
            
			lightRatio -= lightRatio * fresnel * clearCoat;
		}
        
        
        //Base Layer Specular
        {
			float anisoFres = length(cross(anisoVector, eyeVector));
			float fres = lerp(edh, anisoFres, Material.Anisotropic);

			float3 f0 = lerp(baseSpecular, baseColor.rgb, baseMetallic);
			float roughness = lerp(baseRoughness, 0.0, baseMetallic);
			float3 fresnel = Fresnel(f0, fres, roughness);
            
			output.Color.rgb += textureEnv.SampleLevel(sampler1, eyeRefVector, baseRoughness * 10.0 - 0.0).rgb * lightRatio * fresnel;// * length(cross(anisoVector, eyeRefVector));
            
			lightRatio -= lightRatio * fresnel;
		}
        
        
        //Base Layer Diffuse
        {
			float2 iblTexCoord;
			iblTexCoord.x = atan2(normal.x, normal.z) / (PI * 2);
			iblTexCoord.y = acos(normal.y) / PI;

			output.Color.rgb += textureIBL.Sample(sampler2, iblTexCoord).rgb * lightRatio * (1.0 - baseMetallic) * baseColor.rgb; //Normalized during IBL texture generation
		}
       
	}
 
    
    
    
    //Emission
    {   
		output.Color.rgb += emissionColor.rgb;
	}
    
	
	
    
    //Transmission
    {   
		float fresnel = Fresnel(baseSpecular, nde, baseRoughness);
		output.Color.a = 1.0 - (1.0 - baseColor.a) * (1 - fresnel);
		output.Color.a = saturate(output.Color.a * 2.0);//glass*2

	}
	
	
	//output.Color.rgb = anisoVector;
	
	
    
    //velocity vector
    {
		float2 oldPosition = input.OldPosition.xy;
		oldPosition /= input.OldPosition.w;
		oldPosition.y = -oldPosition.y;
		oldPosition = (oldPosition + 1.0) * 0.5;

		float2 newPosition = input.Position.xy;
		newPosition.x /= SSBufferSize.x;
		newPosition.y /= SSBufferSize.y;

		float2 velocity = (newPosition - oldPosition);
		output.Velocity.xy = velocity * Param.w;
		output.Velocity.a = 1.0;
	}


	return output;
}
