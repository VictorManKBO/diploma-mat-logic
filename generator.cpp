//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: generator.cpp
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "generator.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <stack>
#include <algorithm>


int comp( const void *a, const void *b)
{
  return (int)a - (int)b;
}

//
// class Set
/////////////////////////////////
Set::Set(){
	for( int i = 0; i < SETCOUNT; i++ )
		Delete( i );
}

Set::~Set(){
}

void Set::Init( int first, int last ){
	for( int i = 0; i < SETCOUNT; i++ )
		Delete( i );
	for( int i = first; i <= last; i++ )
		_mas[i] = 1;
}


void Set::Add( int el ){
	_mas[el] = 1;
}

int Set::Delete( int el ){
	_mas[el] = 0;
	return el;
}

bool Set::In( int el ){
	return ( _mas[el] == 1 )?true:false;
}

int Set::Size(){
	int size = 0;
	for( int i = 0; i < SETCOUNT; i++ )
		if( _mas[i] > 0 ) size++;
	return size;
}

int Set::MaxCount(){
	return SETCOUNT;
}
//////////////////////////////////
// End of class Set
//

// 
//class Lobj
//////////////////////////////////

Lobj::Lobj(){
}

Lobj::~Lobj(){
} 

void Lobj::SetLvl( LEVEL lvl ){
	_lvl = lvl;

	switch ( _lvl )
	{
		case EASY: _row = 4; _colomn = 3; _cnt = 5;
			break;
		case MEDIUM: _row = 4; _colomn = 5; _cnt = 8;
			break;
		case HARD: _row = 5; _colomn = 5; _cnt = 10;
			break;

		default:
			break;
	}
	_pSource = new int[_row];
	_pReceiver = new int[_row];
	for( int i = 0; i < _row; i++ ){
		_pSource[i] = FREE;
		_pReceiver[i] = FREE;
	}

	RiInit();

	_roCount = 0;// Ещё нет выходных ног операторов
	RrInit();
	RsInit();
	
}

void Lobj::RiInit(){
	_riCount = _cnt;
	for(int i = 0; i < _cnt; i ++ ){
		_riMas[i] = MAS[i];
	}
}

void Lobj::RsInit(){
	_rsCount = _row;
	for(int i = 0; i < _rsCount; i ++ ){
		_rsMas[i] = MAS[i];
	}
}

void Lobj::RrInit(){
	_rrCount = _row;
	for(int i = 0; i < _rrCount; i ++ ){
		_rrMas[i] = MAS[i];
	}
}

int Lobj::RollOperIn(){
	
	int k = rand()%_riCount;
	int number = _riMas[k];
	for(int i = k; i < _riCount - 1  ; i++ )
		_riMas[i] = _riMas[i+1];
	_riCount--;
	_roMas[ _roCount++ ] = number;

	return number;
}

int Lobj::RollOperOut( int num ){
	int number;
	if ( _roCount > 0 ){
		
		int k = rand()%_roCount;
		
		while( ( num == _roMas[k] )&&( _roCount > 1/* ,ьолтше еденицы, чтоб было из чего выбирать и цикл закончился*/ ) ) k = rand()%_roCount;
		if ( num == _roMas[k] ) return NILL;
		number = _roMas[k];
		for(int i = k; i < _roCount - 1  ; i++ )
			_roMas[i] = _roMas[i+1];
		_roCount--;
		return number;
	}
	else return NILL;
}


int Lobj::RollSource(){
	if ( _rsCount > 0 ){
		
		int k = rand()%_rsCount;
		int number = _rsMas[k];
		for(int i = k; i < _rsCount - 1  ; i++ )
			_rsMas[i] = _rsMas[i+1];
		_rsCount--;
		return number;
	}
	else{
		RsInit();// повторная инициализация источников, чтоб занять оставшиеся ноги в некоторых случаях
		return RollSource();
	}
}

int Lobj::RollReceiver(){
	if ( _rrCount > 0 ){
		
		int k = rand()%_rrCount;
		int number = _rrMas[k];
		for(int i = k; i < _rrCount - 1  ; i++ )
			_rrMas[i] = _rrMas[i+1];
		_rrCount--;
		return number;
	}else return NILL;
}

void Lobj::PrepareOper(){
	_prepare_size = _cnt;
	int and = 0, or = 0, xor = 0, imp = 0;
	_prepare_mas[0] = AND;
	_prepare_mas[1] = AND;
	_prepare_mas[2] = OR;
	_prepare_mas[3] = OR;
	_prepare_mas[4] = XOR;
	_prepare_mas[5] = AND;
	_prepare_mas[6] = OR;
	_prepare_mas[7] = IMPLICATION;
	_prepare_mas[8] = XOR;
	_prepare_mas[9] = IMPLICATION;
		
}

LOGICOPERATOR Lobj::RandomOper(){
	if( _prepare_size > 0 ){ 
		int d = rand()%_prepare_size--;
		LOGICOPERATOR ret =  _prepare_mas[d];
		for( int i = d; i < _prepare_size; i++ )
			_prepare_mas[i] = _prepare_mas[i+1];
		return ret;
	}
	return LOGICOPERATOR( rand()%LOGICOPERATOR::COUNT );
}

void Lobj::Init( int key ){
	
	memset( _prepare_mas, NILL, sizeof(_prepare_mas) );
	PrepareOper();

	if( key == NILL ) key = time( NULL );
	srand( key );

	char buffer[80];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char* format = "%d-%B-%Y %I:%M:%S";
	strftime(buffer, 80, format, timeinfo);

	std::ofstream out( "gen.inf", std::ios::app );
	out<< "lvl= " << _lvl << " key= " << key << "	" << buffer << std::endl;
	out.close();

	
	for( int i = 0; i < _cnt; i++ ){		
		_pLgcObj[i]._lgcObj = RandomOper();
		_pLgcObj[i]._in1 = NILL;
		_pLgcObj[i]._in2 = NILL;
		_pLgcObj[i]._out = SHFOUT + i;
	}
	_flag = true;
	while( !Bind() );
	
}

void Lobj::SecondRollSource( int * a, int curr ){
	int source = RollSource();
	if( ( _pLgcObj[curr]._in1 != source )&&( _pLgcObj[curr]._in2 != source ) ) *a = source;
	else {
		source = RollSource();
		if( ( _pLgcObj[curr]._in1 != source )&&( _pLgcObj[curr]._in2 != source ) ) *a = source;
	}
}

bool Lobj::Bind(){
	
	int lostSource = _row;	//Свободные слоты источника
	int lostReceiver = _row;//Свободные слоты приёсника
	int i = 0, j = 0;
	int source;	// Ссылка на текущий оператор

	// part 1
	// Заполнение слотов источников и часть операторов
	//

	//Пока  есть свободные слоты в источнике, к ним подсоединять операторы случайным образом
	while ( lostSource > 0 ){
		if ( _flag ){
			source = RollOperIn();	//Захват случайного оператора
			_pLgcObj[source]._in1 = RollSource(); //Захват случайного источника и присоединение его к первой ноге оператора
			lostSource --;	// - захваченый источник
		}
		if ( _row*0.5 < lostSource ){ // Если осталось ещё больше половины свободных источников, то для второй ноги оператора захватывается другой исочник
			_pLgcObj[source]._in2 = RollSource(); //Захват случайного источника и присоединение его ко второй ноге оператора
			lostSource --;
		}
		else // Иначе же 
			if( ((rand()/(float)RAND_MAX) <= 0.3 )&&( _rsCount > 0 ) ){ // c вероятностью в 30% для второй ноги оператора захватывается другой исочник
				_pLgcObj[source]._in2 = RollSource(); //Захват случайного источника и присоединение его ко второй ноге оператора
				lostSource --;
			}
			else {// если 70%, то второая нога коннектиться с выходом случайного оператора
				int out = RollOperOut( source );
				if ( out != NILL ){ //Если есть что брать
					_pLgcObj[source]._in2 = _pLgcObj[out]._out;
					_flag = true;
				}
				else // Пока не найдена цель для второй ноги, крутить))
					_flag = false;
			}
	}
	
	//part 2
	// Заполнение оставшихся операторов и приёмников
	//1. Операторы, в которых оба входных сокета заняты и операторы которые не соединяются с операторами, у которых таковые свободны, добавляются в список!
	//2. Разыгрываются свободны ноги операторов.
	//3. Свободные выходы законектить с приёмником, если есть такие. Если свободных уже нет, то к приёмнику подсоеденить выходы самого нагруженного оператора(больше всего потомков у которого)
	//4. Проверить на работоспособность и устройчивость.
	//5. Переделать NOT в перемычку вида -[]-
	//!!!После SecondRollSource добавить curr в _roMas, где цепляется на вторую ногу, т.к. он готов для дальнейшей сцепки
	//
	 
	// №1 Формирование списка объектов _roMas
 
	_roCount = 0;
	for( int i = 0; i < _cnt; i++ ){
		if( ( _pLgcObj[i]._in1 > NILL )&&( _pLgcObj[i]._in2 > NILL ) ) 
			for( int j = 0; j < _cnt; j++ ){
				if( (( _pLgcObj[j]._in1 == _pLgcObj[i]._out  )||( _pLgcObj[j]._in2 == _pLgcObj[i]._out  ) )&&
					(( _pLgcObj[j]._in1 > NILL )&&( _pLgcObj[j]._in2 > NILL )) ) _set.Add( i );
				else if( ( _pLgcObj[j]._in1 != _pLgcObj[i]._out  )&&( _pLgcObj[j]._in2 != _pLgcObj[i]._out  ) ) _set.Add( i );
			}
	}
	/* Хз вообще нафига я создал класс Set и НАКОЙ хрен добалвял в него элементы( п. 2.1 ), но вреде как они должны быть в ro, 
	поэтому нижние строчки описывают импорт из Set в ro*/
	Set st = _set;
	_roCount = _set.Size();
	int curr = 0;
	for( int i = 0; i < _set.MaxCount(); i++ ){
		if( _set.In( i ) ) _roMas[curr++] = _set.Delete( i );
	}
	
	//№2

	RsInit();// повторная инициализация источников, чтоб занять оставшиеся ноги в некоторых случаях
	while( _riCount > 0 ){
		int curr = RollOperIn();	// достать оператор со свободной ногой
		int out;					// оператор который станет родителем для оператора curr
		if( _roCount > 0 ) out = RollOperOut( curr ); //находим родителя
		//if( out == NILL ) break;
		//st.Size();
		if( _pLgcObj[curr]._in1 > NILL ){			  //если первая нага занята	
/*-------*/	if( out == NILL ) SecondRollSource( &_pLgcObj[curr]._in2, curr );//для некоторых случаев где массив в -1 уходит
			else _pLgcObj[curr]._in2 = _pLgcObj[out]._out; //цепляем к родителю вторую ногу
			//_roMas[_roCount++] = curr;				  //так как все ноги заняты, то даём возможность оператору curr стать родителем	
		}
		else if( _pLgcObj[curr]._in2 > NILL ){        //если вторая нога занята
/*-------*/ 	if( out == NILL ) SecondRollSource( &_pLgcObj[curr]._in1, curr );//для некоторых случаев где массив в -1 уходит
				else _pLgcObj[curr]._in1 = _pLgcObj[out]._out; //цепляем к родителю вторую ногу
				//_roMas[_roCount++] = curr;				  //Так как все ноги заняты, то даём возможность оператору curr стать родителем	
			}
		else {                                        //иначе свободны две ноги и обе цепляем к родителям
/*-------*/ if( out == NILL ) SecondRollSource( &_pLgcObj[curr]._in1, curr );//для некоторых случаев где массив в -1 уходит
			else _pLgcObj[curr]._in1 = _pLgcObj[out]._out; //цепляем первую ногу к родителю
			if( _roCount > 0 ) out = RollOperOut( curr );// находим родителя для второй и если такой нету - ругаемся!
/*-------*/ if( out == NILL ) SecondRollSource( &_pLgcObj[curr]._in2, curr );//для некоторых случаев где массив в -1 уходит
			else _pLgcObj[curr]._in2 = _pLgcObj[out]._out; //цепляем вторую ногу к родителю
		}
	}

	//---------------------------------
	//поиск нетронутых попок для приёмника
	_pReceiver = new int [_rrCount];
	int untouchedAss[sizeof(MAS)] = { 0 };
	for( int i = 0; i < _cnt; i++ ){
		int in1 = _pLgcObj[i]._in1;
		int in2 = _pLgcObj[i]._in2;
		if( in1 >= SHFOUT )  untouchedAss[ in1 - SHFOUT ] = 1;
		if( in2 >= SHFOUT )  untouchedAss[ in2 - SHFOUT ] = 1;
	}
	for( int i = 0; i < _cnt; i++ ){
		if( ( untouchedAss[i] == 0 )&&( _rrCount > 0 ) ){
			curr = RollReceiver();
			_pReceiver[curr] = i;
		}
	}
	//выставить попки в порядке нетронутости
	
	std::stack<int> stc;
	for( int i = 0; i < _cnt; i++ ){
		untouchedAss[i] = 0;
		
		for( int j = 0; j < _cnt; j++ )
			if( ( _pLgcObj[j]._in1 == _pLgcObj[i]._out )||( _pLgcObj[j]._in2 == _pLgcObj[i]._out ) ){
				untouchedAss[i]++;
				stc.push( _pLgcObj[j]._out );				
			}
	
		while ( !stc.empty() ){
			int out = stc.top();
			stc.pop();
			for( int j = 0; j < _cnt; j++ )
				if( ( _pLgcObj[j]._in1 == out )||( _pLgcObj[j]._in2 == out ) ){
					untouchedAss[i]++;
					stc.push( _pLgcObj[j]._out );				
				}
		}
	}

	
	while ( _rrCount > 0 ){
		int max = 0;
		for( int i = 0; i < _cnt; i++ )
			if ( untouchedAss[i] > untouchedAss[max] ) max = i;
		curr = RollReceiver();
		int del = untouchedAss[max];
		 _pReceiver[curr] = max ;//+ SHFOUT;
		for( int i = 0; i < _cnt; i++ )
			if ( untouchedAss[i] == del ) untouchedAss[i] = 0;

		
	}


	return true;
	
}

void Lobj::Releas(){
	
	delete( _pSource );
	delete( _pReceiver );

}

std::string Equ( LOGICOPERATOR a ){
	switch ( a )
	{
	
	case AND: return "AND";
		break;
	case OR: return "OR";
		break;
	case XOR: return "XOR";
		break;
	case IMPLICATION: return "IMPLICATION";
		break;
	
	default:
		break;
	}
}

void Lobj::PrintResult(){
	
	std::ofstream fout( "The Scheme.txt" );
	for( int i = 0; i < _cnt; i++ ){		
		fout<<  _pLgcObj[i]._in1 << " >- " << std::endl;
		fout<< "      " << Equ( _pLgcObj[i]._lgcObj ) << " -> " << _pLgcObj[i]._out << std::endl;
		fout<<  _pLgcObj[i]._in2 << " >- " <<std::endl;
		fout<< "----------------" << std::endl;
		
	}
	for( int i = 0; i < _row; i++ )
			fout<< _pReceiver[i] << "  ";
	fout<< std::endl;
	
	fout.close();
}

int Lobj::GetCount(){
	return _cnt;
}

int Lobj::GetCountSocket(){
	return _row;
}

int Lobj::GetOut( int curr ){
	if( curr < SHFOUT )	return _pLgcObj[curr]._out;
	else return curr;
}

int Lobj::GetIn1( int curr ){
	return _pLgcObj[curr]._in1;
}

int Lobj::GetIn2( int curr ){
	return _pLgcObj[curr]._in2;
}

int Lobj::GetObj( int curr ){
	return _pLgcObj[curr]._lgcObj;
}

void Lobj::SwapIn( int curr ){
	int temp = _pLgcObj[curr]._in1;
	_pLgcObj[curr]._in1 = _pLgcObj[curr]._in2;
	_pLgcObj[curr]._in2 = temp;
}



