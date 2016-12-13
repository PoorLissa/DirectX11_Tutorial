// The vertex shader has now been modified to use instancing
//  роме того, этот вертексный шейдер теперь заточен исключительно под отрисовку пуль.
// ѕоскольку графически класс пули обслуживаетс€ классом инстанцированного спрайта, который, в свою очередь, опираетс€ на структуры VertexType/InstanceType,
// которые, оп€ть же, должны соответствовать структурам в шейдере и в классе шейдера, мы не мен€ем ничего во входных данных:
// VertexInputType остаетс€ такой же, как и в стандартном спрайте.
// ќднако, как интерпретировать входные данные, мы решаем уже здесь.
// ј вот cbuffer мы можем, например, помен€ть на такой, как нам удобно.

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float  rotationMode;        // Ќужно ли поворачивать все спрайты на одну точку
    float  rotateToX;           // X-координата точки, на которую нужно повернуть все спрайты
    float  rotateToY;           // Y-координата точки, на которую нужно повернуть все спрайты
    float  dummy;               //  <-- dummy
};

// The VertexInputType structure now has the third element which will hold the instanced input position data.
struct VertexInputType
{
    float4 position         : POSITION;
    float2 tex              : TEXCOORD0;
    float4 instancePosition : TEXCOORD1;    // xy = позици€, z = поворот на заданный угол, w = масштаб
    float3 trailInfo        : TEXCOORD2;    // xy = нулева€ позици€, z = признак пули (0 = пул€, 1 = след от пули, 2 = огненна€ пул€)
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

    // Ёто размер спрайта пули. Ќужно будет его потом передать сюда в качестве параметра
    static int texsize = 10;
    float dist = 0.0f, dX = 0.0f, dY = 0.0f;

    static unsigned int ionFactor = 5;

    // в случае пуль этот параметр указывает на тип пули / шлейфа
    switch( input.trailInfo.z )
    {
        // ќбычна€ пул€: немного сжимаем еЄ
        case 0.0f:
        case 1.0f:
            input.position.x *= 1.0f;
            input.position.y *= 0.5f;
        break;

        // огненна€ пул€
        case 2.0f:
            input.position.x *= 2.0f;
            input.position.y *= 2.0f;
        break;

        // ионна€ пул€
        case 3.0f:
            input.position.x *= ionFactor;
            input.position.y *= ionFactor;
        break;

        // тестова€ пул€
        case 3321.0f:
            input.position.x *= 10;
            input.position.y *= 10;
        break;

        // взрыв ионной пули
        case 4.0f:
            input.position.x *= input.instancePosition.w;
            input.position.y *= input.instancePosition.w;
        break;

        // шлейф обычной пули
        case 100.0f:
        case 101.0f:
            dX = (input.instancePosition.x - input.trailInfo.x);
            dY = (input.instancePosition.y - input.trailInfo.y);
            dist = sqrt(dX*dX + dY*dY);

            // масштабирование текстуры
            input.position.x *= dist / texsize;
            input.position.y *= 0.3f;

            input.position.x += texsize;
        break;

        // шлейф огненной пули
        case 102.0f:
            dX = (input.instancePosition.x - input.trailInfo.x);
            dY = (input.instancePosition.y - input.trailInfo.y);
            dist = sqrt(dX*dX + dY*dY);

            // масштабирование текстуры
            input.position.x *= dist / texsize;
            input.position.y *= 2.0f;

            input.position.x += 1.0f;
        break;

        // шлейф ионной пули
        case 103.0f:

            dX = (input.instancePosition.x - input.trailInfo.x);
            dY = (input.instancePosition.y - input.trailInfo.y);

            // делаем шлейф покороче, чем у обычной или огненной пули
            dist = ionFactor > 7 ? sqrt(dX*dX + dY*dY) : sqrt(dX*dX + dY*dY) / 1.5;

            // масштабирование текстуры
            input.position.x *= dist / texsize;
            input.position.y *= ionFactor;
        break;
    }



    // ¬ращаем на угол
    float Angle = input.instancePosition.z, Sin, Cos;

    // https://ru.wikipedia.org/wiki/ћатрица_поворота
    sincos(Angle, Sin, Cos);

    output.position.x = input.position.x * Cos - input.position.y * Sin;
    output.position.y = input.position.x * Sin + input.position.y * Cos;

    output.position.x += input.instancePosition.x;
    output.position.y += input.instancePosition.y;

    // смещаем хвост на половину длины, чтобы он шел как раз от пули до нулевой точки
    if( input.trailInfo.z > 99.0f ) {
        output.position.x += (dist * 0.5f) * Cos;
        output.position.y += (dist * 0.5f) * Sin;
    }

    // ƒобавл€ем матричные преобразовани€ дл€ всей сцены
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates and other stuff for the pixel shader
    output.tex       = input.tex;
    output.trailInfo = input.trailInfo;

    return output;
}
