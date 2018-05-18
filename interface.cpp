

#include "interface.h"



VOID WaitUnpressKey( char a ){
	do{
	}
	while ( ::GetAsyncKeyState( a ) & 0x8000f );
}

bool gui::MainWindow( HINSTANCE hInstance, int width, int height, bool windowed, IDirect3DDevice9** device, HWND *hwnd ){
//
		
		WNDCLASS wc;

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = (WNDPROC)d3d::WndProc; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIconW( 0, IDI_APPLICATION );
		wc.hCursor       = LoadCursor( 0, IDC_ARROW );
		wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
		wc.lpszMenuName  = 0;
		wc.lpszClassName = L"Direct3D9App";

		if( !RegisterClass( &wc ) ) {
			::MessageBox( 0, L"RegisterClass() - FAILED", 0, 0 );
			return false;
		}
		
		
		*hwnd = ::CreateWindow( L"Direct3D9App", TITLE, 
			WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
			0, 0, width, height,
			0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/ ); 

		if( !*hwnd ){
			::MessageBox( 0, L"CreateWindow() - FAILED", 0, 0 );
			return false;
		}
		
		::ShowWindow( *hwnd, SW_HIDE );
		
		::UpdateWindow( *hwnd );
		
	
		return d3d::InitD3D( width, height, windowed, hwnd, D3DDEVTYPE_HAL, device ) ;
	}

void gui::CreateMenu( HWND *h,  HWND * sysmenu, int width, int height ){
		*h = CreateWindowEx( WS_EX_TOPMOST, L"Direct3D9App", L"Меню", 
			  WS_BORDER, 
			width / 2 - 113, height / 2 - 150, 227, 300,
			0 /*parent hwnd*/, 0 /* menu */, 0, 0 /*extra*/ );
		
		sysmenu[smh::CONTINUE] =	CreateWindow( L"button", L"ПРОДОЛЖИТЬ",				WS_VISIBLE|WS_CHILD , 10,	0,	200, 50,		*h, NULL, NULL ,NULL );
		sysmenu[smh::RULEBUTTON2]	=	CreateWindow( L"button", L"Правила игры",		WS_VISIBLE|WS_CHILD , 10,	51,	200, 50,		*h, NULL, NULL, NULL );
		sysmenu[smh::GOSTARTMENU]	=	CreateWindow( L"button", L"Выйти в начальное меню",		WS_VISIBLE|WS_CHILD , 10,	101,	200, 50,	*h, NULL, NULL, NULL );
		sysmenu[smh::EXIT1] =		CreateWindow( L"button", L"ВЫЙТИ НА РАБОЧИЙ СТОЛ",	WS_VISIBLE|WS_CHILD , 10,	151,	200, 100,	*h, NULL, NULL, NULL );
		
}

int gui::EnterMsgLoop( bool (*ptr_display)( float timeDelta ), int *level )
{
	MSG msg;
	::ZeroMemory( &msg, sizeof( MSG ) );
	
	static float lastTime = (float)timeGetTime(); 

	while( msg.message != WM_QUIT )
	{
		if( ::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
		else
        {	
			float currTime  = (float)timeGetTime();
			float timeDelta = ( currTime - lastTime ) * 0.001f;
			if( ((  *level == LEVEL::EASY )||( *level == LEVEL::MEDIUM )||( *level == LEVEL::HARD ))/* &&
				(( name != "" ) && ( name != "Ваше ФИО" ))*/ )//проверка в перехватчике сообщений
					ptr_display( timeDelta );
		
			
			lastTime = currTime;
        }
    }
    return msg.wParam;
}

void gui::PressSource( bool *pSource, int *curr, int n ){
	if( ::GetAsyncKeyState( 'W' ) & 0x8000f ){
			if( *curr > 0 ) (*curr)--;
			WaitUnpressKey( 'W' );
	}
	if( ::GetAsyncKeyState( 'S' ) & 0x8000f ){
			if( *curr < n-1 ) (*curr)++;	
			WaitUnpressKey( 'S' );
	}
	if( ::GetAsyncKeyState( VK_SPACE ) & 0x8000f ){
			pSource[*curr] = ( pSource[*curr] == true )? false: true;
			WaitUnpressKey( VK_SPACE );
	}

}

void gui::CreateStartMenu( HWND *sysmenu, HWND *sm, int width, int height ){
	/**h*/sysmenu[smh::STARTMENU] = CreateWindowEx( WS_EX_TOPMOST, L"Direct3D9App", L"Начальное меню", 
			  WS_BORDER, 
			width / 2 - 143, height / 2 - 215, 287, 430,
			0 /*parent hwnd*/, 0 /* menu */, 0, 0 /*extra*/ );
	
	sysmenu[smh::RULEBUTTON1]	=	CreateWindow( L"button", L"Правила игры", WS_VISIBLE|WS_CHILD ,	10,	0,	260, 50, sysmenu[smh::STARTMENU], NULL, NULL, NULL );
	
	
	sm[LEVEL::EASY]		=	CreateWindow( L"button", L"Тренировка: сложность лёгкая",	WS_VISIBLE|WS_CHILD ,	10,	51,	260, 50,	sysmenu[smh::STARTMENU], NULL, NULL ,NULL );
	sm[LEVEL::MEDIUM]	=	CreateWindow( L"button", L"Тренировка: сложность нормальная",	WS_VISIBLE|WS_CHILD ,	10,	101,	260, 50,	sysmenu[smh::STARTMENU], NULL, NULL, NULL );
	sm[LEVEL::HARD]	    =	CreateWindow( L"button", L"Тренировка: сложность тяжёлая",	WS_VISIBLE|WS_CHILD ,	10,	151,	260, 50,	sysmenu[smh::STARTMENU], NULL, NULL, NULL );

	
	sysmenu[smh::PUPILNAMEEDIT] =CreateWindow( L"edit", L"Ваше ФИО",	WS_VISIBLE|WS_CHILD|WS_BORDER|SS_CENTER ,	10,	201,	260, 20,	sysmenu[smh::STARTMENU], NULL, NULL, NULL );
	
	sysmenu[smh::TEST]	=	CreateWindow( L"button", L"Начать тест ЗУН", WS_VISIBLE|WS_CHILD ,			10,	221,	260, 50, sysmenu[smh::STARTMENU], NULL, NULL, NULL );
	sysmenu[smh::EXIT2] =	CreateWindow( L"button", L"ВЫЙТИ НА РАБОЧИЙ СТОЛ",	WS_VISIBLE|WS_CHILD ,	10,	271,	260, 100,	sysmenu[smh::STARTMENU], NULL, NULL, NULL );

}	

LPWSTR gui::GetRule(){
	return  L"\tНекоторое агентство по борьбе с киберпреступностью проводит набор сотрудников на должность главного хакера, в котором будет принимать участие Вася. Васе требуется пробиться через маршрутизаторы на заданный сервер и запустить скрипт по \"обезоруживанию\" его. Количество серверов будет известно только после начала операции. Маршрутизаторы помечены в сети как логические объекты, на вход которых подаются сигналы 0 или 1. Как только сигнал 1 дойдёт до сервера он подготовит путь скрипту. Затем Васе необходимо запустить скрипт на сервере! \n\tЕсли не пробиваться через маршрутизаторы и предпринять попытку запуска скрипта, то вражеский маршрутизатор начнёт защищаться и в случае ТРЁХ таких неудач, скроет всю сеть и задача будет провалена. Помогите Васе получить работу.\n\t Управление:\n * переход от одного источника сигнала к другому осуществляется с помощью клавиш W и S - вверх и вниз соответственно;\n * изменение сигнала выбранного источника переключается клавишей пробела;\n * запуск скрипта - ЛКМ по серверу;\n * выход в меню - escape.";

}