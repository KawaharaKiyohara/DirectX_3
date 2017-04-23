/*!
 *@brief	定数レジスタにロードされたカラーを使用して、出力カラーを変更してみる。
 */


float4 g_color;		//カラー。これがシェーダー定数。CPUから値が転送されてくる。

struct VS_INPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
};

struct VS_OUTPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
};

/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;
	Out.color = In.color * g_color;	//頂点カラーに定数レジスタに設定されたカラーを乗算してみる。
	return Out;
}
/*!
 *@brief	頂点シェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	return In.color;
}

technique ColorPrim
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}