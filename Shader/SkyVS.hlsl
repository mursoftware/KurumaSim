#include "Common.hlsl"





float scale(float fCos)
{
	float fScaleDepth = 0.01f; // The scale depth (i.e. the altitude at which the atmosphere's average density is found)

	float x = 1.0 - fCos;
	return fScaleDepth * pow(2.71, -0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}



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
    
    
    
    
    
    
    
    
    
    
    
	//float PI = 3.141592653589;
 
    
	//float3 v3CameraPos = CameraPosition.xyz; // The camera's current position
	//float3 v3LightPos = -LightDirection.xyz; // The direction vector to the light source
	//float3 v3Wavelength = float3(0.650, 0.570, 0.475); // 1 / pow(wavelength, 4) for the red, green, and blue channels
	//float3 v3InvWavelength = 1.0 / pow(v3Wavelength, 4.0); // 1 / pow(wavelength, 4) for the red, green, and blue channels
	//float fCameraHeight = 10.1; // The camera's current height
	//float fCameraHeight2 = fCameraHeight * fCameraHeight; // fCameraHeight^2
	//float fOuterRadius = 10.25f; // The outer (atmosphere) radius
	//float fOuterRadius2 = fOuterRadius * fOuterRadius; // fOuterRadius^2
	//float fInnerRadius = 10.0f; // The inner (planetary) radius
	//float fInnerRadius2 = fInnerRadius * fInnerRadius; // fInnerRadius^2
	//float fKrESun = 0.0025f * 5.0f; // Kr * ESun
	//float fKmESun = 0.0010f * 5.0f; // Km * ESun
	//float fKr4PI = 0.0025f * 4.0f * PI; // Kr * 4 * PI
	//float fKm4PI = 0.0010f * 4.0f * PI; // Km * 4 * PI
	//float fScale = 1.0 / (fOuterRadius - fInnerRadius); // 1 / (fOuterRadius - fInnerRadius)
	//float fScaleDepth = 0.01f; // The scale depth (i.e. the altitude at which the atmosphere's average density is found)
	//float fScaleOverScaleDepth = fScale / fScaleDepth; // fScale / fScaleDepth

	//int nSamples = 10;
	//float fSamples = 10.0;

    
    
    
    
	//// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	//float3 v3Pos = input.Position.xyz * 1;
	//float3 v3Ray = v3Pos;
	//float fFar = length(v3Ray);
	//v3Ray /= fFar;

	//// Calculate the ray's starting position, then calculate its scattering offset
	//float3 v3Start = float3(0.0, fCameraHeight, 0.0);
	//float fHeight = length(v3Start);
	//float fDepth = pow(2.71, fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	//float fStartAngle = dot(v3Ray, v3Start) / fHeight;
	//float fStartOffset = fDepth * scale(fStartAngle);

	//// Initialize the scattering loop variables
	//float fSampleLength = fFar / fSamples;
	//float fScaledLength = fSampleLength * fScale;
	//float3 v3SampleRay = v3Ray * fSampleLength;
	//float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	//// Now loop through the sample rays
	//float3 v3FrontColor = float3(0.0, 0.0, 0.0);
	//for (int i = 0; i < nSamples; i++)
	//{
	//	float fHeight = length(v3SamplePoint);
	//	float fDepth = pow(2.71, fScaleOverScaleDepth * (fInnerRadius - fHeight));
	//	float fLightAngle = dot(v3LightPos, v3SamplePoint) / fHeight;
	//	float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
	//	float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
	//	float3 v3Attenuate = pow(2.71, -fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
	//	//v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
	//	v3FrontColor += v3Attenuate;
	//	v3SamplePoint += v3SampleRay;
	//}
	
	//v3FrontColor *= step(0.0, input.Position.y);
	
	
	//// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	//output.Normal.rgb = v3FrontColor * fKmESun * 1.0;
	//output.Color.rgb = v3FrontColor * (v3InvWavelength * fKrESun);
	////output.Color.rgb = v3Ray;
    
	//output.Color.a = 1.0;
    
    
    
    
    
    
    
    
    
    

    return output;
}

