
#include "Common.hlsl"


#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 20
#define FXAA_GREEN_AS_LUMA 1
#include "FXAA.hlsl"



Texture2D<float4> textureColor : register(t0);
Texture2D<float4> textureColorOld : register(t1);
Texture2D<float4> textureVelocity : register(t2);
//Texture2D<float4> textureVelocityOld : register(t3);
Texture2D<float> textureDepth : register(t3);
Texture2D<float> textureDepthOld : register(t4);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler3 : register(s3);


static const float3 Perception = float3(0.299, 0.587, 0.114);

float ReinhardF(float input)
{
	return input / (1 + input);
}


float3 Reinhard(float3 input)
{
	return input / (1 + input);
}

float3 InvReinhard(float3 input)
{
	return input / (1 - input);
}


float4 main(PS_INPUT input) : SV_TARGET0
{
  
    float4 color;
	float2 texcoord = input.TexCoord;
	
	
	
	switch (AntiAliasing)
	{
		case 0: //None
		{
			color.rgb = textureColor.SampleLevel(sampler1, texcoord, 0).rgb;
			color.a = 1.0;

			break;
		}

		case 1: //FXAA
		{
			FxaaTex InputFXAATex = { sampler1, textureColor };
        
			float2 size;
			size.x = 1.0 / SSBufferSize.x;
			size.y = 1.0 / SSBufferSize.y;

			color = FxaaPixelShader(
				texcoord, // FxaaFloat2 pos,
				FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsolePosPos,
				InputFXAATex, // FxaaTex tex,
				InputFXAATex, // FxaaTex fxaaConsole360TexExpBiasNegOne,
				InputFXAATex, // FxaaTex fxaaConsole360TexExpBiasNegTwo,
				size, // FxaaFloat2 fxaaQualityRcpFrame,
				FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt,
				FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
				FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f), // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
				1.0, // FxaaFloat fxaaQualitySubpix,
				0.166, // FxaaFloat fxaaQualityEdgeThreshold,
				0.0833, // FxaaFloat fxaaQualityEdgeThresholdMin,
				0.0, // FxaaFloat fxaaConsoleEdgeSharpness,
				0.0, // FxaaFloat fxaaConsoleEdgeThreshold,
				0.0, // FxaaFloat fxaaConsoleEdgeThresholdMin,
				FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f) // FxaaFloat fxaaConsole360ConstDir,
			);
			color.a = 1.0;

			break;
		}



		case 2: //TAA
		{
			float2 velocity = textureVelocity.Sample(sampler1, texcoord);
			//float2 oldVelocity = textureVelocity.Sample(sampler1, texcoord - velocity);
			float3 newColor = textureColor.SampleLevel(sampler1, texcoord, 0).rgb;
			float3 oldColor = textureColorOld.SampleLevel(sampler1, texcoord - velocity, 0).rgb;
			
			float newDepth = textureDepth.SampleLevel(sampler1, texcoord, 0).r;
			float oldDepth, ddmin = 100.0;
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(-1.0 / SSBufferSize.x, 1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(1.0 / SSBufferSize.x, 1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(-1.0 / SSBufferSize.x, -1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(1.0 / SSBufferSize.x, -1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
		
			newColor = Reinhard(newColor);
			oldColor = Reinhard(oldColor);
		
			float dl = abs(dot(newColor, Perception) - dot(oldColor, Perception));
			//float vd = length(velocity - oldVelocity);
		
	
				float a = saturate(TemporalRatio
			//- vd * 100.0
			- length(velocity) * 100.0
			- dl * 0.1
			- step(0.001, ddmin)
			);
		
			color.rgb = lerp(newColor, oldColor, a);
			color.rgb = InvReinhard(color.rgb);
			color.a = 1.0;

			break;
		}


		case 3: //TSR
		{
			float2 velocity = textureVelocity.Sample(sampler1, texcoord);
			//float2 oldVelocity = textureVelocity.Sample(sampler1, texcoord - velocity);
			float3 newColor = textureColor.SampleLevel(sampler3, texcoord, 0).rgb;		
			float3 oldColor = textureColorOld.SampleLevel(sampler1, texcoord - velocity, 0).rgb;
			
			
			float newDepth = textureDepth.SampleLevel(sampler1, texcoord, 0).r;
			float oldDepth, ddmin = 100.0;
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(-1.0 / SSBufferSize.x, 1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(1.0 / SSBufferSize.x, 1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(-1.0 / SSBufferSize.x, -1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));
			
			oldDepth = textureDepthOld.SampleLevel(sampler1, texcoord - velocity + float2(1.0 / SSBufferSize.x, -1.0 / SSBufferSize.y), 0).r;
			ddmin = min(ddmin, abs(newDepth - oldDepth));

			
			newColor = Reinhard(newColor);
			oldColor = Reinhard(oldColor);
		
			float dl = abs(dot(newColor, Perception) - dot(oldColor, Perception));
			//float vd = length(velocity - oldVelocity);
		
	
			float a;
		
			if (((int) input.Position.x % 2) == (TemporalFrame % 2) && ((int) input.Position.y % 2) == (int) (TemporalFrame / 2))
			{
				a = saturate(TemporalRatio /*- vd * 100.0*/ - length(velocity) * 100.0 - dl * 0.1/* - step(0.001, ddmin)*/);
			}
			else
			{
				a = saturate(1.0 /*- vd * 100.0*/ - length(velocity) * 100.0 - dl * 0.1/* - step(0.1, ddmin)*/);
			}
		
			color.rgb = lerp(newColor, oldColor, a);
			color.rgb = InvReinhard(color.rgb);
			color.a = 1.0;

			break;
		}

	} 
    
    return color;
}


/*
// ヒストリカラーの取得
const float2 uvT = uv + float2(0.0f, TAA_PIXEL_SIZE_Y);
const float2 uvB = uv + float2(0.0f, -TAA_PIXEL_SIZE_Y);
const float2 uvR = uv + float2(TAA_PIXEL_SIZE_X, 0.0f);
const float2 uvL = uv + float2(-TAA_PIXEL_SIZE_X, 0.0f);
// 近傍のピクセルのうち,最も手前にあるピクセルの速度マップを利用する
const float srcLinearDepthT = DecodeFloatRG(SampleTextureLevel0(g_DepthNormalTexture, g_LinearSampler, uvT).ba);
const float srcLinearDepthB = DecodeFloatRG(SampleTextureLevel0(g_DepthNormalTexture, g_LinearSampler, uvB).ba);
const float srcLinearDepthR = DecodeFloatRG(SampleTextureLevel0(g_DepthNormalTexture, g_LinearSampler, uvR).ba);
const float srcLinearDepthL = DecodeFloatRG(SampleTextureLevel0(g_DepthNormalTexture, g_LinearSampler, uvL).ba);
float3 velocityUV = float3(uv, srcLinearDepth);
velocityUV = lerp(velocityUV, float3(uvT, srcLinearDepthT), step(srcLinearDepthT, velocityUV.z));
velocityUV = lerp(velocityUV, float3(uvB, srcLinearDepthB), step(srcLinearDepthB, velocityUV.z));
velocityUV = lerp(velocityUV, float3(uvR, srcLinearDepthR), step(srcLinearDepthR, velocityUV.z));
velocityUV = lerp(velocityUV, float3(uvL, srcLinearDepthL), step(srcLinearDepthL, velocityUV.z));
const float2 velocity = SampleTextureLevel0(g_VelocityTexture, g_LinearSampler, velocityUV.xy).rg * TAA_VELOCITY_DECODE.xy + TAA_VELOCITY_DECODE.zw;
// ヒストリーカラーを速度マップを参照してフェッチする
const float2 historyUV = saturate(uv - velocity);
float3 historyColor = TonemapTAA(SampleTextureLevel0(g_HistoryTexture, g_LinearSampler, historyUV).rgb);
// カラー周辺の色でヒストリカラーをクランプ
const float3 inputColor = TonemapTAA(outColor.rgb);
const float3 cT = TonemapTAA(SampleTextureLevel0(g_SrcTexture, g_LinearSampler, uvT).rgb);
const float3 cB = TonemapTAA(SampleTextureLevel0(g_SrcTexture, g_LinearSampler, uvB).rgb);
const float3 cR = TonemapTAA(SampleTextureLevel0(g_SrcTexture, g_LinearSampler, uvR).rgb);
const float3 cL = TonemapTAA(SampleTextureLevel0(g_SrcTexture, g_LinearSampler, uvL).rgb);
const float3 cMin = min(inputColor, min(min(cT, cB), min(cR, cL)));
const float3 cMax = max(inputColor, max(min(cT, cB), max(cR, cL)));
historyColor = clamp(historyColor, cMin, cMax);
// ヒストリーと現在のカラーの合成
// 大きく動いているピクセルのファクタ, 0~1
const float velocityHistoryColorFactor = saturate(length(velocity) * TAA_ALPHA_VELOCITY_SCALE + TAA_ALPHA_VELOCITY_OFFSET);
float alpha = lerp(TAA_ALPHA_STATIC, TAA_ALPHA_DYNAMIC, velocityHistoryColorFactor);
// 輝度差によってalphaを変える,輝度差が大きい場合は不採用
const float lumaHistoryColorFactor = saturate(abs(GetLuminance(historyColor) - GetLuminance(inputColor)) * TAA_ALPHA_LUMA_SCALE + TAA_ALPHA_LUMA_OFFSET);
alpha *=
lumaHistoryColorFactor;
outColor.rgb = InvTonemapTAA(lerp(inputColor, historyColor, alpha));

*/