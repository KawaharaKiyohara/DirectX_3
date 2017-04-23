/*!
 *@brief	�g�D�[���V�F�[�_�[�B�B
 */


float4x4 g_worldMatrix;			//���[���h�s��B
float4x4 g_viewMatrix;			//�r���[�s��B
float4x4 g_projectionMatrix;	//�v���W�F�N�V�����s��B
float4x4 g_rotationMatrix;		//��]�s��B�@������]�����邽�߂ɕK�v�ɂȂ�B���C�e�B���O����Ȃ�K�{�B

float4	g_diffuseLightDirection;	//�g�D�[���V�F�[�_�[�̓��C�g�͈�{�̂݁B

texture g_diffuseTexture;		//�f�B�t���[�Y�e�N�X�`���B
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
 *@brief	���_�V�F�[�_�[�B
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	float4 pos; 
	pos = mul( In.pos, g_worldMatrix );		//���f���̃��[�J����Ԃ��烏�[���h��Ԃɕϊ��B
	pos = mul( pos, g_viewMatrix );			//���[���h��Ԃ���r���[��Ԃɕϊ��B
	pos = mul( pos, g_projectionMatrix );	//�r���[��Ԃ���ˉe��Ԃɕϊ��B
	Out.pos = pos;
	Out.normal = mul( In.normal, g_rotationMatrix );	//�@�����񂷁B
	return Out;
}
/*!
 *@brief	���_�V�F�[�_�[�B
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	//���C�g���v�Z�B
	float uv = dot(In.normal.xyz, -g_diffuseLightDirection.xyz);
	uv = uv * 0.5f + 0.5f;
	return tex2D( g_diffuseTextureSampler, float2( uv, 0.0f) );
}
/*!
 *@brief	�G�b�W�̒��_�V�F�[�_�[�B
 */
float4 VSEdgeMain( float4 _pos : POSITION, float3 normal : NORMAL ) : POSITION
{
	float4 pos = _pos;
	pos.xyz += normal.xyz * 0.015f;
	pos = mul( pos, g_worldMatrix );		//���f���̃��[�J����Ԃ��烏�[���h��Ԃɕϊ��B
	pos = mul( pos, g_viewMatrix );			//���[���h��Ԃ���r���[��Ԃɕϊ��B
	pos = mul( pos, g_projectionMatrix );	//�r���[��Ԃ���ˉe��Ԃɕϊ��B
	return pos;
}
/*!
 *@brief	�G�b�W�̃s�N�Z���V�F�[�_�[�B
 */
float4 PSEdgeMain( float4 pos : POSITION ) : COLOR
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
/*!
 *@brief	�֊s����`��B
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
 *@brief	�g�D�[�������_�����O�B
 */
technique ToonRender
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}