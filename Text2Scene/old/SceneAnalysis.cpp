// SceneAnalysis.cpp: implementation of the CSceneAnalysis class.
//
//////////////////////////////////////////////////////////////////////

#include "SceneAnalysis.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneAnalysis::CSceneAnalysis()
{
}

CSceneAnalysis::~CSceneAnalysis()
{
	POSITION pos = mDico.GetStartPosition() ;
	while ( pos != NULL )
	{
		CString name ;
		void* ptr ;
		mDico.GetNextAssoc( pos, name, ptr ) ;
		CDico* pDico = (CDico*)ptr ;
		while ( pDico )
		{
			CDico* p = pDico ;
			pDico = pDico->mNext ;
			delete p ;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Find the "Keyword" from "String" in "Dico" 
CString CSceneAnalysis::Find( CString dico, CString& string, int& cpStart, int& cpEnd )
{
	// find dico
	CDico* pDico = NULL ;
	POSITION pos = mDico.GetStartPosition() ;
	while ( pos != NULL )
	{
		CString name ;
		void* ptr ;
		mDico.GetNextAssoc( pos, name, ptr ) ;
		if ( name == dico )
		{
			pDico = (CDico*)ptr ;
			break ;
		}
	}
	// find occurence
	while ( pDico )
	{
		if ( pDico->mString.GetSize()>0 )
		{
			for ( register int i=0; i<pDico->mString.GetSize();i++)
			{
				cpStart = string.Find( pDico->mString[i] ) ;
				if ( cpStart!=-1 )
				{
					string = string.Mid(0,cpStart) + string.Mid( cpStart + strlen(pDico->mString[i])) ;

					cpEnd = cpStart + strlen(pDico->mString[i]) ;

					return pDico->mKey ;
				}
			}
		}
		pDico = pDico->mNext ;
	}
	return "" ;
}

//////////////////////////////////////////////////////////////////////
// Find a "Dico" containing "String"
CDico* CSceneAnalysis::FindDico(CString string )
{
	POSITION pos = mDico.GetStartPosition() ;
	while ( pos != NULL )
	{
		CString name ;
		void* ptr ;
		mDico.GetNextAssoc( pos, name, ptr ) ;
		// find occurence
		CDico* pDico = (CDico*)ptr ;
		while ( pDico )
		{
			if ( pDico->mString.GetSize()>0 )
			{
				for ( register int i=0; i<pDico->mString.GetSize();i++)
				{
					if ( string == pDico->mString[i] )
						return pDico ;
				}
			}
			else
			if ( pDico->mKey == string )
				return pDico ;

			pDico = pDico->mNext ;
		}
	}
	return NULL ;
}

//////////////////////////////////////////////////////////////////////
// Find a "Dico" containing "Keyword"
CDico* CSceneAnalysis::FindKey(CString key )
{
	POSITION pos = mDico.GetStartPosition() ;
	while ( pos != NULL )
	{
		CString name ;
		void* ptr ;
		mDico.GetNextAssoc( pos, name, ptr ) ;
		// find occurence
		CDico* pDico = (CDico*)ptr ;
		while ( pDico )
		{
			if ( pDico->mKey == key )
				return pDico ;
			pDico = pDico->mNext ;
		}
	}
	return NULL ;
}

//////////////////////////////////////////////////////////////////////
// Get Dico pointer from keyword
CDico* CSceneAnalysis::GetPtr(const char* pkey)
{
	CString key = pkey ;
	void* ptr ;
	mDico.Lookup(key, ptr ) ;
	return (CDico*)ptr ;
}

//////////////////////////////////////////////////////////////////////
// is "string" of that "Style" ?
bool CSceneAnalysis::Is(const char* pStyle, const char* pString)
{
	CString style = pStyle ;
	CString string = pString ;
	POSITION pos = mDico.GetStartPosition() ;
	while ( pos != NULL )
	{
		CString name ;
		void* ptr ;
		mDico.GetNextAssoc( pos, name, ptr ) ;
		CDico* pDico = (CDico*)ptr ;
		while ( pDico )
		{
			if ( style == pDico->mKey && pDico->mString.GetSize()>0 )
			{
				for ( register int i=0; i<pDico->mString.GetSize();i++)
				{
					if ( pDico->mString[i]==string )
						return true ;
				}
			}
			pDico = pDico->mNext ;
		}
	}
	return false ;
}


//////////////////////////////////////////////////////////////////////
CString CSceneAnalysis::GetStyleName(const char* Style)
{
	CString string = Style ;
	string.Remove('$') ;
	return string ;
}

//////////////////////////////////////////////////////////////////////
int CSceneAnalysis::GetStyleId(CString style)
{
	CDico* pDico = GetPtr( SC_DICOENTRY_STYLES ) ;
	int Id = 0 ;
	while ( pDico )
	{
		if ( pDico->mKey == style )
			return Id ;

		Id++ ;
		pDico = pDico->mNext ;
	}
	return 0 ;
}

//////////////////////////////////////////////////////////////////////
char* CSceneAnalysis::GetStyle(const char* pString, int* pIdx)
{
	CString string = pString ;
	CDico* pDico = GetPtr( SC_DICOENTRY_STYLES ) ;
	int idx = 0 ;
	while ( pDico )
	{
		for ( register int i=0; i<pDico->mString.GetSize();i++)
		{
			if ( pDico->mString[i]==string )
			{
				*pIdx = idx ;
				return (char*)(LPCTSTR)pDico->mKey ;
			}
		}
		idx++ ;
		pDico = pDico->mNext ;
	}
	*pIdx = -1 ; 
	return NULL ;
}


//////////////////////////////////////////////////////////////////////
CDico* CSceneAnalysis::_AddDico( CString name, CString key)
{
	void* ptr ;
	mDico.Lookup( name, ptr ) ;
	CDico* pDico = (CDico*)ptr ;
	CDico* p = NULL;
	if ( !pDico )
	{
		p = pDico = new CDico ;
		mDico.SetAt( name, pDico ) ;
	}
	else
	{
		p = pDico;	while (p->mNext) {	p = p->mNext ;	} ;
		p = p->mNext = new CDico ;
	}
	p->mKey = key ;
	return p;
}

//////////////////////////////////////////////////////////////////////
bool CSceneAnalysis::Load(const char* filename)
{
	CFile file;
	char buffer[SA_MAX_FILESIZE];
	char string[SA_MAX_BUFFER];
	ZeroMemory((void*)buffer,SA_MAX_FILESIZE);
	ZeroMemory((void*)string,SA_MAX_BUFFER);

	CString classe(_T ("class")) ;
	CString shadow(_T("shadow")) ;
	CString	format(_T("format"));
	CString	align(_T("align"));
	CString caps(_T("caps"));
	CString li(_T("li"));
	CString ri(_T("ri"));

	if ( file.Open(filename,CFile::modeRead)!=NULL )
	{
		// read file
		int size = file.Read(buffer,SA_MAX_FILESIZE);
		file.Close();
		if ( !size ) return false ;
		// parse file
		int p=0;
		CString current("") ;
		CDico *pDico = NULL ;
		do
		{
			// read a line
			int n = 0;
			while( buffer[p]!='\n' && n<SA_MAX_BUFFER && p<size && p<SA_MAX_FILESIZE ) string[n++] = buffer[p++] ; 
			p++ ;
			// parse a line
			if ( n>1 )
			{
				string[n-1] = 0 ; 
				CString s(string) ;
				int begin = s.Find("[") ;
				int end = s.Find("]") ;
				// create new dico
				if ( begin!=-1 && end!=-1 )
				{
					current = s.Mid( begin+1 , end - begin - 1 ) ;
					mDico[ (LPCTSTR)current ] = NULL ;
				}
				else
				{
					// add new entry
					if ( s.Find(classe)!=-1 )
					{
						int begin = s.Find(classe) + classe.GetLength() ;
						s = s.Mid(begin) ;
						s.TrimLeft() ;
						s.TrimRight() ;
						pDico = _AddDico( current, s ) ;
					}
					else if ( pDico && s.Find(shadow)!=-1 )
					{
						// populate entry
						register int i = 0 ;
						do
						{
							i = s.Find("\"", i ) ;
							if ( i!=-1 )
							{
								register int j = s.Find("\"", i + 1 ) ;
								if ( j!=-1 )
								{
									CString word = s.Mid( i + 1, j - i - 1) ;
									pDico->mString.Add( word ) ;
								}
								i = j + 1 ;
							}
						} while ( i!=-1 ) ;
					}
					// set style format
					else if ( pDico && s.Find(format)!=-1 )
					{
						CString word("") ;
						CString type = s.Mid(format.GetLength()) ;
						type.MakeLower() ;
						type.TrimLeft() ;
						type.TrimRight() ;
						// populate entry
						register int i = 0 ;
						register int j = 0 ;
						do
						{
							j = type.Find(" ", i ) ;
							if ( j<0 )
								word = type.Mid( i ) ;
							else
								word = type.Mid( i, j - i) ;

							if ( word=="bold" )
							{
								pDico->mStyle.mChar |= CH_BOLD ;
							}
							else if ( word=="italic" )
							{
								pDico->mStyle.mChar |= CH_ITALIC ;
							}
							else if ( word=="underline" )
							{
								pDico->mStyle.mChar |= CH_UNDERLINE ;
							}
							i = j + 1 ;
						} while ( j!=-1 ) ;
					}
					// set style algnment
					else if ( pDico && s.Find(align)!=-1 )
					{
						CString type = s.Mid(align.GetLength()) ;
						type.MakeLower() ;
						type.TrimLeft() ;
						type.TrimRight() ;
						if ( type=="left" )			pDico->mStyle.mAlign = PR_LEFT ;
						else if ( type=="center" )	pDico->mStyle.mAlign = PR_CENTER ;
						else if ( type=="right" )	pDico->mStyle.mAlign = PR_RIGHT ;
						else if ( type=="justify" )	pDico->mStyle.mAlign = PR_JUSTIFY ;
					}
					// set style caps or not
					else if ( pDico && s.Find(caps)!=-1 )
					{
						pDico->mStyle.mCaps = true ;
					}
					// set style left indent
					else if ( pDico && s.Find(li)!=-1 )
					{
						CString value = s.Mid(li.GetLength()) ;
						value.TrimLeft() ;
						value.TrimRight() ;
						pDico->mStyle.mLeftIndent = atoi((LPCTSTR)value) ;
					}
					// set style right indent
					else if ( pDico && s.Find(ri)!=-1 )
					{
						CString value = s.Mid(ri.GetLength()) ;
						value.TrimLeft() ;
						value.TrimRight() ;
						pDico->mStyle.mRightIndent = atoi((LPCTSTR)value) ;
					}
				}
			}
		} while ( p<size && p<SA_MAX_FILESIZE );
		return true ;
	}
	return false;
}

