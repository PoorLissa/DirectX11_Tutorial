////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _TEXTOUTCLASS_H_
#define _TEXTOUTCLASS_H_

#include "__fontClass.h"
#include "__fontShaderClass.h"



class TextOutClass {

	// SentenceType is the structure that holds the rendering information for each text sentence
	struct SentenceType {
		ID3D11Buffer   *vertexBuffer, *indexBuffer;
		int             vertexCount, indexCount, maxLength;
		float           red, green, blue;
	};

	// The VertexType must match the one in the FontClass
	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

 public:
	TextOutClass();
	TextOutClass(const TextOutClass &);
   ~TextOutClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);

	// We now have two new functions for setting the fps count and the cpu usage.
	bool SetFps(int,    ID3D11DeviceContext *);
	bool SetCpu(int,    ID3D11DeviceContext *);
    bool SetText(char*, ID3D11DeviceContext *);

 private:
 	bool InitializeSentence(SentenceType **, int, ID3D11Device *);
	bool UpdateSentence(SentenceType *, char *, int, int, float, float, float, ID3D11DeviceContext *);
	void ReleaseSentence(SentenceType **);
	bool RenderSentence(ID3D11DeviceContext *, SentenceType *, D3DXMATRIX, D3DXMATRIX);

 private:
	FontClass		*m_Font;
	FontShaderClass	*m_FontShader;
	int				 m_screenWidth, m_screenHeight;
	D3DXMATRIX		 m_baseViewMatrix;

	// We will use two sentences in this tutorial
	SentenceType	*m_sentence1;
	SentenceType	*m_sentence2;

    // ��������� ������ �����, �����  �������� ������������ �����
    SentenceType	*m_sentence3;
};

#endif
