//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: control.h
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __controlH__
#define __controlH__

#include "d3dUtility.h"

#include "map.h"
#include "stdlib.h"


namespace life{
	enum {
			ONE,
			TWO,
			THREE,
			COUNT
	};

	
}

enum {
	LOCK,
	UNLOCK
};

enum{
	NON,
	TESTSESSION,
	TRAINSESSION,
	TUTORSESSION
};
//
//Максимальная оценка
//
#define MAXMARK 9

class CONTROL{
public:
	CONTROL();
	~CONTROL();
	void Init( MAP* map, IDirect3DDevice9 * pDevice );
	int Training( LPARAM lParam, HWND* elem, HWND *menu_elem );//return level! это ключ на начала прорисовки лисплея
	int Test( LPARAM lParam, HWND* menu_elem );
	int Verdict();
	int Tutorial( LPARAM lParam, HWND* menu_elem );
	void TutCore();
	void Release();
	void Core( int, HWND* );
	void Draw();
	int GetCurrentSession();
	float GetCoefOfLearn();
	void GetYLimit( int*, int* );

private:
	MAP* _map;
	int _life;
	IDirect3DDevice9 * _pDevice;
	ID3DXMesh* _pHeartMsh;
	ID3DXMesh* _pTickMsh;

	std::wstring _pupils_name;
	int _curr_level;
	int _unlock_server[5];
	int _n_socket;
	int _curr_ses;
	int _point;
	
	//for tutorial
	int _curr_server;
	ID3DXMesh* _pLimitedSphereMsh;
	ID3DXMesh* _pArrowMsh;
	int _cycle_anim;
	int _pred[MAXCOUNT];
	int _cnt_pred;
	ID3DXFont * _pFont;      // Шрифт Diect3D
	int _yu, _yd;//vertex limit
};

//	ж  |  	л   | 	o, %
//	3		9		100
//	2		9		85
//	1		9		70
//	0		9		60
//	0		8		54
//	0		7		48
//	0		6		42
//	0		5		36
//	0		4		30
//	0		3		24
//	0		2		18
//	0		1		12
//	0		0		6



#endif // __controlH__