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
    float4 instancePosition : TEXCOORD1;    // xy = �������, z = ������� �� �������� ����, w = �������
    float3 trailInfo        : TEXCOORD2;    // xy = ������� �������, z = ������� ���� (0 = ����, 1 = ���� �� ����, 2 = �������� ����)
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
    static int texsize = 10;
    float dist = 0.0f, dX = 0.0f, dY = 0.0f;

    static unsigned int ionFactor = 5;

    // � ������ ���� ���� �������� ��������� �� ��� ���� / ������
    switch( input.trailInfo.z )
    {
        // ������� ����: ������� ������� �
        case 0.0f:
        case 1.0f:
            input.position.x *= 1.0f;
            input.position.y *= 0.5f;
        break;

        // �������� ����
        case 2.0f:
            input.position.x *= 2.0f;
            input.position.y *= 2.0f;
        break;

        // ������ ����
        case 3.0f:
            input.position.x *= ionFactor;
            input.position.y *= ionFactor;
        break;

        // �������� ����
        case 3321.0f:
            input.position.x *= 10;
            input.position.y *= 10;
        break;

        // ����� ������ ����
        case 4.0f:
            input.position.x *= input.instancePosition.w;
            input.position.y *= input.instancePosition.w;
        break;

        // ����� ������� ����
        case 100.0f:
        case 101.0f:
            dX = (input.instancePosition.x - input.trailInfo.x);
            dY = (input.instancePosition.y - input.trailInfo.y);
            dist = sqrt(dX*dX + dY*dY);

            // ��������������� ��������
            input.position.x *= dist / texsize;
            input.position.y *= 0.3f;

            input.position.x += texsize;
        break;

        // ����� �������� ����
        case 102.0f:
            dX = (input.instancePosition.x - input.trailInfo.x);
            dY = (input.instancePosition.y - input.trailInfo.y);
            dist = sqrt(dX*dX + dY*dY);

            // ��������������� ��������
            input.position.x *= dist / texsize;
            input.position.y *= 2.0f;

            input.position.x += 1.0f;
        break;

        // ����� ������ ����
        case 103.0f:

            dX = (input.instancePosition.x - input.trailInfo.x);
            dY = (input.instancePosition.y - input.trailInfo.y);

            // ������ ����� ��������, ��� � ������� ��� �������� ����
            dist = ionFactor > 7 ? sqrt(dX*dX + dY*dY) : sqrt(dX*dX + dY*dY) / 1.5;

            // ��������������� ��������
            input.position.x *= dist / texsize;
            input.position.y *= ionFactor;
        break;
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
    if( input.trailInfo.z > 99.0f ) {
        output.position.x += (dist * 0.5f) * Cos;
        output.position.y += (dist * 0.5f) * Sin;
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
