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
	input.position.w = 1.0f;

	// Here is where we use the instanced position information to modify the position of each triangle we are drawing.

	// Update the position of the vertices based on the data for this particular instance.
	input.position.x += input.instancePosition.x + 333.3;
	input.position.y += input.instancePosition.y - 333.3;
	//input.position.z += input.instancePosition.z;

	//float2x2 rotation = float2x2(cos(input.instancePosition.z), sin(input.instancePosition.z), -sin(input.instancePosition.z), cos(input.instancePosition.z));

	//float angle = input.instancePosition.z;
	float angle = 2.0;

	float x = 1.0;
	float y = 1.0;
	float z = 1.0;

	// works!
/*
	input.position.x += 33.0 * input.instancePosition.z;
	input.position.y -= 10 * input.instancePosition.z - 10*sin(10 * input.instancePosition.z);
*/

/*
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

	// Apply the rotation to our 3D position:


	// Calculate the position of the vertex against the world, view, and projection matrices.
/*
	output.position = mul(input.position,  worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
*/

	// Compute a 2x2 rotation matrix.
	float c = cos(angle);
	float s = sin(angle);
	float4 rot = float4(c, -s, s, c);
	rot *= 0.5;
	rot += 0.5;

	rot = rot * 2 - 1;


	//output.position = mul(input.position, float3x3(rot));
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//output.position = mul(output.position, rot);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	return output;
}
