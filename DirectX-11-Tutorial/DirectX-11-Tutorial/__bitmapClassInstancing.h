#pragma once
// --------------------------------------------------------------------------------------------------------
// BitmapClass will be used to represent an individual 2D image that needs to be rendered to the screen.
// For every 2D image you have you will need a new BitmapClass for each.
// Note that this class is just the ModelClass re-written to handle 2D images instead of 3D objects.
// --------------------------------------------------------------------------------------------------------

#include <d3d11.h>
#include <d3dx10math.h>
#include <vector>
#include <list>

#include "__textureClass.h"
#include "__textureClass_Array.h"
#include "Helpers.h"

#define ciRef const int &
#define cfRef const float &

class BitmapClass_Instancing {

 protected:
	// Each bitmap image is still a polygon object that gets rendered similar to 3D objects.
	// For 2D images we just need a position vector and texture coordinates.

	    struct VertexType {
		    D3DXVECTOR3 position;
		    D3DXVECTOR2 texture;
	    };

	// We add a new structure that will hold the instance information.
	// In this tutorial we are modifying the position of each instance of the triangle so we use a position vector.
	// But note that it could be anything else you want to modify for each instance such as color, size, rotation, and so forth.
	// You can modify multiple things at once for each instance also.

        // float3 position содержит 2 координаты, по которым будет размещен спрайт, и угол поворота, на который этот спрайт нужно развернуть
        // float3 содержит: а) ширину одного спрайта, б) высоту одного спрайта, в) номер анимации, который нужно использовать
	    struct InstanceType {
		    D3DXVECTOR3  position;	    
            D3DXVECTOR3  animationInfo; 
	    };


 public:
	BitmapClass_Instancing();
	BitmapClass_Instancing(const BitmapClass_Instancing &);
   ~BitmapClass_Instancing();

	bool Initialize(ID3D11Device *, int, int, WCHAR *, int, int);					                            // инициализация одной текстурой
    //bool Initialize(ID3D11Device *, int, int, WCHAR *, WCHAR *, int, int);		                            // инициализация двумя текстурами - для массива текстур
    bool Initialize(ID3D11Device *, ciRef, ciRef, WCHAR**, ciRef, ciRef, ciRef, cfRef = 0.0f, cfRef = 0.0f);    // инициализация масивом текстур

	void Shutdown();
	bool Render(ID3D11DeviceContext *);

    // The GetTexture or GetTextureArray functions return a pointer to the texture resource for this 2D image
    // The shader will call one of these functions so it has access to the image when drawing the buffers
    inline ID3D11ShaderResourceView*  GetTexture()       { return m_Texture->GetTexture();           }
    inline ID3D11ShaderResourceView** GetTextureArray()  { return m_TextureArray->GetTextureArray(); }

	// Получить количества вертексов и инстанций
    inline int GetVertexCount()     { return m_vertexCount;   }
    inline int GetInstanceCount()   { return m_instanceCount; }

    // Получить размеры битмапа, переданные в класс при инициализации
    inline int getBitmapWidth()     { return m_bitmapWidth;   }
    inline int getBitmapHeight()    { return m_bitmapHeight;  }

	bool initializeInstances(ID3D11Device *);


 protected:
    bool InitializeBuffers(ID3D11Device *, const int &, const int &);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext *);

	bool LoadTexture(ID3D11Device *, WCHAR *);					// загрузка одной текстуры
    bool LoadTexture(ID3D11Device *, WCHAR *, WCHAR *);			// загрузка двух текстур
	bool LoadTexture(ID3D11Device *, WCHAR**, ciRef);           // загрузка массива текстур

	void ReleaseTexture();										// освобождение текстур


 protected:
	ID3D11Buffer	    *m_vertexBuffer;
	int				     m_vertexCount;
	TextureClass	    *m_Texture;
    TextureArrayClass   *m_TextureArray;

	// The BitmapClass will need to maintain some extra information that a 3D model wouldn't,
	// such as the screen size, the bitmap size, and the last place it was rendered.
	// We have added extra private variables here to track that extra information.
	int m_screenWidth,  m_screenHeight;
	int m_bitmapWidth,  m_bitmapHeight;

	// The BitmapClass now has an instance buffer instead of an index buffer.
	// All buffers in DirectX 11 are generic so it uses the ID3D11Buffer type.
	ID3D11Buffer	*m_instanceBuffer;

	// The index count has been replaced with the instance count
	int				 m_instanceCount;

    // Размеры одного кадра анимации из текстурного атласа
    float            m_spriteSliceX, m_spriteSliceY;
};
