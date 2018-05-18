//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dutility.cpp
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"



// vertex formats
const DWORD d3d::Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


bool d3d::InitD3D(
	int width, int height,
	bool windowed,	
	HWND *hwnd,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9** device/*, HWND button[]*/ )
{


	//
	// Инициализация D3D: 
	//

	HRESULT hr = 0;

	// Шаг 1: Создание IDirect3D9 объекта.  

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9( D3D_SDK_VERSION );

    if( !d3d9 ){
		::MessageBox( 0, L"Direct3DCreate9() - FAILED", 0, 0 );
		return false;
	}

	
	// Шаг 2: Проверка настроек gpu.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps( D3DADAPTER_DEFAULT, deviceType, &caps );

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Шаг 3: Заполнение D3DPRESENT_PARAMETERS структуры.
	 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 0;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONMASKABLE;
	d3dpp.MultiSampleQuality         = NULL;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow              = *hwnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Шаг 4: Создание устройства.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // первичный адаптер
		deviceType,         // тип устройства
		*hwnd,               // свзязь окна с устройством
		vp,                 // вершинная обработка
	    &d3dpp,             // параметры показа
	    device);            // созданной устройство

	if( FAILED(hr) )
	{
		
		// Попытка использования  16-битный глубинный буффер

		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			*hwnd,
			vp,
			&d3dpp,
			device );

		if( FAILED( hr ) )
		{
			d3d9->Release(); 
			::MessageBox( 0, L"CreateDevice() - FAILED", 0, 0 );
			return false;
		}
	}

	d3d9->Release(); 
	
	return true;
}

bool d3d::HitInObject( IDirect3DDevice9* pDevice,	// устройство Direct3D
						POINT* pScrCoord,			// экранные координаты
						ID3DXMesh* pObjectMsh,		// сетка объекта
						D3DXMATRIX* world )		// мировая матрица для объекта, точь в точки как при отрисовке смого объекта
{
	
	D3DXMATRIX view, proj;
	D3DVIEWPORT9 viewport;
	D3DXVECTOR3 vRayDirNDC;
    D3DXVECTOR3 vRayDirB;
    D3DXVECTOR3 vNPt;
    D3DXVECTOR3 vNPtB;

	D3DXVECTOR3* pEye;
    D3DXVECTOR3* pRayDir;

	BOOL bHit;
    DWORD uFaceIndex = 0;
    float fU, fV, fG;
    float fDist;      

    vRayDirNDC	= D3DXVECTOR3( ( float )pScrCoord->x, ( float )pScrCoord->y, 0.0f );
    vRayDirB	= D3DXVECTOR3( ( float )pScrCoord->x, ( float )pScrCoord->y, 1.0f );

	pDevice->GetTransform( D3DTS_PROJECTION, &proj );
	pDevice->GetTransform( D3DTS_VIEW, &view );		
	pDevice->GetViewport( &viewport );
		
	D3DXVec3Unproject( &vNPt, &vRayDirNDC, &viewport, &proj, &view, world );
	D3DXVec3Unproject( &vNPtB, &vRayDirB,  &viewport, &proj, &view, world );
	vNPtB = vNPtB - vNPt;//???????????????
	pEye = &vNPt;
	pRayDir = &vNPtB;
	D3DXIntersect( pObjectMsh, pEye, pRayDir, &bHit, &uFaceIndex, &fV, &fG, &fDist, NULL, NULL );

	return bHit;
}

D3DLIGHT9 d3d::InitDirectionalLight( D3DXVECTOR3* direction, D3DXCOLOR* color )
{
	D3DLIGHT9 light;
	::ZeroMemory( &light, sizeof( light ) );

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight( D3DXVECTOR3* position, D3DXCOLOR* color )
{
	D3DLIGHT9 light;
	::ZeroMemory( &light, sizeof( light ) );

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight( D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color )
{
	D3DLIGHT9 light;
	::ZeroMemory( &light, sizeof( light ) );

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}

D3DMATERIAL9 d3d::InitMtrl( D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p )
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}


d3d::BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

void d3d::InitFont( IDirect3DDevice9 * pDevice, int size,  LPCWSTR nameFont, ID3DXFont **pFont ){
	HRESULT hResult = D3DXCreateFontW( pDevice, size, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
									DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,  nameFont,  pFont );
	if ( hResult != S_OK ){ 
		::MessageBox( 0, L"d3d::InitFont() - FAILED!", L"error", 0 );
		return ;
	}
}

void d3d::DrawMyText( ID3DXFont * pFont, LPCWSTR text, LPRECT rect, D3DXCOLOR color ){	
	pFont->DrawTextW( NULL, text, -1, rect, DT_LEFT | DT_TOP, color );
	
}

 std::wstring d3d::ToWstring(const std::string& text){
	 //не фурычит
	 using namespace stdext::cvt;
    std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>,wchar_t> conv;
    return conv.from_bytes( text.c_str() );
}

 VOID d3d::LoadXfile( LPCWSTR nameXfile, IDirect3DDevice9* device, ID3DXMesh** outMesh, 
		std::vector<D3DMATERIAL9>* outMtrl, std::vector<IDirect3DTexture9*>* outTextures )
{
	//Загрузка сетки
	
	HRESULT hr = NULL;
	ID3DXBuffer* adjBuffer = NULL;
	ID3DXBuffer* mtrljBuffer = NULL;
	DWORD numMtrls = NULL;

	hr = D3DXLoadMeshFromXW( nameXfile, D3DXMESH_MANAGED, device, &adjBuffer, &mtrljBuffer, NULL, &numMtrls, &*outMesh ); 
	if( FAILED( hr ) )		
	{
		::MessageBox( 0, L"D3DMeshFromFileX() - Failed", 0, 0 );
	}

	//Извлечение материала и загрузка текстур
	if ( mtrljBuffer != NULL && numMtrls != NULL ){
		D3DXMATERIAL * mtrls = (D3DXMATERIAL*)mtrljBuffer->GetBufferPointer();
		for( int i=0; i < numMtrls; i++		){
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;
			//Сохранение материала
			if ( outMtrl != NULL )
				outMtrl->push_back( mtrls[i].MatD3D );
			//Проверяем, связана ли с i-тым материалом текстура
			if( mtrls[i].pTextureFilename != NULL )
			{
				IDirect3DTexture9* tex = NULL;
				char*  filename = mtrls[i].pTextureFilename;
				int len = strlen( filename );
				wchar_t* textureFilename = new wchar_t [len];				
				for( int h = 0; h < len; h++ )
					textureFilename[h] = filename[h];
				textureFilename[len] = '\0';
				D3DXCreateTextureFromFile( device, textureFilename, &tex );
				//Соxраняем текстуру, если пользователь разрешил
				if( outTextures != NULL )
					outTextures->push_back( tex );
			}
			else if( outTextures != NULL )
					outTextures->push_back( NULL );

		}
	}
	//Уничтожаем неиспользуемые переменные
	d3d::Release<ID3DXBuffer*> ( mtrljBuffer );
	d3d::Release<ID3DXBuffer*> ( adjBuffer );
}

