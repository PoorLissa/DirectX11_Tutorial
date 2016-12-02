// The vertex shader has now been modified to use instancing
// ����� ����, ���� ���������� ������ ������ ������� ������������� ��� ��������� ����.
// ��������� ���������� ����� ���� ������������� ������� ����������������� �������, �������, � ���� �������, ��������� �� ��������� VertexType/InstanceType,
// �������, ����� ��, ������ ��������������� ���������� � ������� � � ������ �������, �� �� ������ ������ �� ������� ������:
// VertexInputType �������� ����� ��, ��� � � ����������� �������.
// ������, ��� ���������������� ������� ������, �� ������ ��� �����.
// � ��� cbuffer �� �����, ��������, �������� �� �����, ��� ��� ������.

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float  rotationMode;        // ����� �� ������������ ��� ������� �� ���� �����
    float  rotateToX;           // X-���������� �����, �� ������� ����� ��������� ��� �������
    float  rotateToY;           // Y-���������� �����, �� ������� ����� ��������� ��� �������
    float  dummy;               //  <-- dummy
};

// The VertexInputType structure now has the third element which will hold the instanced input position data.
struct VertexInputType
{
    float4 position         : POSITION;
    float2 tex              : TEXCOORD0;
    float3 instancePosition : TEXCOORD1;    // xy = �������, z = ������� �� �������� ����
    float3 trailInfo        : TEXCOORD2;    // xy = ������� �������, z = ������� ���� (0 = ����, 1 = ���� �� ����)
};

struct PixelInputType
{
    float4 position      : SV_POSITION;
    float2 tex           : TEXCOORD0;
    float3 trailInfo     : TEXCOORD2;
};



// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

    output.position.w = 1.0f;
    output.position.z = 1.0f;

    // ��� ������ ������� ����. ����� ����� ��� ����� �������� ���� � �������� ���������
    int texsize = 10;
    float dist;

    // �� ������ ������ ��������� ���� ������ ����� ��� ����
    if(input.trailInfo.z == 1.0) {
    
        float dX = (input.instancePosition.x - input.trailInfo.x);
        float dY = (input.instancePosition.y - input.trailInfo.y);
        dist = sqrt(dX*dX + dY*dY);

        // ��������������� ��������
        input.position.x *= dist / texsize;
        input.position.y *= 0.33;

        input.position.x += texsize;
    }
    else {
        // ������� ������� ����
        input.position.x *= 2.0;
        input.position.y *= 0.5;
    }

    // ������� �� ����
    float Angle = input.instancePosition.z, Sin, Cos;

    // https://ru.wikipedia.org/wiki/�������_��������
    sincos(Angle, Sin, Cos);

    output.position.x = input.position.x * Cos - input.position.y * Sin;
    output.position.y = input.position.x * Sin + input.position.y * Cos;

    output.position.x += input.instancePosition.x;
    output.position.y += input.instancePosition.y;

    // ������� ����� �� �������� �����, ����� �� ��� ��� ��� �� ���� �� ������� �����
    if( input.trailInfo.z == 1.0 ) {
        output.position.x += (dist/2) * Cos;
        output.position.y += (dist/2) * Sin;
    }

    // ��������� ��������� �������������� ��� ���� �����
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates and other stuff for the pixel shader
    output.tex       = input.tex;
    output.trailInfo = input.trailInfo;

    return output;
}
