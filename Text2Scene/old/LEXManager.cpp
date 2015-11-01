
#include "LEXManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLEXManager::CLEXManager()
{
	_m_numLEX = 0 ;
	_m_ptrLEX = NULL ;
}

CLEXManager::~CLEXManager()
{
	if ( _m_ptrLEX )
	{
		for (register unsigned int i=0;i<_m_numLEX;i++ )
		{
			if ( !_m_ptrLEX[i] ) continue ;
			delete [] _m_ptrLEX[i]->lexeme ;
			delete _m_ptrLEX[i] ;
		}
		delete [] _m_ptrLEX ;
		_m_ptrLEX = NULL ;
	}

}

//////////////////////////////////////////////////////////////////////
// LEXFindPos
// insensible à la case et aux accents
// trouver le tag du lexeme connu le plus similaire. 

//////////////////////////////////////////////////////////////////////
unsigned int CLEXManager::LEXFindPos( CString& string )
{
	if ( !_m_ptrLEX ) return 0 ;
	register unsigned i,j ;
	CString s = string ;
	// MISE EN FORME
	CString t = s ;
	// pb de la case
	t.MakeLower() ;
	// pb des accents
	for(i=0;i<(unsigned)t.GetLength();i++)	if ( t.Mid(i,1)=="é" ||	t.Mid(i,1)=="è" )	t = t.Left(i) + "e" + t.Mid(i+1) ;
	// chercher une occurence similaire
	unsigned pos = 0 ;
	unsigned diffmin = 100000 ;
	for (i=0; i<_m_numLEX; i++)
	{
		if ( !_m_ptrLEX[i] ) continue ;
		for(j=0; j<_m_ptrLEX[i]->count ;j++)
		{
			int k = t.Find( _m_ptrLEX[i]->lexeme[j] ) ;
			if ( k>=0 )
			{
				// ne comparer que des string de longueurs approchées  
				if ( _m_ptrLEX[i]->lexeme[j].GetLength()>(t.GetLength()>>1)  )
				{
					// chercher le mot a comparer
					CString w = t.Mid(k); k = w.Find(" ") ;	if ( k>0 ) w = w.Mid(0,k) ;
					// prendre la string trouvée la plus ajustée
					unsigned diff = _CompString( _m_ptrLEX[i]->lexeme[j] , w ) ;
					if ( diff<diffmin )
					{
						diffmin = diff ;
						pos = _m_ptrLEX[i]->pos ;
					}
				}
			}
		}
	}
	return pos ;
}

//////////////////////////////////////////////////////////////////////
unsigned int	CLEXManager::_CompString( CString& s, CString& t)
{
	unsigned diff = abs( s.GetLength() - t.GetLength() ) ;
	unsigned len = ( s.GetLength()<t.GetLength() ? s.GetLength() : t.GetLength() ) ;
	for( register unsigned i=0;i<len;i++)	if ( s.Mid(i,1)!=t.Mid(i,1) ) diff++ ;
	return diff ;
}

//////////////////////////////////////////////////////////////////////
// LEXCreateLexicon
// Créer les lexiques appropriés

//////////////////////////////////////////////////////////////////////
void* CLEXManager::LEXCreateLexicon( char* filename, unsigned int pos )
{
	CFile file;
	int size;
	char buffer[MAX_FILESIZE];
	char string[MAX_BUFFER];
	int n,p;
	ZeroMemory((void*)buffer,MAX_FILESIZE);
	ZeroMemory((void*)string,MAX_BUFFER);
	CString s("") ;
	if ( file.Open(filename,CFile::modeRead)!=NULL )
	{
		// parse all text
		// 1. count lexeme number
		size = file.Read(buffer,MAX_FILESIZE);
		file.Close();
		if ( !size ) return NULL ;
		int count = 0 ;
		p=0;
		do
		{
			// read a line
			n = 0;	while( buffer[p]!='\n' && n<MAX_BUFFER && p<MAX_FILESIZE ) string[n++] = buffer[p++] ; string[--n] = 0 ; p++;
			s = string ;
			if ( s!="" && s.Left(1)!="#" ) count++ ;
		} while ( p<MAX_FILESIZE );
		//
		if ( !count ) return NULL ;
		// 2. create lexicon
		SLexicon* ptr = new SLexicon ;
		ptr->pos = pos ;
		ptr->count = count ;
		ptr->lexeme = new CString[count] ;
		// 3. read and store each lexeme
		if ( file.Open(filename,CFile::modeRead)!=NULL )
		{
			size = file.Read(buffer,MAX_FILESIZE);
			file.Close();
			if ( !size ) return NULL ;
			// parse all text
			p=0;
			count=0;
			do
			{
				// read a line
				n = 0;	while( buffer[p]!='\n' && n<MAX_BUFFER && p<MAX_FILESIZE ) string[n++] = buffer[p++] ; string[--n] = 0 ; p++;
				s = string ;
				s.MakeLower() ;
				if ( s!="" && s.Left(1)!="#" )
				{
					// crop word
					while ( s.Left(1)==" " )	s=s.Mid(1,s.GetLength());
					while ( s.Right(1)==" " )	s=s.Left(s.GetLength()-1);
					ptr->lexeme[count++] = s ;
				}
			} while ( p<MAX_FILESIZE );
			// end success - return lexicon ptr
			return (void*)ptr ;
		}
		delete [] ptr->lexeme ;
		delete ptr ;
	}
	return NULL ;
}

//////////////////////////////////////////////////////////////////////
void* CLEXManager::LEXLoad(char* filename, unsigned pos )
{
	_m_numLEX++ ;
	SLexicon** ptr = new SLexicon*[_m_numLEX] ;
	if ( _m_ptrLEX )
	{
		for( register unsigned i=0;i<_m_numLEX;i++)	ptr[i] = _m_ptrLEX[i] ;
		delete [] _m_ptrLEX ;
	}
	_m_ptrLEX = ptr ;
	_m_ptrLEX[_m_numLEX - 1] =  (SLexicon*)LEXCreateLexicon( filename, pos ) ;
	return _m_ptrLEX[_m_numLEX - 1] ;
}


