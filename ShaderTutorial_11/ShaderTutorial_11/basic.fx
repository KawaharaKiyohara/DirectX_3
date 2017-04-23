/*!
 *@brief	投影シャドウ。
 */

float4x4 g_worldMatrix;			//ワールド行列。
float4x4 g_viewMatrix;			//ビュー行列。
float4x4 g_projectionMatrix;	//プロジェクション行列。

int g_isShadowReciever;				//シャドウレシーバー？１ならシャドウレシーバー。
float4x4 g_lightViewMatrix;			//ライトビュー行列。
float4x4 g_lightProjectionMatrix;	//ライトプロジェクション行列。

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

texture g_shadowMapTexture;		//シャドウマップテクスチャ。
sampler g_shadowMapTextureSampler = 
sampler_state
{
	Texture = <g_shadowMapTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
	AddressV = CLAMP;
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
	float4	lightViewPos 	: TEXCOORD1;		//ワールド空間->ライトビュー空間->ライト射影空間に変換された座標。

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
	if(g_isShadowReciever == 1){
		//シャドウレシーバー。
		//ワールド座標をライトカメラから見た射影空間に変換する。
		Out.lightViewPos = mul(worldPos, g_lightViewMatrix);
		Out.lightViewPos = mul(Out.lightViewPos, g_lightProjectionMatrix);
	}
	return Out;
}
/*!
 *@brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	float4 color = tex2D( g_diffuseTextureSampler, In.uv );
	if(g_isShadowReciever == 1){
		//射影空間(スクリーン座標系)に変換された座標はw成分で割ってやると(-1.0f～1.0)の範囲の正規化座標系になる。
		//これをUV座標系(0.0～1.0)に変換して、シャドウマップをフェッチするためのUVとして活用する。
		float2 shadowMapUV = In.lightViewPos.xy / In.lightViewPos.w;	//この計算で(-1.0～1.0)の範囲になる。
		shadowMapUV *= float2(0.5f, -0.5f);								//この計算で(-0.5～0.5)の範囲になる。
		shadowMapUV += float2(0.5f, 0.5f);								//そしてこれで(0.0～1.0)の範囲になってＵＶ座標系に変換できた。やったね。
		float4 shadowVal = tex2D(g_shadowMapTextureSampler, shadowMapUV);	//シャドウマップは影が落ちているところはグレースケールになっている。
		color *= shadowVal;
	}
	return color;
}
/*!
 *@brief	シャドウマップ書き込み用のピクセルシェーダー。
 */
float4 PSRenderToShadowMapMain(VS_OUTPUT In) : COLOR
{
	return float4(0.5f, 0.5f, 0.5f, 1.0f);
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
/*!
 *@brief	シャドウマップ書き込み用のテクニック
 */
technique SkinModelRenderToShadowMap
{
	pass p0
	{
		VertexShader 	= compile vs_3_0 VSMain();
		PixelShader 	= compile ps_3_0 PSRenderToShadowMapMain(); 
	}
}