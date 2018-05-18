//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: map.cpp
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "map.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <queue>

//отступ после схемы, для расположения приёмников
#define PARAGRAPH 2

using namespace pipeconst;

MAP::MAP(){
	gen_cnt = 0;
}

MAP::~MAP(){
}

bool MAP::Init(){

	_debag = false;
	if( !PreRenderingScheme() ) return false;
	

	point first = STARTPOS;
	point last = FINISHPOS;

	float stepRow;
	float stepColumn;
	stepColumn = stepRow = STEP;
	_sourceCnt = _lgc.GetCountSocket() ; 
	_row = ( _sourceCnt - 1 ) * 2 + 2;
	_column = ( _lvlcnt + 1 ) * 2 ;

	_map = new point* [_row ];
	for( int i = 0; i < _row; i++ )
		_map[i] = new point [_column];
	
	for( int i = 0; i < _row; i++ )
		for( int j = 0; j < _column; j++ )
			_map[i][j] = point( first.x + j*stepColumn, first.y - i*stepRow ); 

	for( int i = 0; i < _quecnt; i++ ){
				int curr = _queue[i];
				_operPos[curr] = GetPos( _seat[curr], _level[i]*2+1 );
	}

	ConnectionReceiver();//подсоеденим приёмники

	_sourcePos = new point [ _sourceCnt * 2 ];
	_source_curr = 0;
	_n_source = _sourceCnt;
	for( int i = 0; i < _sourceCnt * 2; i++ )
		_sourcePos[i] = point( first.x - stepRow, first.y - i*stepRow ); 
	
	for( int i = 0; i < _sourceCnt; i++ ) source_value[i] = false;

	for( int i = 0; i < _row; i++ )
		for( int j = 0; j < _column; j ++ )
			_way[i][j] = WAY( NILL, NILL, NILL, NILL );
	ConnectionOfScheme();
	
	return true;
}

void MAP::RenderingScheme( IDirect3DDevice9* pDevice, ID3DXMesh* pLgcObj[], ID3DXMesh * pSphereMsh, ID3DXMesh * pPipe[], 
						  ID3DXMesh* pCell, std::vector<IDirect3DTexture9*>* pCellTxtr, std::vector<D3DMATERIAL9> pCellMtrl,
						  ID3DXMesh* pServMsh, std::vector<IDirect3DTexture9*>* pServTxtr, std::vector<D3DMATERIAL9> pServMtrl ){
	
	gui::PressSource( source_value, &_source_curr, _n_source );
	Calculate( );
	//отрисовка операторов
	for( int i = 0; i < _quecnt; i++ ){
				int curr = _queue[i];
				point p = GetOperPos( curr );
				D3DXMATRIX w, rot;
				D3DXMatrixTranslation( &w,
								p.x,
								p.y,
								0.0f );
				D3DXMatrixRotationX( &rot, -D3DX_PI / 6 );
				D3DXMatrixMultiply( &w, &rot, &w );
				pDevice->SetTransform( D3DTS_WORLD, &w );
				
				//Установа материала и прорисовка значка оператора
				D3DMATERIAL9 red = d3d::RED_MTRL;
				pDevice->SetMaterial( &red );					
				pLgcObj[_lgc.GetObj( curr )]->DrawSubset( 0 );
				
				//Установа материала и прорисовка логической ячейки - сферы
				pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
				D3DMATERIAL9 color = ( result[curr]._res & _debag )?d3d::GREEN_MTRL:d3d::YELLOW_MTRL;
				color.Diffuse.a = 0.4f;
				pDevice->SetMaterial( &color );
				pSphereMsh->DrawSubset( 0 );
				pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	}

	//отрисовка приёмников от операторов
	for( int i = 0; i < _n_receiver; i++ ){
		//матрица для горизонтального отваода от оператора
		D3DXMATRIX world;
		D3DXMatrixTranslation( &world,
							_receivers_way[i].x,
							_receivers_way[i].y,
							_receivers_way[i].z );				
		
		//Установа материала для дорожек приёмников
		D3DMATERIAL9 color = ( receiver_value[i] & _debag )?d3d::GREEN_MTRL:d3d::BLUE_MTRL;
		color.Diffuse.a = 0.4f;
		
		pDevice->SetMaterial( &color );
		
		//установка матрицы и отрисовка горизонтального отвода
		D3DXMATRIX w = world, rot;
		D3DXMatrixRotationX( &rot, D3DX_PI/2 );// верхний вокруг иксовой оси на 90 градусов | -> x
		D3DXMatrixMultiply( &w, &rot, &w );
		pDevice->SetTransform( D3DTS_WORLD, &w );
		pPipe[FATUP]->DrawSubset( 0 );
		D3DXMatrixRotationX( &rot, D3DX_PI );// так как нижнего пайпа нет, используем верхний с поворотом на 180, от уже повёрнутого на 90
		D3DXMatrixMultiply( &w, &rot, &w );
		pDevice->SetTransform( D3DTS_WORLD, &w );
		pPipe[FATUP]->DrawSubset( 0 );

		//матрица для вертикального отвода от оператора
		int y = _receivers_way[i].y;
		int len = _receivers_way[i].len*2;
		for( ; len > 0 ; len-- ){
			y -= STEP / 2;
			D3DXMatrixTranslation( &world,
							_receivers_way[i].x,
							y,
							_receivers_way[i].z + STEP/2 );	
			w = world;
			D3DXMatrixRotationX( &rot, 0 );// верхний 
			D3DXMatrixMultiply( &w, &rot, &w );
			pDevice->SetTransform( D3DTS_WORLD, &w );
			pPipe[FATUP]->DrawSubset( 0 );
			D3DXMatrixRotationX( &rot, D3DX_PI );// так как нижнего пайпа нет, используем верхний с поворотом на 180
			D3DXMatrixMultiply( &w, &rot, &w );
			pDevice->SetTransform( D3DTS_WORLD, &w );
			pPipe[FATUP]->DrawSubset( 0 );

			
		}
		//подготовка матрицы к прорисовке терминала
		pos_receiver[i] = D3DXVECTOR3( _receivers_way[i].x,
								y,
							_receivers_way[i].z + STEP/2 );
		D3DXMatrixTranslation( &world,
							pos_receiver[i].x,
							pos_receiver[i].y,
							pos_receiver[i].z );	
		w = world;	
		D3DXMatrixRotationX( &rot, -D3DX_PI / 6 );
		D3DXMatrixMultiply( &w, &rot, &w );
		D3DXMatrixRotationY( &rot, D3DX_PI / 2 );
		D3DXMatrixMultiply( &w, &rot, &w );
		pDevice->SetTransform( D3DTS_WORLD, &w );
		
		
		
		for( int i = 0; i < pServMtrl.size(); i++ ){
			pDevice->SetMaterial( &pServMtrl[i]);	
			pDevice->SetTexture( 0, pServTxtr->at(i));
		
			pServMsh->DrawSubset( i );
		}
		
		pDevice->SetTexture( 0, NULL );
	}

	//отрисовка источников
	for( int i = 0; i < _sourceCnt * 2; i++ ){
				point p = _sourcePos[i++];
				D3DXMATRIX w, rot;
				D3DXMatrixTranslation( &w,
								p.x,
								p.y,
								0.0f );
		
				
				
				
				
				//*****
				if( i-1 == _source_curr*2 ){
						
					D3DXMATRIX world = w, rot;	
					D3DXMatrixRotationX( &rot, -D3DX_PI / 5 );
					D3DXMatrixMultiply( &world, &rot, &world );
					D3DXMatrixRotationY( &rot, D3DX_PI / 2 );
					D3DXMatrixMultiply( &world, &rot, &world );
					pDevice->SetTransform( D3DTS_WORLD, &world );

					for( int i = 0; i < pCellMtrl.size(); i++ ){
						pDevice->SetMaterial( &pCellMtrl[i] );	
						pDevice->SetTexture( 0, pCellTxtr->at(i) );		
						pCell->DrawSubset( i );
					}
		
						pDevice->SetTexture( 0, NULL );
				}
				
//*****
				//Установа материала и прорисовка значка источник 0 или 1
				D3DXMatrixRotationX( &rot, -D3DX_PI / 6 );
				D3DXMatrixMultiply( &w, &rot, &w );
				pDevice->SetTransform( D3DTS_WORLD, &w );
				pDevice->SetTransform( D3DTS_WORLD, &w );
				D3DMATERIAL9 red = d3d::RED_MTRL;
				D3DMATERIAL9 green = d3d::GREEN_MTRL;
				if( source_value[(i-1)/2]  ){
					pDevice->SetMaterial( &green );
					pLgcObj[LOGICONE]->DrawSubset( 0 );
				}
				else{ 
					pDevice->SetMaterial( &red );
					pLgcObj[LOGICZERO]->DrawSubset( 0 );		
				}
					

				pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );

				
				green.Diffuse.a = 0.4f;
				 red = d3d::RED_MTRL;
				red.Diffuse.a = 0.4f;
				
				D3DMATERIAL9 yell = d3d::YELLOW_MTRL;
				yell.Diffuse.a = 0.4f;
				pDevice->SetMaterial( &yell );
				pSphereMsh->DrawSubset( 0 );
				pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	
	}

	//отрисовка путей
	for( int i = 0; i < ( _sourceCnt-1 )*2+1; i++ )
		for( int j = 0; j < _lvlcnt*2+1; j++ ){
				
				if( (( _way[i][j].in1 != NILL )||( _way[i][j].in2 != NILL ))/*&&(  _way[i][j].left == NILL )*/ ) continue;

				point p = GetPos( i, j );
				D3DXMATRIX world;
				D3DXMatrixTranslation( &world,
								p.x,
								p.y,
								0.0f );
		
				
		
				//Установа материала для дорожек схемы
				D3DMATERIAL9 blue = d3d::BLUE_MTRL;
				pDevice->SetMaterial( &blue );
				
				
				if( ( _way[i][j].up == _way[i][j].bottom )&&( _way[i][j].up != NILL )&&( _way[i][j].left != NILL )&&( _way[i][j].right != NILL ) ){
						D3DXMATRIX w = world, rot;
						D3DXMatrixRotationZ( &rot, 0 );
						D3DXMatrixMultiply( &w, &rot, &w );
						pDevice->SetTransform( D3DTS_WORLD, &w );
						pPipe[VERTDUGA]->DrawSubset( 0 );
						pPipe[HORDUGA]->DrawSubset( 0 );
				}
				else {
					if( _way[i][j].left > NILL ){
					D3DXMATRIX w = world, rot;
					D3DXMatrixRotationZ( &rot, D3DX_PI/2 );
					D3DXMatrixMultiply( &w, &rot, &w );
					pDevice->SetTransform( D3DTS_WORLD, &w );
					
					pPipe[UP]->DrawSubset( 0 );
					pDevice->SetMaterial( &blue );
				}
				if( _way[i][j].right > NILL ){
					D3DXMATRIX w = world, rot;
					D3DXMatrixRotationZ( &rot, -D3DX_PI/2 );
					D3DXMatrixMultiply( &w, &rot, &w );
					pDevice->SetTransform( D3DTS_WORLD, &w );
					pPipe[UP]->DrawSubset( 0 );
				}

				if( _way[i][j].up > NILL ){
						D3DXMATRIX w = world, rot;
						D3DXMatrixRotationZ( &rot, 0 );
						D3DXMatrixMultiply( &w, &rot, &w );
						pDevice->SetTransform( D3DTS_WORLD, &w );
						pPipe[UP]->DrawSubset( 0 );
					}
				if( _way[i][j].bottom > NILL ){
						D3DXMATRIX w = world, rot;
						D3DXMatrixRotationZ( &rot, - D3DX_PI );
						D3DXMatrixMultiply( &w, &rot, &w );
						pDevice->SetTransform( D3DTS_WORLD, &w );
						pPipe[UP]->DrawSubset( 0 );	
					}
				}

				//разветвление на 3, вставить шарнир
				if( (( _way[i][j].left > NILL )&&( _way[i][j].right > NILL )) &&
					(( _way[i][j].up > NILL )||( _way[i][j].bottom > NILL ))  )
						if( !(( _way[i][j].up != NILL )&&( _way[i][j].bottom != NILL )) ){
							pDevice->SetTransform( D3DTS_WORLD, &world );
							pPipe[BALL]->DrawSubset( 0 );
					}
				
	}

	//отрисовка подключений in1/in2
	for( int i = 0; i < ( _sourceCnt-1 )*2+1; i++ )
		for( int j = 0; j < _lvlcnt*2+1; j++ ){
			point p = GetPos( i, j );
			D3DXMATRIX world;
			D3DXMatrixTranslation( &world,
								p.x,
								p.y,
								0.0f );
		
				
			pDevice->SetTransform( D3DTS_WORLD, &world );
				//Установа материала для дорожек схемы
			D3DMATERIAL9 blue = d3d::BLUE_MTRL;
			pDevice->SetMaterial( &blue );

			if( (  _way[i][j].in1 > NILL )&&(  _way[i][j].in2 > NILL ) ){

				if( ( i > 0 )&&( _way[i-1][j].bottom == _way[i][j].in1 ) )
					pPipe[UPRIGHT]->DrawSubset( 0 );
				if( ( i < ( _sourceCnt-1 )*2 )&&( _way[i+1][j].up == _way[i][j].in1 ) )
					pPipe[DOWNRIGHT]->DrawSubset( 0 );
				
				if( ( _way[i][j].left == _way[i][j].in1 ) )
					if( _way[i][j].up == _way[i][j].in2 ) pPipe[LEFTRIGHT_DOWN]->DrawSubset( 0 );
					else pPipe[LEFTRIGHT_UP]->DrawSubset( 0 );
				
				
				if( ( i > 0 )&&( _way[i-1][j].bottom == _way[i][j].in2 ) )
					pPipe[UPRIGHT]->DrawSubset( 0 );
				if( ( i < ( _sourceCnt-1 )*2 )&&( _way[i+1][j].up == _way[i][j].in2 ) )
					pPipe[DOWNRIGHT]->DrawSubset( 0 );
				
				if( ( _way[i][j].left == _way[i][j].in2 ) )
					if( _way[i][j].up == _way[i][j].in1 ) pPipe[LEFTRIGHT_DOWN]->DrawSubset( 0 );
					else pPipe[LEFTRIGHT_UP]->DrawSubset( 0 );
			}
			

		}		
	
}

point MAP::GetPos( int i, int j ){
	return _map[i][j];
}

void MAP::GenerateScheme( LEVEL lvl, int key ){
	char file[] = "pipe.x";
	_lgc.SetLvl( (LEVEL)lvl );
	std::ifstream in( file );
	int index = 0;
	if ( gen_cnt == 0 ) {
		while ( in>> gen[gen_cnt].level >> gen[gen_cnt].key ) gen_cnt++;
		in.close();
		if( !in.fail() ) index = -1;

	}
	if ( gen_cnt > 0 ) {
			int i = 0;
			while( ( gen[i].level != lvl )&&( i < gen_cnt ) ) i++;
			index = i;

	}
	int new_key;
	if( index >= 0 ){
		new_key = gen[index].key;
		temp = gen[index];
		for( int j = index; j < gen_cnt-1; j++ )
			gen[j] = gen[j+1];
		gen[gen_cnt-1] = temp;
		std::ofstream out( file );
		for( int j = 0; j < gen_cnt; j++ )
			out<< gen[j].level << ' ' << gen[j].key << std::endl;
		out.close();
	}
	else new_key = key;
	
	if( new_key != NULL )_lgc.Init( new_key );
	else _lgc.Init( NILL );
	_lgc.PrintResult();
}


void SwapLobj( Lobj *a, Lobj *b ){
	Lobj tmp = *a;
	*a = *b;
	*b = tmp;
}

int MAP::GetLvlObj( int a ){
	for( int i = 0; i < _quecnt; i++ )
		if ( _queue[i] == a ) return _level[i];
	return NILL;
}


bool MAP::PreRenderingScheme(){
	//выстраивание схемы для наглядности
	//
	int cnt = _lgc.GetCount();
	int min = 0;
	
	int k = 0;
	//
	
	
	int queue[MAXCOUNT] = { 0 };
	std::queue<int> que;
	while(  ( k < cnt ) ) {
		if (( _lgc._pLgcObj[k]._in1 < cnt )&&( _lgc._pLgcObj[k]._in2 < cnt )) que.push( k );
		k++;
	}
	
	int out;
	k = 0;// текущий номер queue
	//выстраивание схемы начинается от того, кто уже в игре и смотрится кто может следубщий появится, пока это только источники
	int allowedParents[MAXCOUNT*2]  = { 0 };
	int ap = _lgc.GetCountSocket();
	for( int i = 0; i < ap; i++ ) allowedParents[i] = i;

	while( k < cnt ){
		out = _lgc._pLgcObj[ que.front() ]._out;
		allowedParents[ap++] = out; // к cписку разрешённых добовляется выход оператора
		queue[k++] = que.front();		
		que.pop();
		for( int i = 0; i < cnt; i++ )
			if ( ( _lgc._pLgcObj[i]._in1 == out )||( _lgc._pLgcObj[i]._in2 == out ) ) {
				bool f1 = false, f2 = false;
				for( int allow = 0; allow < ap; allow++ ){
					if( _lgc._pLgcObj[i]._in1 == allowedParents[allow] ) f1 = true;
					if( _lgc._pLgcObj[i]._in2 == allowedParents[allow] ) f2 = true;
				}
				if( f1 && f2 ) que.push(i);
			}
	}
	memcpy( _queue, queue, sizeof( _queue ) );
	_quecnt = cnt;

	//уровни по кол-ву объектов в вертикальной линии
	int level[MAXCOUNT] = { 0, 0 };
	for( int i = 1; i < cnt; i++ ){
		int k = i-1;
		while( ( k >= 0 ) && (( _lgc._pLgcObj[ queue[i] ]._in1 != queue[k] + SHFOUT ) &&
			( _lgc._pLgcObj[ queue[i] ]._in2 != queue[k] + SHFOUT )) ) k--;
		if( k >= 0 ) level[i] = level[k] + 1;
		else{
			float average =  ( _lgc.GetCountSocket() - 1 )*2/2;
			if( ( ( max( _lgc.GetIn1( queue[i] ), _lgc.GetIn2( queue[i] ) ) >= average )&&
				  ( max( _lgc.GetIn1( queue[i-1] ), _lgc.GetIn2( queue[i-1] ) ) >= average ) ) ||
				( ( min( _lgc.GetIn1( queue[i] ), _lgc.GetIn2( queue[i] ) ) <= average )&&
				  ( min( _lgc.GetIn1( queue[i-1] ), _lgc.GetIn2( queue[i-1] ) ) <= average ) )
				) level[i] = level[i-1] + 1;
				
		}
		
	}
	memcpy( _level, level, sizeof( _level ) );	
	_lvlcnt = level[ cnt - 1 ];

	//место по вертикальной оси
	float avg = ( _lgc.GetCountSocket() - 1 )*2/2;//
	int seat[MAXCOUNT] = { NILL, NILL };
	for( int i = 0; i < cnt; i++ ){
		int curr = queue[i];
		int pred_in1 = NILL, pred_in2 = NILL;
		int in1 = _lgc.GetIn1( curr );
		int in2 = _lgc.GetIn2( curr );
		
		in1 = ( in1 >= SHFOUT )? seat[ (pred_in1 = in1) - SHFOUT ]: (pred_in1 = in1) * 2;//
		in2 = ( in2 >= SHFOUT )? seat[ (pred_in2 = in2) - SHFOUT ]: (pred_in2 = in2) * 2;//
		int max = max( in1, in2 );
		int min = min( in1, in2 );
		int dy = 0;
		if( abs(min - avg) < abs(max - avg ) ) { seat[curr] = max; dy = -1; }
		else { seat[curr] = min; dy = 1; }
		int source = seat[curr]/2;
		
			if( /*( curr != queue[j] )|| ( j + 1 == cnt )*/true ){//для последнеего можно пройтись, ему ничего не мешает, но чтоб избежать "линейного" высторения
				int out1 = NILL, out2 = NILL;//12.06 было изменено с 0 на НИЛЛ
				if( ( pred_in1 == NILL )&&( pred_in2 == NILL ) ) out1 = source;//по сути нужна  хотя бы одна точка отсчёта, но в идеале две. 0% что эта ветка хоть раз сработает
				else {
					/* метод GetOut( num ) возвращает выход оператора num, однако в нашем случае может использоваться не оператор, а источник, а метод не в курсе! 
					поэтому если аргумент меньше SHFOUT то оставить как есть..... Вообще бред, out1/out2 трогать вообще не нужно, пусть остаётся как есть
					но не хочется менять код, поэтому проверка, которая при ЛЮБЫХ out1/out2 юудет давать один резалт*/
					out1 /*= ( out1 < SHFOUT )? out1: _lgc.GetOut( pred_in1 )*/;
					out2 /*= ( out2 < SHFOUT )? out2: _lgc.GetOut( pred_in2 )*/;
				}
				//***** РЕШЕНИЕ НЕ НАЙДЕНО, перегенерация по коду фальш( 1 уровень ключ  1450281376 )
					//бывает что от одного выхода идёт на два оператора, так вот ото выход должен встать между детками
					
					int num1 = NILL, num2 = NILL;
					if( ( i < cnt-2 )&&( GetLvlObj( queue[i+1] ) == GetLvlObj( queue[i+2] ) ) ){ 
						num1 = queue[i+1]; num2 = queue[i+2]; 
						int limit1 = NILL, limit2 = NILL;
						if( (( _lgc.GetOut( curr ) == _lgc.GetIn1( num1 ) )||( _lgc.GetOut( curr ) == _lgc.GetIn2( num1 ) ))&&
							(( _lgc.GetOut( curr ) == _lgc.GetIn1( num2 ) )||( _lgc.GetOut( curr ) == _lgc.GetIn2( num2 ) )) ){ 
							limit1 = ( _lgc.GetOut( curr ) == _lgc.GetIn1( num1 ) )?_lgc.GetIn2( num1 ):_lgc.GetIn1( num1 );
							limit2 = ( _lgc.GetOut( curr ) == _lgc.GetIn1( num2 ) )?_lgc.GetIn2( num2 ):_lgc.GetIn1( num2 );
							limit1 = ( limit1 < SHFOUT )? limit1*2: seat[limit1 - SHFOUT];
							limit2 = ( limit2 < SHFOUT )? limit2*2: seat[limit2 - SHFOUT];
							while( ( seat[curr] <= min( limit1, limit2 ) )||( seat[curr] >= max( limit1, limit2 ) ) )
								if( ( dy < 0 )&&( seat[curr] > 0  ) )  seat[curr]+= dy;
								else
									if( ( dy > 0 )&&( seat[curr] < ( _lgc.GetCountSocket() - 1 )*2 ) )  seat[curr]+= dy;
									else break;
							
							
						}
						
				}
					//*****
				{
					
					//проверка занявшего места, не перекрывает ли путь у другм, иначе сдвинуть ещё
					bool f = false;
					for( int k = i+1; k < cnt; k++ ){  
							if ( ( _lgc.GetIn1( queue[k] )*2 == seat[curr] )||( _lgc.GetIn2( queue[k] )*2 == seat[curr] ) ){ 
								f = false; 
								//были внесены изменения в бывший pred!!! теперь появился pred1 и pred2, они не тестены и может нужен тут только один
								pred_in1 = NILL; 
								pred_in2 = NILL; 
								source = seat[curr]/2; 
								seat[curr] += dy;
								k = i+1;
								if( seat[curr] > (_lgc.GetCountSocket() - 1)*2 ){ 
									dy *= -1;
									seat[curr] += dy;
									
								}
								if( seat[curr] < 0 ) ::MessageBox( 0, L" seat[curr] up limit in PreRenderingSceme()" , 0, 0 );
							}
						}
					if( f ) {
						//j = i-1;
						seat[curr] += dy;
						//break;
					}
									

				}
				 { 
						bool f = true;
						if( GetLvlObj( curr ) == 0 ) f = false;

						//бывает что операторы не сдвигаются и выстраиваются в линию. Придадим ступенчатость
						int pred1 = _lgc.GetIn1( curr ) - SHFOUT;
						
						int pred2 = _lgc.GetIn2( curr ) - SHFOUT;
						
						bool b1 = abs( GetLvlObj( pred1 ) - GetLvlObj(curr) ) == 1;
						bool b2 = abs( GetLvlObj( pred2 ) - GetLvlObj(curr) ) == 1;
						bool b3 = seat[ pred1 ] == seat[curr];
						bool b4 = seat[ pred2 ] == seat[curr];
						if( ( b1 && b3 ) ||
							( b2 && b4 ) ){

							int new_seat = seat[curr] + dy;
							while( new_seat <= ( _lgc.GetCountSocket() - 1 )*2 )	{
								int k = i+1;
								for( ; k < cnt; k++ )
									if(  (( _lgc.GetIn1( queue[k] )*2 == new_seat   )||
										( _lgc.GetIn2( queue[k] )*2 == new_seat )) ){ //dy приплюсовалось в прошлов сравнении
											new_seat += dy;
											break; 
									}
									if( k < cnt ) ;
									else break;
							}
							//при оступенивании одноуровневых деток, бывает один накладывается на другого....запрет в таком случае
							for( int e = 0; e < cnt; e++ )
								if( i != e )
									if( ( seat[_queue[e]] == new_seat ) && ( _level[e] == _level[i] ) ) f = false;
							if( f  ) seat[curr] = new_seat;
								//break;
						}
						
					}
				
					//*****
				
			}
			
	}
	memcpy( _seat, seat, sizeof( _seat ) );
	_seatcnt = cnt;


	// корректировка level. уместь в один проход две трубы - плохая идея. поэтому все i++ двигаются на 1 вправо, освобождая тем самым место 
	//под вторую трубу для in оператора
	for( int i = 1; i < cnt; i++ ){
		int curr = _queue[i];
		int in1 = _lgc.GetIn1( curr );
		int in2 = _lgc.GetIn2( curr );
		in1 = ( in1 < SHFOUT)? in1*2: _seat[ in1 -SHFOUT ];
		in2 = ( in2 < SHFOUT)? in2*2: _seat[ in2 -SHFOUT ];
		if( (( in1 - _seat[curr] > 0 ) && ( in2 - _seat[curr] > 0 )) ||
			(( in1 - _seat[curr] < 0 ) && ( in2 - _seat[curr] < 0 )) ){
				for( int j = i; j < cnt; j++ )
					_level[j]++;
				_lvlcnt ++;
				
		}
	}
		//случай, где с одного оператора идут два потомка и родитель никак не расположить между ними, он где-то скраю и происходит перекрытие
		//нужно их подальше отодвинуть от родителя горизонтально сменить уровень второму детке
		for( int i = 1; i < cnt-1; i++ ){
			int curr = _queue[i];
			if( ( _seat[curr] - _seat[curr+1] < 2 )&&( _level[i] == _level[i+1] ) ){
				for( int j = i+1; j < cnt; j++ )
					_level[j]++;
				_lvlcnt ++;
			}

	}

	
	return true;
}


void MAP::ConnectionOfScheme(){
#define IN1 ( in1 >= SHFOUT )? _seat[in1 - SHFOUT]: in1// cnt-> SHFOUT
#define IN2 ( in2 >= SHFOUT )? _seat[in2 - SHFOUT]: in2//cnt-> SHFOUT
#define R1 ( remark1 == NILL )? IN1:_seat[curr]
#define R2 ( remark2 == NILL )? IN2:_seat[curr]

	int cnt = _quecnt;
	for( int i = 0; i < cnt; i++ ){
		int curr = _queue[i];
		int in1 = _lgc.GetIn1( curr );
		int in2 = _lgc.GetIn2( curr );
		in1 = ( in1 >= SHFOUT )? in1: in1 * 2;//cnt-> SHFOUT
		in2 = ( in2 >= SHFOUT )? in2: in2 * 2;//cnt-> SHFOUT
		int remark1 = NILL, remark2 = NILL, remark3 = 0;
		//для перекрытия одним объектом другого в линии и от обоих выходит труба к ребёнку. развод труб - горизонтальное слияние труб
		if( ( (in1 >= SHFOUT) && (in2 >= SHFOUT) )&&( _seat[in1 - SHFOUT] == _seat[in2 - SHFOUT] ) ){
			remark2 = ( GetLvlObj( in1 - SHFOUT ) > GetLvlObj( in2 - SHFOUT ) )? 0: NILL;
			remark1 = ( GetLvlObj( in1 - SHFOUT ) < GetLvlObj( in2 - SHFOUT ) )? 0: NILL;
		}else
			//решение вертикальнго слияния труб
			if( (i > 0) && (GetLvlObj(_queue[i]) - GetLvlObj(_queue[i-1]) > 1)){
				remark1 = ( GetLvlObj( in1 - SHFOUT ) > GetLvlObj( in2 - SHFOUT ) )? 0: NILL;
				remark2 = ( GetLvlObj( in1 - SHFOUT ) < GetLvlObj( in2 - SHFOUT ) )? 0: NILL;
			}
			else if( ( max( in1, in2 ) > SHFOUT ) && (GetLvlObj( curr ) - GetLvlObj( max( in1, in2 ) - SHFOUT ) > 1) && ( in1*in2 < SHFOUT*SHFOUT ) ){
				remark3 = NILL;//флаг для некоторого случая, где происходит вертикальное слияние, но нельзя применить случай, что выше, потому что будет опять вертикальное перекрытие
				//по этому флагу сделать недоход на одну клетку
			}
		// in1----------------------------------------------------------
		//идём вправо для in1
		int start = ( in1 <	SHFOUT )? 0: GetLvlObj( in1 - SHFOUT )*2+2;//1 смещение карты вправо + 1( не на самом ебъекте, а cправа от него, т.е от выхода объекта.)
		int column = 0;
		int coef = ( in1 < SHFOUT )? 0: remark3;
		for( int j = start ; j < GetLvlObj( curr )*2 + 1 + coef; j++ ){
				
				if(( remark1 != NILL )&&( j != start ))_way[R1][j].left = in1;
				else if( remark1 == NILL ) _way[R1][j].left = in1;
				if( j > start ) _way[R1][j-1].right = in1;
				if(( remark1 != NILL )&&( j == start )) _way[IN1][j].left = in1;
				column = ( remark1 == NILL)? j: start;
			}
		
		//идём вверх/вниз  для in1
		start = ( in1 <	SHFOUT )? in1: _seat[ in1 - SHFOUT ];
		int row = start;
		if( start < _seat[curr] )
			for( int k = start; k < _seat[curr]; k ++ ){
				_way[k][column].bottom = in1;
				_way[k+1][column].up = in1;
				row = k+1;
			}
		else
			for( int k = start; k > _seat[curr]; k -- ){
				_way[k][column].up = in1;
				_way[k-1][column].bottom = in1;
				row = k-1;
			}

		//цепляемся к объекту
		
		if( coef != 0 ){
			_way[row][column].right = in1;
			column++;
			_way[row][column].left = in1;
		}
		_way[row][(remark1 != NILL)? GetLvlObj(curr)*2 : column].in1 = in1;

		// in2--------------------------------------------------
		//идём вправо для in2
		start = ( in2 <	SHFOUT )? 0: GetLvlObj( in2 - SHFOUT )*2+2;//1 смещение карты вправо + 1( не на самом ебъекте, а мправа от него, т.е от выхода объекта.)
		column;
		coef  = ( in2 < SHFOUT )? 0: remark3;
		for( int j = start ; j < GetLvlObj( curr )*2 + 1 + coef; j++ ){
				
				if(( remark2 != NILL )&&( j != start ))_way[R2][j].left = in2;
				else if( remark2 == NILL ) _way[R2][j].left = in2;
				if( j > start ) _way[R2][j-1].right = in2;
				if(( remark2 != NILL )&&( j == start )) _way[IN2][j].left = in2;
				column = ( remark2 == NILL)? j: start;
			}
		//идём вверх/вниз  для in2
		start = ( in2 <	SHFOUT )? in2: _seat[ in2 - SHFOUT ];
		row = start;
		if( start < _seat[curr] )
			for( int k = start; k < _seat[curr]; k ++ ){
				_way[k][column].bottom = in2;
				_way[k+1][column].up = in2;
				row = k+1;
			}
		else
			for( int k = start; k > _seat[curr]; k -- ){
				_way[k][column].up = in2;
				_way[k-1][column].bottom = in2;
				row = k-1;
			}

		//цепляемся к объекту
		// не забываем пронедохрд на 1 при  remark3
		if( coef != 0 ){
			_way[row][column].right = in2;
			column++;
			_way[row][column].left = in2;
		}
		_way[row][(remark2 != NILL)? GetLvlObj(curr)*2 : column].in2 = in2;
	}
}

void MAP::ConnectionReceiver(){

	_n_receiver = _lgc.GetCountSocket();
	for( int i = 0; i < _n_receiver; i++ ){
		int curr = _lgc._pReceiver[i];
		point pos = GetPos( _seat[curr], GetLvlObj( curr )*2+1 );
		_receivers_way[i].x = pos.x;
		_receivers_way[i].y = pos.y;
		_receivers_way[i].z = STEP;
		_receivers_way[i].len = ( _n_receiver - 1 )*2 - _seat[curr] + PARAGRAPH;
		
	}
	
}

void MAP::Release(){
	_lgc.Releas();
	for( int i = 0; i < _row; i++ )
		delete []_map[i];
	delete []_map;
	delete []_sourcePos;
	
}

int MAP::GetLink1( int curr ){
	return result[curr]._link1;
}

int MAP::GetLink2( int curr ){
	return result[curr]._link2;
}

int MAP::GetRes( int curr ){
	int temp = ( curr < SHFOUT )? curr: curr - SHFOUT;
	curr = temp;
	return result[curr]._res;
}

void MAP::SetRes( int curr, bool value ){

	result[curr]._res = value;
}


void MAP::Calculate( ){
	//вычисления значения на операторах
	for( int i = 0; i < _quecnt; i++ ){
		int curr = _queue[i];
		
		result[curr]._link1 = min( _lgc.GetIn1(curr), _lgc.GetIn2(curr) );
		result[curr]._link2 = max( _lgc.GetIn1(curr), _lgc.GetIn2(curr) );
		int res = 0;
		int a = result[curr]._link1;
		a = ( a < SHFOUT )? source_value[a]: GetRes( a );
		int b = result[curr]._link2;
		b = ( b < SHFOUT )? source_value[b]: GetRes( b );

		switch ( _lgc.GetObj(curr) ){

			case LOGICOPERATOR::AND: 		
				SetRes( curr, a*b );
				break;
			case LOGICOPERATOR::OR: 
				SetRes( curr, a+b );
				break;
			case LOGICOPERATOR::IMPLICATION: 
				SetRes( curr, ( a <= b )? true: false );
				break;
			case LOGICOPERATOR::XOR: 
				SetRes( curr, ( a == b)? false: true );
				break;
		default:
			break;
		}
	}
	//вычисления значения на приёмнике
	for( int i = 0; i < _n_receiver; i++ ){
		int curr = _lgc._pReceiver[i];
		receiver_value[i] = GetRes( curr );
	}
	

}

D3DXVECTOR3 * MAP::GetReceiverPos(){
	return pos_receiver;
}

int MAP::PickOnTerminal( IDirect3DDevice9* pDevice, POINT* pt, ID3DXMesh* pServMsh ){
	
	D3DXMATRIX world, rot/*, view, proj*/;
	
	
	BOOL bHit;
	for( int i = 0; i < _n_receiver; i++ )  { 
		D3DXMatrixTranslation( &world,
						pos_receiver[i].x,
						pos_receiver[i].y,
						pos_receiver[i].z );	
			
		D3DXMatrixRotationX( &rot, -D3DX_PI / 6 );
		D3DXMatrixMultiply( &world, &rot, &world );
		D3DXMatrixRotationY( &rot, D3DX_PI / 2 );
		D3DXMatrixMultiply( &world, &rot, &world );

		
		bHit = d3d::HitInObject( pDevice, pt, pServMsh, &world );

		if( bHit ) 
			return i;
	}

	return NILL;
}

int MAP::PickOnSource( IDirect3DDevice9* pDevice, POINT* pt, ID3DXMesh* pServMsh ){
	
	D3DXMATRIX world, rot/*, view, proj*/;

	BOOL bHit;
	
	for( int i = 0; i < _n_source; i++ )  { 
		D3DXMatrixTranslation( &world,
						_sourcePos[i*2].x,
						_sourcePos[i*2].y,
						0.0f );	
			
		D3DXMatrixRotationX( &rot, -D3DX_PI / 6 );
		D3DXMatrixMultiply( &world, &rot, &world );
		D3DXMatrixRotationY( &rot, D3DX_PI / 2 );
		D3DXMatrixMultiply( &world, &rot, &world );
		
		bHit = d3d::HitInObject( pDevice, pt, pServMsh, &world );
		if( bHit ) return i;
	}
	return NILL;
}

int MAP::GetValueReceiver( int index ){
	return receiver_value[index];
}

int MAP::GetReceiversPort( int index ){
	return _lgc._pReceiver[index];
}
int MAP::GetCountSocket(){
	return _n_receiver;
}


void MAP::SetDebageState( bool state ){
	_debag = state;
}

void MAP::GetIn( int curr, int* in1, int* in2 ){
	curr = ( curr < SHFOUT )? curr: curr - SHFOUT;
	*in1 = _lgc.GetIn1( curr );
	*in2 = _lgc.GetIn2( curr );
}



int MAP::GetCountObject(){
	return _lgc.GetCount();
}

point MAP::GetOperPos( int curr ){
	return _operPos[curr];
}

point MAP::GetSourcePos( int curr ){
	
	return _sourcePos[curr*2];
}

RECT MAP::GetRect(){
	RECT a;
	a.left = _map[0][0].x;
	a.right = _map[0][(_lvlcnt-1)*2+1].x;
	a.top = _map[0][0].y;
	a.bottom = _map[_seatcnt-1][0].y;
	return a;
}