// The vertex shader has now been modified to use instancing

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

// The VertexInputType structure now has the third element which will hold the instanced input position data
struct VertexInputType
{
    float4 position         : POSITION;
    float2 tex              : TEXCOORD0;
    float4 instancePosition : TEXCOORD1;    // x = позиция x, y = позиция y, z = поворот на заданный угол, w = масштаб
    float3 animationInfo    : TEXCOORD2;
};

struct PixelInputType
{
    float4 position      : SV_POSITION;
    float2 tex           : TEXCOORD0;
    float3 animationInfo : TEXCOORD2;
};



// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

	// изменим масштаб спрайта, если коэффициент масштабирования не равен единице
	if( input.instancePosition.w != 1.0f ) {
		input.position.x *= input.instancePosition.w;
		input.position.y *= input.instancePosition.w;
	}

	//input.position.x *= 2.0;
    //input.position.y *= 2.0;

    // Change the position vector to be 4 units for proper matrix calculations
    //input.position.w = 1.0f;

    // --- Here is where we use the instanced position information to modify the position of each triangle we are drawing ---
    float Angle, Sin, Cos;

    if( rotationMode == 0.0f ) {

        // Учитываем только вращение на угол, который задается для каждой инстанции
        Angle = input.instancePosition.z;

    } else {

        // Поворачиваем спрайт так, что он смотрит на указанную точку в пространстве
        static const float PI = 3.14159265f;
        static const float divPIby180 = PI / 180.0f;
        static const float div180byPI = 180.0f / PI;

        float dX = input.instancePosition.x - rotateToX;
        float dY = input.instancePosition.y - rotateToY;

        if (dX == 0.0f) {
            Angle = dY > 0.0f ? 180.0f : 0.0f;
        }
        else {
            Angle = atan(dY / dX) * div180byPI;
            Angle = dX > 0.0f ? Angle + 90.0f : Angle + 270.0f;
        }

        // Angle to Radians:
        if( rotationMode == 2.0 )
            Angle = (Angle - 90.0f) * divPIby180;								// Без учета начального угла
        else
            Angle = (Angle + input.instancePosition.z) * divPIby180;    // С учетом начального угла
    }



    // https://ru.wikipedia.org/wiki/Матрица_поворота
    /*
    float Cos = cos(input.instancePosition.z);
    float Sin = sin(input.instancePosition.z);
    */
    sincos(Angle, Sin, Cos);
    //float4 rotation = float4(Cos, -Sin, Sin, Cos);

    output.position.x = input.position.x * Cos - input.position.y * Sin;
    output.position.y = input.position.x * Sin + input.position.y * Cos;

    output.position.w = 1.0f;
    output.position.z = 1.0f;

    output.position.x += input.instancePosition.x;
    output.position.y += input.instancePosition.y;

    // Добавляем матричные преобразования для всей сцены
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates and other stuff for the pixel shader
    output.tex           = input.tex;
    output.animationInfo = input.animationInfo;

    return output;
}

/*
// some rotation example, didn't work though

float x = 0.0;
float y = 0.0;
float z = 1.0;

float3 p = input.instancePosition.xyz;
float3 q;

q.x = p.x * (x*x * (1.0 - cos(angle)) +     cos(angle))
+ p.y * (x*y * (1.0 - cos(angle)) + z * sin(angle))
+ p.z * (x*z * (1.0 - cos(angle)) - y * sin(angle));

q.y = p.x * (y*x * (1.0 - cos(angle)) - z * sin(angle))
+ p.y * (y*y * (1.0 - cos(angle)) +     cos(angle))
+ p.z * (y*z * (1.0 - cos(angle)) + x * sin(angle));

q.z = p.x * (z*x * (1.0 - cos(angle)) + y * sin(angle))
+ p.y * (z*y * (1.0 - cos(angle)) - x * sin(angle))
+ p.z * (z*z * (1.0 - cos(angle)) +     cos(angle));
*/

// Apply the rotation to our 2d position:

// Calculate the position of the vertex against the world, view, and projection matrices.
// Compute a 2x2 rotation matrix.
/*
rot *= 0.5;
rot += 0.5;
rot = rot * 2 - 1;
*/
