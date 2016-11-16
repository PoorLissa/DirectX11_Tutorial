#include "__colorShaderClass.h"

// для функции D3DCompileFromFile
#pragma comment(lib, "d3dcompiler.lib")
#include "D3Dcompiler.h"


ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../DirectX-11-Tutorial/_shaderColor.vs", L"../DirectX-11-Tutorial/_shaderColor.ps");

	if (!result)
		return false;

	return true;
}

void ColorShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
		return false;

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

// Now we will start with one of the more important functions to this tutorial which is called InitializeShader.
// This function is what actually loads the shader files and makes it usable to DirectX and the GPU.
// You will also see the setup of the layout and how the vertex buffer data is going to look on the graphics pipeline in the GPU.
// The layout will need the match the VertexType in the modelclass.h file as well as the one defined in the color.vs file.
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT		 result;
	ID3D10Blob	*errorMessage;
	ID3D10Blob	*vertexShaderBuffer;
	ID3D10Blob	*pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
	
	
	// Here is where we compile the shader programs into buffers. We give it the name of the shader file, the name of the shader,
	// the shader version(5.0 in DirectX 11), and the buffer to compile the shader into. If it fails compiling the shader it will put an
	// error message inside the errorMessage string which we send to another function to write out the error.
	// If it still fails and there is no errorMessage string then it means it could not find the shader file in which case we pop up a dialog box saying so.

	// Compile the vertex shader code.
	// D3DX11CompileFromFile is deprecated
	//result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if( FAILED(result) ) {

		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			// If there was nothing in the error message then it simply could not find the shader file itself.
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	// Compile the pixel shader code.
	// D3DX11CompileFromFile is deprecated
	//result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if( FAILED(result) ) {

		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			// If there was  nothing in the error message then it simply could not find the file itself.
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}


	// Once the vertex shader and pixel shader code has successfully compiled into buffers we then use those buffers to create the shader objects themselves.
	// We will use these pointers to interface with the vertex and pixel shader from this point forward.

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);

	if (FAILED(result))
		return false;

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);

	if (FAILED(result))
		return false;

	
	// The next step is to create the layout of the vertex data that will be processed by the shader.
	// As this shader uses a position and color vector we need to create both in the layout specifying the size of both.
	// The semantic name is the first thing to fill out in the layout, this allows the shader to determine the usage of this element of the layout.
	// As we have two different elements we use POSITION for the first one and COLOR for the second.
	// The next important part of the layout is the Format. For the position vector we use DXGI_FORMAT_R32G32B32_FLOAT and for the color
	// we use DXGI_FORMAT_R32G32B32A32_FLOAT.
	// The final thing you need to pay attention to is the AlignedByteOffset which indicates how the data is spaced in the buffer.
	// For this layout we are telling it the first 12 bytes are position and the next 16 bytes will be color, AlignedByteOffset shows where each element begins.
	// You can use D3D11_APPEND_ALIGNED_ELEMENT instead of placing your own values in AlignedByteOffset and it will figure out the spacing for you.
	// The other settings I've made default for now as they are not needed in this tutorial.

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	// Once the layout description has been setup we can get the size of it and then create the input layout using the D3D device.
	// Also release the vertex and pixel shader buffers since they are no longer needed once the layout has been created.

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	// The final thing that needs to be setup to utilize the shader is the constant buffer.
	// As you saw in the vertex shader we currently have just one constant buffer so we only need to setup one here so we can interface with the shader.
	// The buffer usage needs to be set to dynamic since we will be updating it each frame.
	// The bind flags indicate that this buffer will be a constant buffer. The cpu access flags need to match up with the usage so it is set to D3D11_CPU_ACCESS_WRITE.
	// Once we fill out the description we can then create the constant buffer interface and then use that to access the internal variables in the shader using the function SetShaderParameters.

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	if (FAILED(result))
		return false;

	return true;
}

// ShutdownShader releases the four interfaces that were setup in the InitializeShader function.
void ColorShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


// The OutputShaderErrorMessage writes out error messages that are generating when compiling either vertex shaders or pixel shaders.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("____shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
		fout << compileErrors[i];

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader. Check '__shader-error.txt' for message.", shaderFilename, MB_OK);

	return;
}


// The SetShaderVariables function exists to make setting the global variables in the shader easier.
// The matrices used in this function are created inside the GraphicsClass, after which this function is called to send them from there into the vertex shader during the Render function call.
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Make sure to transpose matrices before sending them into the shader, this is a requirement for DirectX 11.

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);


	// Lock the m_matrixBuffer, set the new matrices inside it, and then unlock it.

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
		return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);


	// Now set the updated matrix buffer in the HLSL vertex shader.

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}


// RenderShader is the second function called in the Render function.
// SetShaderParameters is called before this to ensure the shader parameters are setup correctly.
// --------------------------------------------------------------------------------------------------
// The first step in this function is to set our input layout to active in the input assembler.
// This lets the GPU know the format of the data in the vertex buffer.
// The second step is to set the vertex shader and pixel shader we will be using to render this vertex buffer.
// Once the shaders are set we render the triangle by calling the DrawIndexed DirectX 11 function using the D3D device context.
// Once this function is called it will render the green triangle.
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
