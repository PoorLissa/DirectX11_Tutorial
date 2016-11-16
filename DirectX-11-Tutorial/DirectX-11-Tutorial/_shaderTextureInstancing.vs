// The vertex shader has now been modified to use instancing.

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// The VertexInputType structure now has the third element which will hold the instanced input position data.

struct VertexInputType
{
	float4 position			: POSITION;
	float2 tex				: TEXCOORD0;
	float3 instancePosition : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;
};

// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	//input.position.w = 1.0f;

	// Here is where we use the instanced position information to modify the position of each triangle we are drawing.
	// Update the position of the vertices based on the data for this particular instance.

/*
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

	// https://ru.wikipedia.org/wiki/Матрица_поворота
	float Cos = cos(input.instancePosition.z);
	float Sin = sin(input.instancePosition.z);
	//sincos(input.instancePosition.z, Sin, Cos);
	//float4 rotation = float4(Cos, -Sin, Sin, Cos);

	output.position.x = input.position.x * Cos - input.position.y * Sin;
	output.position.y = input.position.x * Sin + input.position.y * Cos;

	output.position.z = 1.0f;
	output.position.w = 1.0f;

	output.position.x += input.instancePosition.x;
	output.position.y += input.instancePosition.y;


	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);



	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

//	output.position.x += input.instancePosition.x;
//	output.position.y += input.instancePosition.y;

	return output;
}
