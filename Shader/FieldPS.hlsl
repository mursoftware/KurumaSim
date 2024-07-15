


#include "Common.hlsl"

TextureCube<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);
Texture2D<float4> textureSub : register(t4);
Texture2D<float> textureShadow[3] : register(t5);
Texture2D<float4> textureBlobShadowBody : register(t8);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);


//Clip and Discard are not available when EarlyZ is enabled.
//[earlydepthstencil]


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;


       
	float4 baseColor = textureDif.Sample(sampler0, input.TexCoord);
	float threshold = Random(float4(input.TexCoord.xy, 0.0, TemporalFrame)) * 0.5 + 0.5;
	clip(baseColor.a - threshold);
    
	float4 subColorTex = textureSub.Sample(sampler0, input.TexCoord);

	baseColor.rgb = lerp(subColorTex.rgb, baseColor.rgb, input.Color.b) * input.Color.g;
 

	float3 normal = normalize(input.Normal.xyz);
	float3 position = input.WorldPosition.xyz;
	float ao = pow(input.Color.r, 4);
	

	float baseSpecular = Material.BaseSpecular;
	float baseRoughness = Material.BaseRoughness;
      

	float3 eyeVector = normalize(CameraPosition.xyz - position.xyz);
	float3 eyeRefVector = reflect(-eyeVector, normal.xyz);
	float nde = saturate(dot(normal.xyz, eyeRefVector));
	float ndl = saturate(dot(normal.xyz, LightDirection.xyz));
	float rdl = saturate(dot(eyeRefVector.xyz, LightDirection.xyz));
	float3 halfVector = normalize(eyeVector + LightDirection.xyz);
	float ndh = saturate(dot(normal.xyz, halfVector));


 
    //blob shadow
    {
		float4 wp = float4(input.WorldPosition.xyz, 1.0);
		float4 localShadowPos = mul(wp, BodyW);
		float2 blobTexCoord = localShadowPos.xz;
		blobTexCoord.y /= 1.5;
		blobTexCoord += float2(-0.01, 0.04);
		blobTexCoord *= 0.171;
		blobTexCoord += 0.5;
    
		float4 blobShadowTex = textureBlobShadowBody.Sample(sampler1, blobTexCoord);
    
		ao *= pow(blobShadowTex.r, 1.6);
	}
    
    
   
	output.Color.rgb = 0.0;
    
     
    
    //Sun Light
    {
		float3 light = ScatteringLight;
		

		
		//Shadow
		float shadow = 0.0;
        {
			float2 shadowTexCoord;
    
			float3 debugShadowColor[3] =
			{
				{ 1.0, 0.5, 0.5 },
				{ 0.5, 1.0, 0.5 },
				{ 0.5, 0.5, 1.0 }
			};

			for (int i = 0; i < 3; i++)
			{
				shadowTexCoord.x = input.ShadowPosition[i].x * 0.5f + 0.5f;
				shadowTexCoord.y = -input.ShadowPosition[i].y * 0.5f + 0.5f;

				if (0.01 < shadowTexCoord.x && shadowTexCoord.x < 0.99 && 0.01 < shadowTexCoord.y && shadowTexCoord.y < 0.99)
				{
					//for (int j = 0; j < 4; j++)
					{
						float2 offset = PoissonSamples[(int) (Random(float4(position, TemporalFrame)) * 64)] * 0.002;
						float shadowColorTex = textureShadow[i].Sample(sampler1, shadowTexCoord + offset).r;
						if (shadowColorTex + 0.001 * (i + 1) < input.ShadowPosition[i].z)
							shadow += 1.0 / 1.0;
					}
                
            		//baseColor.rgb *= debugShadowColor[i];

					break;
				}
			}
        
			light -= light * shadow;
		}
		
        
        
		
		
		float sunSize = 0.0001;
		float x0 = (1 - rdl) - sunSize;
		float x1 = (1 - rdl) + sunSize;
		
        
/*    
		//Base Layer Specular
		if (Material.NormalWeight < 0.5)
        {
			float3 fresnel = Fresnel(baseSpecular, ndl, baseRoughness);
                       			
			float d0, d1, d;
			float a = 1.0 / (baseRoughness * baseRoughness * 0.1 + 0.00001);
			d0 = 1.0 / (1.0 + exp(-x0 * a));
			d1 = 1.0 / (1.0 + exp(-x1 * a));
			d = d1 - d0;
          
			output.Color.rgb += light * 1000.0 * (1.0 - shadow) * fresnel * d;

			light -= light * fresnel * d;

		}
 */     
        
        //Base Layer Diffuse
        {
			if (Material.NormalWeight > 0.5)
			{
				//output.Color.rgb += light * (dot(normal.xyz, LightDirection.xyz) * 0.5 + 0.5) * (1.0 - baseMetallic) * baseColor.rgb / PI;
			}
            else
			{
				output.Color.rgb += light * ndl * baseColor.rgb / PI;
			}
		}
       
	}
    
    
    
    

    //Environment Light
    {
		float lightRatio = ao;
        
/*         
        //Base Layer Specular
		if (Material.NormalWeight < 0.5)
		{
			float3 fresnel = Fresnel(baseSpecular, ndl, baseRoughness);
            
			output.Color.rgb += textureEnv.SampleLevel(sampler1, eyeRefVector, baseRoughness * 10.0 - 0.0).rgb * lightRatio * fresnel;
            
			lightRatio -= lightRatio * fresnel;
		}
 */      
        
        //Base Layer Diffuse
        {
			float2 iblTexCoord;
			iblTexCoord.x = atan2(normal.x, normal.z) / (PI * 2);
			iblTexCoord.y = acos(normal.y) / PI;

			output.Color.rgb += textureIBL.Sample(sampler2, iblTexCoord).rgb * lightRatio * baseColor.rgb; //Normalized during IBL texture generation
		}
       
	}
 
  
    
    //Transmission
    {   
		output.Color.a = baseColor.a;
	}
	
	 
    
    

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
