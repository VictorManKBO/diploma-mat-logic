//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: interface.h
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __interfaceH__
#define __interfaceH__

#include "d3dUtility.h"
#include "generator.h"


#define TITLE  L"Логик"

namespace smh{
	enum STATE{
		EXIT1,
		EXIT2,
		CONTINUE,
		RULEBUTTON1,
		RULEBUTTON2,
		MAINWINDOW,
		MENUWINDOW,
		STARTMENU,
		PUPILNAMEEDIT,
		TEST,
		TUTORIAL,
		GOSTARTMENU,
		COUNT
	};
}

namespace gui{
	

	

	bool MainWindow( HINSTANCE hInstance, int width, int height, bool windowed, IDirect3DDevice9** device, HWND *hwnd );
	void CreateMenu( HWND *h,  HWND* sysmenu, int width, int height );
	int EnterMsgLoop( bool ( *ptr_display )( float timeDelta ), int *level );

	void PressSource( bool * source, int *curr, int n );
	void CreateStartMenu( HWND *sysmenu, HWND *SM, int width, int height );
	LPWSTR GetRule();
	
}


#endif // __interfaceH__