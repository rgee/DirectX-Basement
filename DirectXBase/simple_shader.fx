//
// Simple shader. Does nothing fancy.
//

Texture2D tex2D;
SamplerState linearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


struct PointLight
{
	float3 Pos;
	float4 Color;
};

float3 lightDir = float3(0, 0, 10);
float4 lightColor = float4(0.9, 0.2, 0.2, 1.0);
float4 ambientLight = float4(0.02, 0.02, 0.02, 1.0);
float3 lightPosition = float3(0, 50, 10);

float3 lightDir2 = float3(0, 0, -10);

float lightIntensity = 1.9;

matrix World;
matrix View;
matrix Projection;

PointLight light;

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float2 Tex : TEXCOORD1;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};



PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	
	output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );    
    output.Pos = mul( output.Pos, Projection );
	output.Normal = normalize(mul(input.Normal, World));
	output.Tex = input.Tex;

	
    return output;  
}

float4 textured( PS_INPUT input ) : SV_Target
{
	float4 finalColor = 0;
	
	float3 toLight = light.Pos - input.Pos;
	float attenuation = saturate(1.0 - pow(length(toLight), 2.0));

	finalColor = saturate( dot(normalize(-toLight), normalize(input.Normal)) * tex2D.Sample(linearSampler, input.Tex)) ;

	//finalColor = finalColor + saturate(dot(normalize(lightDir2), normalize(input.Normal)) * tex2D.Sample(linearSampler, input.Tex)) * lightIntensity;


	//finalColor = tex2D.Sample(linearSampler, input.Tex);
	finalColor.a = 1.0;

	return finalColor;
}

float4 noTexture( PS_INPUT input ) : SV_Target
{
	float4 finalColor = 0;
	
	finalColor = saturate(dot(lightDir, input.Normal) * lightColor);
	finalColor.a = 1.0;

	return finalColor;
}

technique10 full
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, textured() ) );
    }
}

technique10 texturingDisabled
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, noTexture() ) );
    }
}