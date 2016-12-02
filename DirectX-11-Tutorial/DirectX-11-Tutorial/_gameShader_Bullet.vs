// The vertex shader has now been modified to use instancing
// Кроме того, этот вертексный шейдер теперь заточен исключительно под отрисовку пуль.
// Поскольку графически класс пули обслуживается классом инстанцированного спрайта, который, в свою очередь, опирается на структуры VertexType/InstanceType,
// которые, опять же, должны соответствовать структурам в шейдере и в классе шейдера, мы не меняем ничего во входных данных:
// VertexInputType остается такой же, как и в стандартном спрайте.
// Однако, как интерпретировать входные данные, мы решаем уже здесь.
// А вот cbuffer мы можем, например, поменять на такой, как нам удобно.

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float  rotationMode;        // Нужно ли поворачивать все спрайты на одну точку
    float  rotateToX;           // X-координата точки, на которую нужно повернуть все спрайты
    float  rotateToY;           // Y-координата точки, на которую нужно повернуть все спрайты
    float  dummy;               //  <-- dummy
};

// The VertexInputType structure now has the third element which will hold the instanced input position data.
struct VertexInputType
{
    float4 position         : POSITION;
    float2 tex              : TEXCOORD0;
    float3 instancePosition : TEXCOORD1;    // xy = позиция, z = поворот на заданный угол
    float3 trailInfo        : TEXCOORD2;    // xy = нулевая позиция, z = признак пули (0 = пуля, 1 = след от пули)
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

    // Это размер спрайта пули. Нужно будет его потом передать сюда в качестве параметра
    int texsize = 10;
    float dist;

    // из каждой второй инстанции пуль делаем шлейф для пули
    if(input.trailInfo.z == 1.0) {
    
        float dX = (input.instancePosition.x - input.trailInfo.x);
        float dY = (input.instancePosition.y - input.trailInfo.y);
        dist = sqrt(dX*dX + dY*dY);

        // масштабирование текстуры
        input.position.x *= dist / texsize;
        input.position.y *= 0.33;

        input.position.x += texsize;
    }
    else {
        // Немного сжимаем пулю
        input.position.x *= 2.0;
        input.position.y *= 0.5;
    }

    // Вращаем на угол
    float Angle = input.instancePosition.z, Sin, Cos;

    // https://ru.wikipedia.org/wiki/Матрица_поворота
    sincos(Angle, Sin, Cos);

    output.position.x = input.position.x * Cos - input.position.y * Sin;
    output.position.y = input.position.x * Sin + input.position.y * Cos;

    output.position.x += input.instancePosition.x;
    output.position.y += input.instancePosition.y;

    // смещаем хвост на половину длины, чтобы он шел как раз от пули до нулевой точки
    if( input.trailInfo.z == 1.0 ) {
        output.position.x += (dist/2) * Cos;
        output.position.y += (dist/2) * Sin;
    }

    // Добавляем матричные преобразования для всей сцены
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates and other stuff for the pixel shader
    output.tex       = input.tex;
    output.trailInfo = input.trailInfo;

    return output;
}
