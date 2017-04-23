/*!
 *@brief	トゥーンシェーダー。。
 */


float4x4 g_worldMatrix;			//ワールド行列。
float4x4 g_viewMatrix;			//ビュー行列。
float4x4 g_projectionMatrix;	//プロジェクション行列。
float4x4 g_rotationMatrix;		//回転行列。法線を回転させるために必要になる。ライティングするなら必須。

float4	g_diffuseLightDirection;	//トゥーンシェーダーはライトは一本のみ。

texture g_diffuseTexture;		//ディフューズテクスチャ。
sampler g_diffuseTextureSampler = 
sampler_state
{
	Texture = <g_diffuseTexture>;
    MipFilter = NONE;
    MinFilter = NONE;
    MagFilter = NONE;
    AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT{
	float4	pos		: POSITION;
	float3	normal	: NORMAL0;
};

struct VS_OUTPUT{
	float4	pos		: POSITION;
	float3	normal	: TEXCOORD1;
};

/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	float4 pos; 
	pos = mul( In.pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	pos = mul( pos, g_viewMatrix );			//ワールド空間からビュー空間に変換。
	pos = mul( pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。
	Out.pos = pos;
	Out.normal = mul( In.normal, g_rotationMatrix );	//法線を回す。
	return Out;
}
/*!
 *@brief	頂点シェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	//ライトを計算。
	float uv = dot(In.normal.xyz, -g_diffuseLightDirection.xyz);
	uv = uv * 0.5f + 0.5f;
	return tex2D( g_diffuseTextureSampler, float2( uv, 0.0f) );
}
/*!
 *@brief	エッジの頂点シェーダー。
 */
float4 VSEdgeMain( float4 _pos : POSITION, float3 normal : NORMAL ) : POSITION
{
	float4 pos = _pos;
	pos.xyz += normal.xyz * 0.015f;
	pos = mul( pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	pos = mul( pos, g_viewMatrix );			//ワールド空間からビュー空間に変換。
	pos = mul( pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。
	return pos;
}
/*!
 *@brief	エッジのピクセルシェーダー。
 */
float4 PSEdgeMain( float4 pos : POSITION ) : COLOR
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
/*!
 *@brief	輪郭線を描画。
 */
technique EdgeRender
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSEdgeMain();
		PixelShader		= compile ps_2_0 PSEdgeMain();
	}
}
/*!
 *@brief	トゥーンレンダリング。
 */
technique ToonRender
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}