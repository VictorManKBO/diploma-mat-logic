//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dutility.h
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __d3dUtilityH__
#define __d3dUtilityH__
#define POINTER_64 __ptr64
#include <E:\Program Files\DirectX SDK\Include\d3d9.h>
#include <E:\Program Files\DirectX SDK\Include\d3dx9.h>
#include <E:\Program Files\DirectX SDK\Include\D3dx9core.h>
#include <string>
#include <limits>
#include <vector>
#include <cvt/wstring>
#include <locale>

#pragma comment( lib,"E:\\Program Files\\DirectX SDK\\Lib\\x86\\d3d9.lib" )
#pragma comment( lib,"E:\\Program Files\\DirectX SDK\\Lib\\x86\\d3dx9.lib" )
#pragma comment( lib,"winmm.lib" )
#pragma comment( lib,"E:\\Program Files\\DirectX SDK\\Lib\\x86\\d3dx9.lib" )





namespace d3d{
	//
	// Init
	//
	bool InitD3D(
		int width, int height,     // [in] Размер Backbuffer.
		bool windowed,             // [in] Оконный (true) или полноэкранный (false).
		HWND *hwnd,
		D3DDEVTYPE deviceType,     // [in] HAL или REF
		IDirect3DDevice9** device // [out]Созданной устройство.
				); 

		
	bool HitInObject(	IDirect3DDevice9* pDevice,	// устройство Direct3D
						POINT* pScrCoord,			// экранные координаты
						ID3DXMesh* pObjectMsh,		// сетка объекта
						D3DXMATRIX* world );		// мировая матрица для объекта, точь в точки как при отрисовке смого объекта

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg, 
		WPARAM wParam,
		LPARAM lParam );

	//
	// font
	//
	
	void InitFont( IDirect3DDevice9 * pDevice, int size,  LPCWSTR nameFont, ID3DXFont **pFont );
	void DrawMyText( ID3DXFont *pFont, LPCWSTR text, LPRECT pos, D3DXCOLOR color );
	//
	// End font
	//

	VOID LoadXfile( LPCWSTR nameXfile, IDirect3DDevice9* device, ID3DXMesh** outMesh, 
		std::vector<D3DMATERIAL9>* outMtrl, std::vector<IDirect3DTexture9*>* outTextures );

	//
	// wstring
	//
	std::wstring ToWstring(const std::string& text);

	//
	// Cleanup
	//
	template<class T> void Release( T t )
	{
		if( t )
		{
			t->Release();
			t = 0;
		}
	}
		
	template<class T> void Delete( T t )
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}

	//
	// Colors
	//
	const D3DXCOLOR      WHITE( D3DCOLOR_XRGB( 255, 255, 255 ) );
	const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(   0,   0,   0 ) );
	const D3DXCOLOR        RED( D3DCOLOR_XRGB( 255,   0,   0 ) );
	const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(   0, 255,   0 ) );
	const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(   0,   0, 255 ) );
	const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB (255, 255,   0 ) );
	const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(   0, 255, 255 ) );
	const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB( 255,   0, 255 ) );



	//
	// Световые источники
	//

	D3DLIGHT9 InitDirectionalLight( D3DXVECTOR3* direction, D3DXCOLOR* color );
	D3DLIGHT9 InitPointLight( D3DXVECTOR3* position, D3DXCOLOR* color );
	D3DLIGHT9 InitSpotLight( D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color );

	//
	// Материалы
	//

	D3DMATERIAL9 InitMtrl( D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p );

	const D3DMATERIAL9 WHITE_MTRL  = InitMtrl( WHITE, WHITE, WHITE, BLACK, 2.0f );
	const D3DMATERIAL9 RED_MTRL    = InitMtrl( RED, RED, RED, BLACK, 2.0f );
	const D3DMATERIAL9 GREEN_MTRL  = InitMtrl( GREEN, GREEN, GREEN, BLACK, 2.0f );
	const D3DMATERIAL9 BLUE_MTRL   = InitMtrl( BLUE, BLUE, BLUE, BLACK, 2.0f );
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl( YELLOW, YELLOW, YELLOW, BLACK, 2.0f );
	const D3DMATERIAL9 BLACK_MTRL  = InitMtrl( BLACK, BLACK, BLACK, BLACK, 2.0f );

	//
	// Bounding Objects / Math Objects
	//


	struct BoundingSphere
	{
		BoundingSphere();

		D3DXVECTOR3 _center;
		float       _radius;
	};

	 bool ComputeBoundingSphere( ID3DXMesh* mesh, d3d::BoundingSphere* sphere );

	
	//
	// Vertex Structures
	//

	struct Vertex
	{
		
		Vertex(float x, float y, float z, 
			float nx, float ny, float nz,
			float u, float v)
		{
			_x  = x;  _y  = y;  _z  = z;
			_nx = nx; _ny = ny; _nz = nz;
			_u  = u;  _v  = v;
		}
		float _x, _y, _z;
		float _nx, _ny, _nz;
		float _u, _v;

		static const DWORD FVF;
	};
}

#endif // __d3dUtilityH__