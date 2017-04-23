/*!
 *@brief	投影シャドウ。
 */

float4x4 g_worldMatrix;			//ワールド行列。
float4x4 g_viewMatrix;			//ビュー行列。
float4x4 g_projectionMatrix;	//プロジェクション行列。


texture g_diffuseTexture;		//ディフューズテクスチャ。
sampler g_diffuseTextureSampler = 
sampler_state
{
	Texture = <g_diffuseTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Wrap;
	AddressV = Wrap;
};


struct VS_INPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
	float2	uv		: TEXCOORD0;
};

struct VS_OUTPUT{
	float4	pos				: POSITION;	
	float4	color			: COLOR0;
	float2	uv				: TEXCOORD0;
};

/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4 worldPos; 
	worldPos = mul( In.pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	Out.pos = mul( worldPos, g_viewMatrix );		//ワールド空間からビュー空間に変換。
	Out.pos = mul( Out.pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。
	Out.color = In.color;
	Out.uv = In.uv;
	return Out;
}
/*!
 *@brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	float4 color = tex2D( g_diffuseTextureSampler, In.uv );
	return color;
}


/*!
 *@brief	通常描画用のテクニック
 */
technique SkinModel
{
	pass p0
	{
		VertexShader 	= compile vs_3_0 VSMain();
		PixelShader 	= compile ps_3_0 PSMain();
	}
}
