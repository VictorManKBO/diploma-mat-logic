

#include "control.h"

CONTROL::CONTROL(){
	_map = new MAP;
}
CONTROL::~CONTROL(){}

void CONTROL::Init( MAP* map, IDirect3DDevice9 * pDevice ){
	_map = map;
	_pDevice = pDevice;
	_life = 3;
	_curr_level = NULL;
	_curr_ses = NON;
	_point = 0;
	memset( _unlock_server, LOCK, sizeof( _unlock_server ) );
	d3d::LoadXfile( L"heart.x", _pDevice, &_pHeartMsh, NULL, NULL );
	d3d::LoadXfile( L"tick.x", _pDevice, &_pTickMsh, NULL, NULL );
	
	_curr_server = NILL;
	_cycle_anim = NULL;
	_cnt_pred = 0;
	d3d::LoadXfile( L"limitedsphere.x", pDevice, &_pLimitedSphereMsh , NULL, NULL );
	d3d::LoadXfile( L"arrow.x", pDevice, &_pArrowMsh , NULL, NULL );
	
	d3d::InitFont( pDevice, 40, L"Segoe Print", &_pFont );

}

int CONTROL::Training( LPARAM lParam, HWND* elem, HWND* menu_elem ){
	
	int level = NILL;
	if( ( (HWND)lParam == elem[LEVEL::EASY] )  ||
		( (HWND)lParam == elem[LEVEL::MEDIUM] )||
		( (HWND)lParam == elem[LEVEL::HARD] )   ){
		
			
			if( (HWND)lParam == elem[LEVEL::EASY] )		level = LEVEL::EASY;
			if( (HWND)lParam == elem[LEVEL::MEDIUM] )	level = LEVEL::MEDIUM;
			if( (HWND)lParam == elem[LEVEL::HARD] ) 	level = LEVEL::HARD;
			
			
			::ShowWindow( menu_elem[smh::STARTMENU], SW_HIDE );
			::ShowWindow( menu_elem[smh::MAINWINDOW], SW_SHOW );
			_curr_ses = TRAINSESSION;
			_map->GenerateScheme( (LEVEL)level, NULL );
			if( !_map->Init() ){ 
				int key = std::time( NULL );
				srand( key % 3  );
				_map->GenerateScheme( (LEVEL)level, NULL );
				_map->Init();
			}
			
	}
	return level;
}

void CONTROL::Draw(){
	RECT wind = _map->GetRect();
	float x = ( wind.left-100 + wind.right ) / 2;
	
	for( int i = 0; i < _life; i ++ ){
		D3DXMATRIX world, rot;
		D3DXMatrixTranslation( &world,
								x + i*STEP*1.5,
								700,
								0.0f );
		D3DXMatrixRotationX( &rot, D3DX_PI / 3 );
		D3DXMatrixMultiply( &world, &rot, &world );
		D3DXMatrixRotationZ( &rot, -D3DX_PI  );
		D3DXMatrixMultiply( &world, &rot, &world );	

		_pDevice->SetTransform( D3DTS_WORLD, &world );
		_pDevice->SetMaterial( &d3d::GREEN_MTRL );
		_pHeartMsh->DrawSubset( 0 );
	}

	D3DXVECTOR3* rec_pos = _map->GetReceiverPos();
	for( int i = 0; i < _n_socket; i++ )
		if( _unlock_server[i] == UNLOCK ){
			D3DXMATRIX world, rot;
			D3DXMatrixTranslation( &world,
								rec_pos[i].x,
								rec_pos[i].y,
								rec_pos[i].z );
		D3DXMatrixRotationX( &rot, D3DX_PI / 3 );
		D3DXMatrixMultiply( &world, &rot, &world );
		
		_pDevice->SetTransform( D3DTS_WORLD, &world );
		_pDevice->SetMaterial( &d3d::GREEN_MTRL );
		_pTickMsh->DrawSubset( 0 );
		}
		_yu = 700.f;
		_yd = rec_pos[0].y-100;
		//
		// for tutorial
		//
		TutCore();
		RECT rect = { 1, 1, 100, 200 };
		
		if( _curr_ses == TUTORSESSION ){

			if( _curr_server != NILL ){
				D3DXMATRIX world, rot;
				D3DXMatrixTranslation( &world,
								rec_pos[_curr_server].x,
								rec_pos[_curr_server].y,
								rec_pos[_curr_server].z );
				D3DXMatrixRotationX( &rot, D3DX_PI / 3 );
				D3DXMatrixMultiply( &world, &rot, &world );
							
				_pDevice->SetTransform( D3DTS_WORLD, &world );
				_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
				D3DMATERIAL9 blue = d3d::BLUE_MTRL;
				blue.Diffuse.a = 0.4;
				_pDevice->SetMaterial( &blue );
				_pLimitedSphereMsh->DrawSubset( 0 );
				_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
			

				for( int i = 0; i < _cnt_pred; i++ ){
					point p;
					int pred = _pred[i];
					if( pred >= SHFOUT ) {
						pred -= SHFOUT;
						p = _map->GetOperPos( pred );
					}
					else p = _map->GetSourcePos( pred );
					
					D3DXMATRIX w, rot;
					D3DXMatrixTranslation( &w,
								p.x,
								p.y + 100.f,
								0.f );
					D3DXMatrixRotationX( &rot, -D3DX_PI / 6 );
					D3DXMatrixMultiply( &w, &rot, &w );
					_pDevice->SetTransform( D3DTS_WORLD, &w );
			
				
					_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
					D3DMATERIAL9 green = d3d::GREEN_MTRL;
					_pDevice->SetMaterial( &green );
					_pArrowMsh->DrawSubset( 0 );
					_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
				}
			}
		}
}

int CONTROL::Test( LPARAM lParam, HWND* menu_elem  ){
	int level = NILL;

	if( ( (HWND)lParam == menu_elem[smh::TEST] ) ||
		( _curr_level > NULL )						)  {			
			level = _curr_level;
			
			wchar_t name[50] ;			
			GetWindowText( menu_elem[smh::PUPILNAMEEDIT], name, 50 );
			_pupils_name = name;
			if( ( _pupils_name == L"" )||( _pupils_name == L"Ваше ФИО" ) ) 	return NILL;

			::ShowWindow( menu_elem[smh::STARTMENU], SW_HIDE );
			::ShowWindow( menu_elem[smh::MAINWINDOW], SW_SHOW );
			_curr_ses = TESTSESSION;
			_point ++;
			if( ( _curr_level < HARD + 1 )/*||( _point < 10 )*/ ) {
				_map->GenerateScheme( (LEVEL)_curr_level, NULL );
				if( !_map->Init() ){ 
					int key = std::time( NULL );
					srand( key % 3  );
					_map->GenerateScheme( (LEVEL)_curr_level, NULL );
					_map->Init();
				}				
			}			

	}
	return level;
}

int CONTROL::Tutorial( LPARAM lParam, HWND* menu_elem ){
	
	int level = NILL;
	if( ( (HWND)lParam == menu_elem[smh::TUTORIAL] )  ){		
			level = LEVEL::EASY;
			::ShowWindow( menu_elem[smh::STARTMENU], SW_HIDE );
			::ShowWindow( menu_elem[smh::MAINWINDOW], SW_SHOW );
			_curr_ses = TUTORSESSION;
			_map->GenerateScheme( (LEVEL)level, /*NULL*/2020327 );
			if( !_map->Init() ){ 
				int key = std::time( NULL );
				srand( key % 3  );
				_map->GenerateScheme( (LEVEL)level, NULL );
				_map->Init();
			}
			//Init( _map, _pDevice );
		_n_socket = _map->GetCountSocket();
		::MessageBox( NULL, gui::GetRule(), L"Задача", 0 );
	}
	return level;
}

void CONTROL::Core( int picking_serv, HWND* sysmenu ){
	
	if( _map->GetValueReceiver( picking_serv ) == true ){ 
		_unlock_server[picking_serv] = UNLOCK;
		::MessageBox( 0, L"Сервер разблокирован!", L"Удача =)", 0 );
	}
	else 
		if( _life > 0 ){ 
			_life--;
			::MessageBox( 0, L"Вы потеряли жизнь!", L"Неудача =(", 0 );
		}
		
	
	if ( _life == 0 ){
		::MessageBox(0, L"Вы провалили тест!", L"Фейл =/", 0 );
		Verdict();
		::ShowWindow( sysmenu[smh::STARTMENU]/*start_menu*/, SW_SHOW );
		::ShowWindow( sysmenu[smh::MAINWINDOW]/*main_hwnd*/, SW_HIDE );
		_curr_ses = NON;
	}
	int done = 0;
	_n_socket = _map->GetCountSocket();
	for( int i = 0; i < _n_socket; i++ )
		if( _unlock_server[i] == UNLOCK ) done ++;
	if( done == _map->GetCountSocket() ) 
		if( ( _curr_level < HARD + 1 )&&(_curr_ses == TESTSESSION ) ) {
				::MessageBox( 0, L"Перевод на следующий уровень.", L"Уровень пройден! :D", 0 );
				done = 0;
				_point ++;
				memset( _unlock_server, LOCK, sizeof( _unlock_server ) );
				_map->GenerateScheme( (LEVEL)_curr_level, NULL );
				if( !_map->Init() ){ 
					int key = std::time( NULL );
					srand( key % 3  );
					_map->GenerateScheme( (LEVEL)_curr_level, NULL );
					_map->Init();
				}
				if( _point % 3 == 0 ) _curr_level++;
			}
		else{
			::MessageBox( 0, L"Хорошо! ;).", L"Уровень пройден! :D", 0 );
			Verdict();
			::ShowWindow( sysmenu[smh::STARTMENU]/*start_menu*/, SW_SHOW );
			::ShowWindow( sysmenu[smh::MAINWINDOW]/*main_hwnd*/, SW_HIDE );
			_curr_ses = NON;
		}

}

void CONTROL::TutCore(){
	if( _curr_ses != TUTORSESSION ) return ;
	int i = 0;
	while( (i < _n_socket)&&(_unlock_server[i] == UNLOCK) ) i++;
	if( i < _n_socket ) _curr_server = i;
	else {
		_curr_server = NILL;
		return;
	}
	
	_cycle_anim ++;

	
	int curr_pred = 0;
	int in1 = 777 , in2 = 777;
	int predok = _map->GetReceiversPort( _curr_server )+ SHFOUT;
	_pred[curr_pred] = predok ;
	int index = 0;
	while( ( index < curr_pred  )||( index == 0 ) ){
		_map->GetIn( predok, &in1, &in2 );
		if( in1 < SHFOUT )  { 
			_pred[curr_pred++] = in2;
			_pred[curr_pred++] = in1;
		}else 
			if( in2 < SHFOUT )  { 
				_pred[curr_pred++] = in1;
				_pred[curr_pred++] = in2;
			}else { 				 
				_pred[curr_pred++] = in1;
				_pred[curr_pred++] = in2;
			}
		if( _pred[++index] >= SHFOUT )	predok =_pred[index];
		else
			if( _pred[++index] >= SHFOUT )	predok =_pred[index];
			else index++;
	}	
	_cnt_pred = curr_pred++;
	
}

float CONTROL::GetCoefOfLearn(){
	float coef = 0;

	if( _life == 3 ) coef = 100./100;
	if( _life == 2 ) coef = 60./100;
	if( _life == 1 ) coef = 20./100;

	if( ( _life == 3 )&&( _point == 9 ) ) coef = 100./100;
	if( ( _life == 2 )&&( _point == 9 ) ) coef = 85./100;
	if( ( _life == 1 )&&( _point == 9 ) ) coef = 70./100;
	if( ( _life == 0 )&&( _point == 9 ) ) coef = 60./100;
	if( ( _life == 0 )&&( _point == 8 ) ) coef = 54./100;
	if( ( _life == 0 )&&( _point == 7 ) ) coef = 48./100;
	if( ( _life == 0 )&&( _point == 6 ) ) coef = 42./100;
	if( ( _life == 0 )&&( _point == 5 ) ) coef = 36./100;
	if( ( _life == 0 )&&( _point == 4 ) ) coef = 30./100;
	if( ( _life == 0 )&&( _point == 3 ) ) coef = 24./100;
	if( ( _life == 0 )&&( _point == 2 ) ) coef = 18./100;
	if( ( _life == 0 )&&( _point == 1 ) ) coef = 12./100;
	if( ( _life == 0 )&&( _point == 0 ) ) coef = 6./100;
	return coef;
}

int CONTROL::Verdict(){
	int mark = MAXMARK * GetCoefOfLearn();
	wchar_t str[50] = L"Ваш результат, ";
	if( _curr_ses == TESTSESSION )
		lstrcat( str, _pupils_name.c_str() );
	else 
		lstrcat( str, L"Вася" );
	lstrcat( str, L", " );
	wchar_t  tmp[10];
	_itow( mark, tmp, 10 );
	lstrcat( str, tmp );
	lstrcpy( tmp,L" баллов!");
	lstrcat( str, tmp );
	::MessageBox( 0, str, L"Конец", 0 );
	return mark;
}

void CONTROL::Release(){	
	d3d::Release<ID3DXMesh*>( _pHeartMsh );
	d3d::Release<ID3DXMesh*>( _pTickMsh );
	d3d::Release<ID3DXMesh*>( _pLimitedSphereMsh );
	d3d::Release<ID3DXMesh*>( _pArrowMsh );
	d3d::Release<ID3DXFont*>( _pFont );
}

int CONTROL::GetCurrentSession(){
	return _curr_ses;
}

void CONTROL::GetYLimit( int *y1, int *y2 ){
	*y1 = _yu;
	*y2 = _yd;
}