//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: главный.cpp
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0 
//
// Desc: The main program file.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////
#define POINTER_64 __ptr64
#include "interface.h"
#include "d3dUtility.h"
#include "map.h"
#include "control.h"
#include <string.h>
//#include <cvt/wstring>
#include <math.h>
#include <vector>
#include <stack>
#include <ctime>
#include <fstream>


//
// Globals
//

IDirect3DDevice9* pDevice = NULL; 
ID3DXFont * pMyFont;

int width  = 0;// 0 - фулл скрин
int height = 0;// 0 - фулл скрин

int level = NILL;

HWND elem[LEVEL::HARD+1] = { NULL };

HWND menu_elem[smh::COUNT] = { NULL };

LPWSTR rules;


IDirect3DTexture9* pTexture = NULL;
IDirect3DVertexBuffer9* pBufferVershin = NULL;
IDirect3DIndexBuffer9* pBufferIndex = NULL;

ID3DXFont * pFont = NULL;      // Шрифт Diect3D
RECT Rec;       // Прямоугольник


d3d::BoundingSphere BSphere;
ID3DXMesh* pSphereMsh = NULL;
ID3DXMesh* pLgcObj[LOGICOPERATOR::COUNT+3] = { NULL };
ID3DXMesh* pCell = NULL;
std::vector<IDirect3DTexture9*> pCellTxtr( NULL );
std::vector<D3DMATERIAL9> pCellMtrl( NULL );


ID3DXMesh* pPipeMsh[pipeconst::COUNT] = { NULL };
ID3DXMesh* pServerMsh =  NULL;
std::vector<IDirect3DTexture9*> pServerTxtr( NULL );
std::vector<D3DMATERIAL9> pServerMtrl( NULL );


//Стрелка вектора магнитной индукции
ID3DXMesh* pVecMagnInd = NULL;
std::vector<D3DMATERIAL9> vec_magnindMtrl( NULL );
std::vector<IDirect3DTexture9*> vec_magnindTextures( NULL );

MAP map;
CONTROL ctrl;

D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 target( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );

float deltaX; // прирощение координаты
float v0 = 0.0f;	// скорость заряда
D3DXVECTOR3 b( 1.0f, 0.0f, 0.0f );
D3DXVECTOR3 v( 1.0f, 0.0f, 0.0f );
float q = 1.6f * 100;
float m = 1.67f;
//float time;
float chargeX = -300.0f;
float chargeY = 0.0f;
float chargeZ = 0.0f;
float dv = -1.0f, angle = 0.0f;
float dx, dy, dz;
float dx1, dy1, dz1;
#define TAILLEN 200
struct {
	float x, y, z;
	float time;
} tail[TAILLEN] ;

#define mx( y ) ( dx1 - dx )*( y - dy )/( dy1 - dy ) + dx
#define mz( y ) ( dz1 - dz )*( y - dy )/( dy1 - dy ) + dz

float angle_camX = 0.0f, angle_camY = 0.0f, rad_cam = 1300.0f;


//
// Functions
//



int Sign( float a ){
	if( a > 0.0f ) return 1;
	else if( a < 0.0f ) return -1;
	else return 0;
}



BOOL Setup(){
	
	//
	//Инициализации классов
	//
	
	//
	//Загрузка X - объекта
	//
	d3d::LoadXfile( L"nout.x", pDevice, &pCell, &pCellMtrl, &pCellTxtr );
	d3d::LoadXfile( L"and.x", pDevice, &pLgcObj[LOGICOPERATOR::AND], NULL, NULL );
	d3d::LoadXfile( L"or.x", pDevice, &pLgcObj[LOGICOPERATOR::OR], NULL, NULL );
	d3d::LoadXfile( L"xor.x", pDevice, &pLgcObj[LOGICOPERATOR::XOR], NULL, NULL );
	d3d::LoadXfile( L"implic.x", pDevice, &pLgcObj[LOGICOPERATOR::IMPLICATION], NULL, NULL );
	d3d::LoadXfile( L"zero.x", pDevice, &pLgcObj[LOGICOPERATOR::LOGICZERO], NULL, NULL );
	d3d::LoadXfile( L"one.x", pDevice, &pLgcObj[LOGICOPERATOR::LOGICONE], NULL, NULL );

	d3d::LoadXfile( L"duga.x", pDevice, &pPipeMsh[pipeconst::VERTDUGA], NULL, NULL );
	d3d::LoadXfile( L"dugahor.x", pDevice, &pPipeMsh[pipeconst::HORDUGA], NULL, NULL );
	d3d::LoadXfile( L"up.x", pDevice, &pPipeMsh[pipeconst::UP], NULL, NULL );
	d3d::LoadXfile( L"leftright_uppipe.x", pDevice, &pPipeMsh[pipeconst::LEFTRIGHT_UP], NULL, NULL );
	d3d::LoadXfile( L"leftright_downpipe.x", pDevice, &pPipeMsh[pipeconst::LEFTRIGHT_DOWN], NULL, NULL );
	d3d::LoadXfile( L"upright_pipe.x", pDevice, &pPipeMsh[pipeconst::UPRIGHT], NULL, NULL );
	d3d::LoadXfile( L"downright_pipe.x", pDevice, &pPipeMsh[pipeconst::DOWNRIGHT], NULL, NULL );
	d3d::LoadXfile( L"ball.x", pDevice, &pPipeMsh[pipeconst::BALL], NULL, NULL );
	d3d::LoadXfile( L"fat-up.x", pDevice, &pPipeMsh[pipeconst::FATUP], NULL, NULL );

	
	d3d::LoadXfile( L"sphere.x", pDevice, &pSphereMsh , NULL, NULL );
	d3d::LoadXfile( L"server.x", pDevice, &pServerMsh , &pServerMtrl, &pServerTxtr );
	
	
	//
	// Настройка света
	// 
	
	D3DXCOLOR col( 1.0f, 1.0f, 1.0f, 1.0f );	
	D3DXVECTOR3 direction( 0.0f,  0.0f, 1.0f );
	D3DLIGHT9 light = d3d::InitDirectionalLight( &direction, &col );
	pDevice->SetLight( 0, &light );
	pDevice->LightEnable( 0, true );
	pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, true );
	pDevice->SetRenderState( D3DRS_SPECULARENABLE, false );	

	//
	// Установка матрицы вида - в Display()  идёт перерасчёт на кажыдй цикл
	//
	angle_camY = D3DX_PI / 6;


	//
	// Установка матрицы проекции
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.4f, // 45 - градусов
			(float)width / (float)height,
			1.0f,
			2500.0f );
	pDevice->SetTransform( D3DTS_PROJECTION, &proj );

	return true;
}

VOID Cleanup()
{
	
	d3d::Release<ID3DXFont*>( pMyFont );
	d3d::Release<ID3DXMesh*>( pSphereMsh );
	d3d::Release<ID3DXMesh*>( pCell );
	d3d::Release<ID3DXMesh*>( pServerMsh );
	
	d3d::Release<ID3DXMesh*>( pVecMagnInd );
	for( int i = 0; i < LOGICOPERATOR::COUNT+3; i++ )
		d3d::Release<ID3DXMesh*> (pLgcObj[i] );
	for( int i = 0; i < pipeconst::COUNT; i++ )
		d3d::Release<ID3DXMesh*> (pPipeMsh[i] );

	for( int i = 0; i < vec_magnindTextures.size(); i++ )
		d3d::Release<IDirect3DTexture9*>( vec_magnindTextures[i] );
	vec_magnindMtrl.resize( NULL );
	for( int i = 0; i < pServerTxtr.size(); i++ )
		d3d::Release<IDirect3DTexture9*>(  pServerTxtr[i] );
	 pServerTxtr.resize( NULL );
	 for( int i = 0; i < pCellTxtr.size(); i++ )
		d3d::Release<IDirect3DTexture9*>(  pCellTxtr[i] );
	 pCellTxtr.resize( NULL );
	 

	if( pTexture != NULL )
        pTexture->Release();

     if( pBufferIndex  != NULL )
        pBufferIndex->Release(); 

    if( pBufferVershin  != NULL )
        pBufferVershin->Release(); 
	if( pFont  != NULL )
        pFont->Release();
	
	
	for( int i = 0; i < vec_magnindTextures.size(); i++ )
		d3d::Release<IDirect3DTexture9*>( vec_magnindTextures[i] );
	vec_magnindMtrl.resize( NULL );
	d3d::Release<IDirect3DTexture9*>( pTexture );

	map.Release();
	ctrl.Release();
}

bool Display( float timeDelta )
{
	if( pDevice )
	{
		
		//
		// Центровка камеры и Обновление точки наблюдателя на сфере радиуса rad_cam
		//
		RECT wind_view = map.GetRect();
		ctrl.GetYLimit( (int*)&wind_view.top, (int*)&wind_view.bottom ); 
		pos.x = ( wind_view.left-100 + wind_view.right ) / 2;
		pos.y = 0;
		
		target.x = pos.x;
		
		pos.z = -rad_cam;
		
		
		if( ::GetAsyncKeyState( VK_UP ) & 0x8000f )
			angle_camY += 1.0f * timeDelta ;			
		
		if( ::GetAsyncKeyState( VK_DOWN ) & 0x8000f )
			angle_camY -= 1.0f * timeDelta ;			
		
		if( ::GetAsyncKeyState( VK_RIGHT ) & 0x8000f ) 
			angle_camX += 1.0f * timeDelta ;

		if( ::GetAsyncKeyState( VK_LEFT ) & 0x8000f )
			angle_camX -= 1.0f * timeDelta ;

		if( ::GetAsyncKeyState( 'Z' ) & 0x8000f )
			angle_camX = angle_camY = 0;
		
		if( ::GetAsyncKeyState( VK_ADD ) & 0x8000f )
			rad_cam -= 100.0f * timeDelta ;

		if( ::GetAsyncKeyState( VK_HOME ) & 0x8000f )
			map.SetDebageState( true );
		if( ::GetAsyncKeyState( VK_END ) & 0x8000f )
			map.SetDebageState( false );

	

		if( ::GetAsyncKeyState( VK_SUBTRACT ) & 0x8000f )
			rad_cam += 100.0f * timeDelta ;

		
		//
		// Клавиша перезапуска сцены
		//

		if( ::GetAsyncKeyState( 'R' ) & 0x8000f ){
			v0 = 0.0f;
			angle = 0.0f;
			chargeX = -300.0f;
			chargeY = 0.0f;
			chargeZ = 0.0f;
		}

		
		//
		//Обновление матрицы вида для точки наблюдателя
		//

		D3DXMATRIX view;		
		D3DXMatrixLookAtLH( &view, &pos, &target, &up );
		D3DXMATRIX rotV;
		D3DXMatrixRotationY( &rotV, angle_camX );
		D3DXMatrixMultiply( &view, &rotV, &view );

		D3DXMatrixRotationX( &rotV, angle_camY );
		D3DXMatrixMultiply( &view, &rotV, &view );
		pDevice->SetTransform( D3DTS_VIEW, &view );

		//
		// Визуализация
		//

		pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, d3d::WHITE, 1.0f, 0);
		pDevice->BeginScene();

		RECT rect = { 1, 1, width, height };
		
		map.RenderingScheme( pDevice, pLgcObj, pSphereMsh, pPipeMsh, pCell, &pCellTxtr, pCellMtrl, pServerMsh, &pServerTxtr, pServerMtrl );
		ctrl.Draw();
		
		//Включение смешивания
		pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		
					
		pDevice->EndScene();
		pDevice->Present( NULL, NULL, NULL, NULL );
		
		
	}
	return true;
}

//
// Функция обработки сообщений
//

LRESULT CALLBACK d3d::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	
	POINT pt;
	int hi, lo;
	int res;
	int temp;
	
	switch( msg )
	{
	
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE ){ 
			
			::ShowWindow( menu_elem[smh::MENUWINDOW]/*hwnd_menu*/, SW_SHOW );
			
		}

		break;
	case WM_MOUSEWHEEL:
		hi = HIWORD( wParam );
		hi = ( hi > 120 )? -hi: hi;
		rad_cam += Sign( hi ) * 5 ;
		
		break;
	case WM_LBUTTONDOWN:
		
		
		GetCursorPos( &pt );
		ScreenToClient( menu_elem[smh::MAINWINDOW]/*main_hwnd*/, &pt );
		res = map.PickOnTerminal( pDevice, &pt, pServerMsh );
		if(  res != NILL ) ctrl.Core( res, menu_elem );


		res = map.PickOnSource( pDevice, &pt, pSphereMsh );
		if( res != NILL ) ::MessageBox( 0, L"HIT ON Source", 0, 0 );

		break;
	case WM_COMMAND:
		
		if ( (HWND)lParam == menu_elem[smh::CONTINUE]/*button[butt::CONTINUE]*/ ){
						
						::ShowWindow( menu_elem[smh::MENUWINDOW]/*hwnd_menu*/, SW_HIDE );
						
					break; 	
		}
		if( ((HWND)lParam == menu_elem[smh::RULEBUTTON1]/*sysmenu[sm::RULEBUTTON1]*/)||((HWND)lParam == menu_elem[smh::RULEBUTTON2]/*sysmenu[sm::RULEBUTTON2]*/) ) {
			
			::MessageBox( menu_elem[smh::STARTMENU]/*start_menu*/, gui::GetRule(), L"Правила игры", MB_ICONINFORMATION| MB_ICONASTERISK );
			break;
		}

		ctrl.Init( &map, pDevice );
		if( ( level == NILL )||( ctrl.GetCurrentSession() == NON ) ){
			temp = ctrl.Training( lParam, elem, menu_elem );
			level = ( temp != NILL )? temp: NILL;
			temp = ctrl.Test( lParam, menu_elem );
			level = ( temp != NILL )? temp: level;
			temp = ctrl.Tutorial( lParam, menu_elem );
			level = ( temp != NILL )? temp: level;
		}
		
				
					
		if( ( (HWND)lParam == menu_elem[smh::EXIT1] )|| ( (HWND)lParam == menu_elem[smh::EXIT2] ) ){							
					
					::PostQuitMessage( 0 );
		}
		if ( (HWND)lParam == menu_elem[smh::GOSTARTMENU]/*button[butt::EXIT]*/ ){							
					::ShowWindow( menu_elem[smh::MENUWINDOW], SW_HIDE );
					::ShowWindow( menu_elem[smh::MAINWINDOW], SW_HIDE );
					::ShowWindow( menu_elem[smh::STARTMENU], SW_SHOW );
		}

		break;
		
		case WM_DESTROY:
			::PostQuitMessage( 0 );
		break;
	
	}
	return ::DefWindowProc( hwnd, msg, wParam, lParam );
}

//
// WinMain
//

int WINAPI WinMain( HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd )
{
	if( ( width == 0 ) || ( height == 0 ) ){
		width = GetSystemMetrics( SM_CXSCREEN );//Получить ширину экрана
		height = GetSystemMetrics( SM_CYSCREEN );//Получить высоту экрана
	}

	if( !gui::MainWindow( hinstance, width, height, true, &pDevice, /*&main_hwnd*/&menu_elem[smh::MAINWINDOW] )
		/*!d3d::InitD3D( hinstance,
		width, height, true, D3DDEVTYPE_HAL, &pDevice, button )*/ )
	{
		::MessageBox( 0, L"InitD3D() - FAILED", 0, 0 );
		return 0;
	}
	
	if( !Setup()  ){
		::MessageBox( 0, L"Setup() - FAILED", 0, 0 );
		return 0;
	}
	gui::CreateStartMenu( menu_elem, elem, width, height  );
	::ShowWindow( menu_elem[smh::STARTMENU]/*start_menu*/, SW_SHOW );
	::UpdateWindow( menu_elem[smh::STARTMENU]/*start_menu*/ );
	
	std::stack<int> screen;
	
	gui::CreateMenu( &menu_elem[smh::MENUWINDOW], menu_elem, width, height );
	::ShowWindow( menu_elem[smh::MENUWINDOW]/*hwnd_menu*/, SW_HIDE );
	
	gui::EnterMsgLoop( Display, &level );
	Cleanup();

	pDevice->Release();

	return 0;
}

