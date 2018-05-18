//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: map.h
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __mapH__
#define __mapH__
#define POINTER_64 __ptr64
#include "generator.h"
#include "interface.h"
#include "d3dUtility.h"


namespace pipeconst{
	enum PIPE{
		UP,
		HORDUGA,
		VERTDUGA,
		UPRIGHT,
		DOWNRIGHT,
		LEFTRIGHT_UP,
		LEFTRIGHT_DOWN,
		BALL,
		FATUP,
		COUNT
	};
}

//количество стольбцов на карте
//#define COLUMN 7*2

//количество строк на карте
//#define ROW 5



struct point
{
	point( float a, float b ){
		x = a;
		y = b;
	}
	point(){
	};
	float x, y;
};

//позици€ €чейки (0,0)
#define STARTPOS point( -800.0f, 400.0f )
// Ўаг сетки
#define STEP 100.0f

//позици€ €чейки (4,4)
#define FINISHPOS point( 2000.0f, -300.0f )

class MAP{
public:
	MAP();
	~MAP();
	bool Init();
	point GetPos( int, int );
	void GenerateScheme( LEVEL lvl, int key );
	bool PreRenderingScheme();
	void RenderingScheme( IDirect3DDevice9*, ID3DXMesh* [], ID3DXMesh*, ID3DXMesh* [], ID3DXMesh*, 
		std::vector<IDirect3DTexture9*>* pCellTxtr, std::vector<D3DMATERIAL9> pCellMtrl,
		ID3DXMesh* , std::vector<IDirect3DTexture9*>*, std::vector<D3DMATERIAL9> );
	void ConnectionOfScheme();
	void ConnectionReceiver();
	int GetLink1( int );
	int GetLink2( int );
	int GetRes( int );
	void SetRes( int, bool );
	void Calculate( );
	int GetLvlObj( int );
	void Release();
	D3DXVECTOR3 * GetReceiverPos();
	point  GetSourcePos( int );
	int PickOnTerminal( IDirect3DDevice9 *, POINT *, ID3DXMesh* );
	int PickOnSource( IDirect3DDevice9 *, POINT *, ID3DXMesh* );
	int GetValueReceiver( int );
	int GetReceiversPort( int );

	int GetCountSocket();
	void SetDebageState( bool );
	void GetIn( int, int*, int* );
	/*int QueueAt( int );*/
	int GetCountObject();
	point GetOperPos( int );
	RECT GetRect();

private:
	bool _debag;
	point **_map;
	int _row, _column;

	
	struct WAY{
		WAY::WAY(){};
		WAY::WAY( int l, int r, int u, int b ){
			left = l;
			right = r;
			up = u;
			bottom = b;
			in1 = NILL;
			in2 = NILL;
		}
	
		WAY::~WAY(){};
		int left, right, bottom, up, in1, in2;
	}  _way[10][10*3];

	struct {
		float x, y, z, len;
	} _receivers_way[5];
	D3DXVECTOR3 pos_receiver[5];
	int _n_receiver;
	bool receiver_value[5];

	bool source_value[5];
	int _source_curr, _n_source;
	struct TRUTHTABLE{
		int _num_curr;
		int _link1, _link2, _res;//link1 - верхний, link2 - нижний
	} result[MAXCOUNT];

	point * _sourcePos;
	int _sourceCnt;
	Lobj _lgc;
	int _level[MAXCOUNT];
	int _lvlcnt;
	int _queue[MAXCOUNT];
	int _quecnt;
	int _seat[MAXCOUNT];
	int _seatcnt;

	point _operPos[MAXCOUNT];

	struct {
		int level, key;
	} gen[1000], temp;
	int gen_cnt;
};



#endif // __mapH__