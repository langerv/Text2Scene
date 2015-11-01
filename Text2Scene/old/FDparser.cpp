// FDparser.cpp: implementation of the CFDparser class.
//
//////////////////////////////////////////////////////////////////////

//#include <afxwin.h>
//#include <afxadv.h>
#include "FDparser.h"
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

CFDparser::CFDparser()
{
	_m_pActor = NULL ;
	_m_pScene = NULL ;
	_m_pDialog = NULL ;
	_m_NumActor = 0 ;
	_m_NumScene = 0 ;
	_m_NumObject = 0 ;

	m_SitCpStart = m_SitCpEnd = m_PerCpStart = m_PerCpEnd = -1 ;

	LEXLoad( SC_LEXICON_FILE0, NNPM ) ;
	LEXLoad( SC_LEXICON_FILE1, NNPF ) ;
	LEXLoad( SC_LEXICON_FILE2, NNJM ) ;
	LEXLoad( SC_LEXICON_FILE3, NNJF ) ;

	Load(SC_INIT_FILE) ;

	srand((unsigned)(time(NULL)));
}

CFDparser::~CFDparser()
{
	_DELETE_ACTORS() ;
	_DELETE_SCENES() ;
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// IMPORT
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
bool CFDparser::Open(char* filename)
{
	FILE *fp;
	int ec;
	if ( !(fp = fopen(filename, "r")) )
		return false ;
	//
	Close() ;
	//
	if ( (ec = ecRtfParse(fp)) != ecOK )
	{
		fclose(fp);
		return false ;
	}
	fclose(fp);
	//
	// Construire les listes 
	SRTFObject*	pRTFobj = m_pObjectList ;
	CFDscene* pCurrentScene = NULL ;
	CFDactor* pCurrentActor = NULL ;
	SFDreply* pCurrentReply = NULL ;
	while( pRTFobj )
	{
		if ( pRTFobj->szObjectStyle )
		{
			if ( Is(SC_STYLEENTRY_TITLE, pRTFobj->szObjectStyle->szStyleName) )
			{
				// scènes
				pCurrentScene = _CreateScene( pRTFobj->szObjectInfo ) ;

				if ( pCurrentScene )
				{
					pRTFobj->szCpStart1 = pCurrentScene->m_SitCpStart ;
					pRTFobj->szCpEnd1 = pCurrentScene->m_SitCpEnd ;
					pRTFobj->szCpStart2 = pCurrentScene->m_PerCpStart ;
					pRTFobj->szCpEnd2 = pCurrentScene->m_PerCpEnd ;
				}

			}
			else 
			if ( Is(SC_STYLEENTRY_TRANSITION, pRTFobj->szObjectStyle->szStyleName) )
			{
				// transitions
				// ...
			}
			else 
			if ( Is(SC_STYLEENTRY_PARENTHESIS, pRTFobj->szObjectStyle->szStyleName) )
			{
				// parenthèses
				// ...
			}
			else 
			if ( Is(SC_STYLEENTRY_CHARACTER, pRTFobj->szObjectStyle->szStyleName) )
			{
				// répliques
				pCurrentActor = _CreateActor( pRTFobj->szObjectInfo ) ;

				if ( pCurrentActor )
				{
					pRTFobj->szCpStart1 = pCurrentActor->m_CpStart ;
					pRTFobj->szCpEnd1 = pCurrentActor->m_CpEnd ;
				}

			}
			else 
			if ( Is(SC_STYLEENTRY_DIALOG, pRTFobj->szObjectStyle->szStyleName) )
			{
				// répliques
				SFDreply* pR = pCurrentReply ;
				pCurrentReply = pCurrentActor->AddReply( pCurrentScene, pRTFobj->szObjectInfo ) ;

				if ( _m_pDialog == NULL )
					_m_pDialog = pCurrentReply ;

				if ( pR )
					pR->pCoSuiv = pCurrentReply ;

				if ( pCurrentReply )
					pCurrentReply->pCoPrec = pR ;
			}
		}
		_m_NumObject++ ;
		pRTFobj = pRTFobj->pNext ;
	}
	return true ;
}

//////////////////////////////////////////////////////////////////////
void CFDparser::Close()
{
	_m_pDialog = NULL ;
	_m_NumObject = 0 ;
	//
	_DELETE_ACTORS() ;
	_DELETE_SCENES() ;
	//
	_DELETE_STYLELIST() ;
	_DELETE_OBJECTLIST() ;
}

/*
//////////////////////////////////////////////////////////////////////
bool CFDparser::Import( CEBDoc* pDoc)
{
	// first create actors casting
	_CreateDocIdCards( pDoc ) ;

	// Import scenes
	CFDscene* pS = GetScene() ;
	while( pS )
	{
		// create scene
		CEBOScene* pScene = _CreateDocScene( pDoc, pS ) ;

		if ( pScene )
		{
			// create actors
			_CreateDocActors( pDoc, pS, pScene ) ;

			// create scene dialogs
			SFDreply* pR = GetDialog() ;
			while( pR )
			{
				if ( pS == pR->pScene )
				{
					// (CEBOScene*)pScene ; (CEBDialogLine*)pR->pDialogLine
					ASSERT(pR->pDialogLine != NULL);
				}
				pR = pR->pCoSuiv ;
			}
		}

		pS = (CFDscene*)pS->m_pNext ;
	}
	// close all
	return true ;
}
*/

//////////////////////////////////////////////////////////////////////
CFDscene* CFDparser::_CreateScene(char* pDefname )
{
	CFDscene* ptr = NULL;
	if ( !_m_pScene )
		ptr = _m_pScene = new CFDscene( /*pDefname*/ ) ;
	else
	{
		ptr = _m_pScene; while (ptr->m_pNext) { ptr = (CFDscene*)ptr->m_pNext;	} ;
		ptr->m_pNext = (CFDobject*)new CFDscene( /*pDefname*/ ) ;
		ptr = (CFDscene*)ptr->m_pNext ;
	}
	if ( !ptr )
		return NULL;

	// parse and numerate scene
	_Parse( pDefname, ptr, _m_NumScene + 1 ) ;

	// end
	_m_NumScene++ ; 

	return ptr ;
} ;

//////////////////////////////////////////////////////////////////////
CFDactor* CFDparser::_CreateActor(char* pDefname )
{
	CFDactor* ptr = NULL;
	if ( !_m_pActor )	ptr = _m_pActor = new CFDactor( pDefname ) ;
	else
	{
		CString name( pDefname ) ; 
		name.MakeUpper() ;
		ptr = _m_pActor; 
		while (ptr->m_pNext) 
		{
			if ( name.Find(ptr->m_Name)>=0 )
				return ptr ;
			ptr = (CFDactor*)ptr->m_pNext;	
		} ;
		if ( name.Find(ptr->m_Name)>=0 )
			return ptr ;
		ptr->m_pNext = (CFDobject*)new CFDactor( pDefname ) ;
		ptr = (CFDactor*)ptr->m_pNext ;
	}
	if ( !ptr )
		return NULL;
	// parse
	_Parse(ptr->m_Name, ptr ) ;
	// end
	_m_NumActor++ ; 
	return ptr ;
}

//////////////////////////////////////////////////////////////////////
/// Find global IdCard
/*
CEBIdCard* CFDparser::_FindIdCard(CEBDoc* pDoc, CFDactor* pActor)
{
	for(CEBCell* Current = pDoc->mCharactersList->GetFirst(); Current;
													Current = Current->GetNext() )
	{
		CEBIdCard* pIdCard = (CEBIdCard*)Current->GetElem() ;
		if ( !stricmp( pActor->m_Name, pIdCard->GetName()) )
		{
				return pIdCard ;
*/
/*
			// test du sexe ;
			if ( pActor->m_Sexe == SC_GENDER_FEMALE && pIdCard->mFemale )
				return pIdCard ;
			if ( pActor->m_Sexe == SC_GENDER_MALE && pIdCard->mMale )
				return pIdCard ;
*/
/*
		}
	}
	return NULL ;
}
*/

//////////////////////////////////////////////////////////////////////
SFDreply* CFDactor::AddReply(CFDscene* pS, char* pString)
{
	SFDdialog_group* pD = NULL;
	if ( !m_Dialog )
	{
		pD = m_Dialog = new SFDdialog_group ;
		pD->pScene = pS ;
		pD->pReply = NULL ;
		pD->pNext = NULL ;
	}
	else
	{
		pD = m_Dialog; 
		while ( pD->pNext ) 
		{
			if ( pD->pScene == pS )
				break ;
			pD = pD->pNext;	
		} ;
		if ( pD->pScene != pS && !pD->pNext )
		{
			pD->pNext = new SFDdialog_group ;
			pD = pD->pNext ;
			pD->pScene = pS ;
			pD->pReply = NULL ;
			pD->pNext = NULL ;
		}
	}
	return ( pD ? _AddReply( pD, pString ) : NULL ) ;
};

//////////////////////////////////////////////////////////////////////
SFDreply* CFDactor::_AddReply(SFDdialog_group* pD, char* pString)
{
	SFDreply* pR = NULL;
	if ( !pD->pReply )	pR = pD->pReply = new SFDreply ;
	else
	{
		pR = pD->pReply; 
		while (pR->pNext) { pR = pR->pNext;	} ;
		pR->pNext = new SFDreply ;
		pR = pR->pNext ;
	}
	if ( !pR ) return NULL;
	pR->rep = pString ;
	// filtrage de la réplique
	pR->rep.TrimLeft() ;
	pR->rep.TrimRight() ;
	// fin filtrage de la réplique
	pR->pScene = pD->pScene ;
	pR->pSpeaker = this ;
	pR->pCoPrec = NULL ;
	pR->pCoSuiv = NULL ;
	//
	pR->pDialogLine = NULL ;
	//
	pR->pNext = NULL ;
	return pR ;
}

//////////////////////////////////////////////////////////////////////
CFDactor::CFDactor(char* pDefname)
{
	// store name
	m_Name = pDefname ; 
	m_Name.MakeUpper() ;
	Crop( m_Name ) ;
	//
	m_Dialog = NULL ;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// ANALYSE
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/*
CEBOScene* CFDparser::AnalyseScene( CEBDoc* pDoc, CString string )
{
	if ( string=="" )
		return NULL ;

	// format text
	string.TrimLeft() ;
	string.TrimRight() ;

	// check existence
//	if ( IsSceneIdExist( pDoc, string ) == true )
//		return NULL ;

	// then create
	CFDscene scene ;

	m_SitCpStart = m_SitCpEnd = m_PerCpStart = m_PerCpEnd = -1 ;

	_Parse(string, &scene) ;

	m_SitCpStart = scene.m_SitCpStart ;
	m_SitCpEnd = scene.m_SitCpEnd ;
	m_PerCpStart = scene.m_PerCpStart ;
	m_PerCpEnd = scene.m_PerCpEnd ;

	CEBOScene* pScene = IsSceneExist( pDoc, string ) ;

	if ( !pScene )
	{
		// create scene
		pScene = pDoc->CreateScene(0, _GetInsertAfterScene(pDoc, scene.m_Title) ) ;
	}

	if ( !pScene )
		return NULL ;

	// UPDATE TITLE
	// title => original string title (as is RTF file / use as a "key" for recognition)
	pScene->SetTitle( (LPCTSTR)scene.m_Title ) ;
	CString Title(scene.m_Title) ;

	// name => always uppercase string title / for display purpose
	Title.MakeUpper() ;
	pDoc->ChangeObjectName( (CEBBase*)pScene, (LPCTSTR)Title ) ;

	// UPDATE PERIOD (SKYBOX)
	// find skybox
	CFDscene filename ;
	CEBBase* pNewData = NULL ;
	for(CEBCell* Current = CEBOScene::mSkyList->GetFirst(); Current;
														Current = Current->GetNext() )
	{
		_Parse(Current->GetElem()->GetName(), &filename) ;
		if ( filename.m_Period ==  scene.m_Period )
		{
			pNewData = (CEBBase*)Current->GetElem() ;
			break ;
		}
	}

	// set skybox
	if ( !pNewData )
	{
		// if no skybox has been found, take first
		pNewData = (CEBBase*)CEBOScene::mSkyList->GetFirst()->GetElem() ;
	}
	pDoc->ChangeParamFromUserId( pScene, CEBOScene::_SKY_ , pNewData, sizeof(pNewData) ) ;

	// UPDATE LOCATION
	// set location
	// ...

	return pScene ;
}
*/

//////////////////////////////////////////////////////////////////////
/*
CEBOScene* CFDparser::_GetInsertAfterScene(CEBDoc* pDoc, CString name)
{
	if ( !pDoc )
		return NULL ;

	if ( name == "" )
		return NULL ;

	CString sId0 = GetSceneId(name) ;

	for(CEBCell* Current = pDoc->mScenesList->GetLast(); Current;
													Current = Current->GetPrev() )
	{
		CEBOScene* pScene = (CEBOScene*)Current->GetElem() ;

		CString sId = GetSceneId( pScene->GetTitle() ) ;

		if ( _CompareId( sId, sId0 )<0 )
			return pScene ;
	}
	return NULL ;
}
*/

//////////////////////////////////////////////////////////////////////
/*
CEBOScene* CFDparser::_CreateDocScene( CEBDoc* pDoc, CFDscene* pS )
{
	if ( !pDoc )
		return NULL ;

	CEBOScene* pAfterScene = _GetInsertAfterScene(pDoc, pS->m_Title) ;

	// create scene
	CEBOScene* pScene = pDoc->CreateScene(0, pAfterScene ) ;
	if ( !pScene )
		return NULL ;

	// UPDATE TITLE
	// title => original string title (as is RTF file / use as a "key" for recognition)
	pScene->SetTitle( (LPCTSTR)pS->m_Title ) ;

	CString Title(pS->m_Title) ;

	// name => always uppercase string title / for display purpose
	Title.MakeUpper() ;

	pDoc->ChangeObjectName( (CEBBase*)pScene, (LPCTSTR)Title ) ;

	// UPDATE PERIOD (SKYBOX)
	// find skybox
	CFDscene filename ;
	CEBBase* pNewData = NULL ;
	for(CEBCell* Current = CEBOScene::mSkyList->GetFirst(); Current;
														Current = Current->GetNext() )
	{
		_Parse(Current->GetElem()->GetName(), &filename) ;
		if ( filename.m_Period ==  pS->m_Period )
		{
			pNewData = (CEBBase*)Current->GetElem() ;
			break ;
		}
	}

	// set skybox
	if ( !pNewData )
	{
		// if no skybox has been found, take first
		pNewData = (CEBBase*)CEBOScene::mSkyList->GetFirst()->GetElem() ;
	}
	pDoc->ChangeParamFromUserId( pScene, CEBOScene::_SKY_ , pNewData, sizeof(pNewData) ) ;

	// UPDATE LOCATION
	// set location
	// ...

	return pScene ;

}
*/

//////////////////////////////////////////////////////////////////////
/*
bool CFDparser::IsSceneIdExist(CEBDoc* pDoc, CString string )
{
	if ( !pDoc )
		return NULL ;

	if ( string == "" )
		return false ;

	CString numScene = GetSceneId(string) ;
	for(CEBCell* Current = pDoc->mScenesList->GetFirst(); Current;
													Current = Current->GetNext() )
	{
		CEBOScene* pScene = (CEBOScene*)Current->GetElem() ;
		CString title = pScene->GetTitle() ;
		CString currId = GetSceneId( title ) ;
		if ( currId == numScene )
			return true ;
	}
	return false ;
}
*/

//////////////////////////////////////////////////////////////////////
/*
CEBOScene* CFDparser::IsSceneExist(CEBDoc* pDoc, CString name)
{
	if ( !pDoc )
		return NULL ;

	name.MakeUpper() ;
	for(CEBCell* Current = pDoc->mScenesList->GetFirst(); Current;
													Current = Current->GetNext() )
	{
		CEBOScene* pScene = (CEBOScene*)Current->GetElem() ;
		CString currentName = pScene->GetTitle() ;
		currentName.MakeUpper() ;
		if ( currentName == name )
			return pScene ;
	}
	return NULL ;
}
*/

//////////////////////////////////////////////////////////////////////
/// parse scenes
void CFDparser::_Parse(LPCTSTR szName, CFDscene* ptr, int numScene)
{
	// find situation & period
	CString	name = szName ; 

	name.TrimLeft() ;
	name.TrimRight() ;

	ptr->m_Title = name ;

	// enumerate scene
	if ( numScene>=0 )
		SetSceneNum( ptr->m_Title, numScene) ;

	// store situation & period
	name.MakeUpper() ;

	ptr->m_Situation = Find( SC_DICOENTRY_SITUATIONS, name, ptr->m_SitCpStart, ptr->m_SitCpEnd  ) ;
	ptr->m_Period = Find( SC_DICOENTRY_PERIODS, name, ptr->m_PerCpStart, ptr->m_PerCpEnd  ) ;

	// rectify PerCps from SitCps
	if ( ptr->m_PerCpStart!=-1 && ptr->m_SitCpStart != -1 )
	{
		ptr->m_PerCpStart += ptr->m_SitCpEnd - ptr->m_SitCpStart ;
		ptr->m_PerCpEnd += ptr->m_SitCpEnd - ptr->m_SitCpStart ;
	}

	// store name
	ptr->Crop( name ) ;

	ptr->m_Name = ( name!="" ? name : _T(SC_SCENE_NOTITLE) ) ;
}

//////////////////////////////////////////////////////////////////////
/*
CEBOChar* CFDparser::AnalyseActor( CEBDoc* pDoc, CEBOScene* pScene, CString string)
{
	if ( !pScene )
		return NULL ;

	if ( !pDoc )
		return NULL ;

	if ( string=="" )
		return NULL ;

	// check if actor is in scene
	CEBOChar* pActor = IsActorInScene( pDoc, pScene, string ) ;

	if ( pActor )
		return pActor ;

	// create a parser model and parse string
	CFDactor actor ;
	actor.m_Name = string ;
	_Parse(string, &actor) ;

	// check if actor exist in casting
	CEBIdCard* pIdCard = _FindIdCard( pDoc, &actor) ;

	if ( !pIdCard )
	{
		// create IdCard
		pIdCard = (CEBIdCard*)pDoc->CreateObject(CEBIdCard::CreateInstanceRB, NULL ) ;

		// set name
		pDoc->ChangeObjectName( (CEBBase*)pIdCard, (LPCTSTR)string ) ;

		// set gender
		if ( actor.m_Sexe == SC_GENDER_FEMALE )
			pIdCard->mFemale = true ;
		else
		if ( actor.m_Sexe == SC_GENDER_MALE )
			pIdCard->mMale = true ;

		// then go directly to create actor if we have created a new IdCard...
	}

	// create scene actor (actor is not in scene)
	if ( pIdCard )
	{
		// create actor in pScene
		pActor = (CEBOChar*)pDoc->CreateObject(CEBOChar::CreateInstanceRB, pScene ) ;

		if ( pActor )
		{
			// connect to new idcard first
			pActor->SetId( pIdCard );

			// Set name
			pDoc->ChangeObjectName( (CEBBase*)pActor, (LPCTSTR)string ) ;

			// set geometry
			pIdCard->UpdateDefaultGeom();
			pIdCard->UpdateDefaultVoice();

			// set position
			float x = SC_ACTORSTAGE_RANGE - 2.f * (SC_ACTORSTAGE_RANGE*rand()/(RAND_MAX- 1)) ;		// -SC_ACTORSTAGE_RANGE<x<SC_ACTORSTAGE_RANGE
			float z = SC_ACTORSTAGE_RANGE - 2.f * (SC_ACTORSTAGE_RANGE*rand()/(RAND_MAX- 1)) ;		// -SC_ACTORSTAGE_RANGE<y<SC_ACTORSTAGE_RANGE
			float y = 0.f ;
			pDoc->SetObjectPos((CEBO3D*)pActor, x , y, z) ;

		}
	}

	return pActor ;
}
*/

//////////////////////////////////////////////////////////////////////
/// parse actors
/*
CEBOChar* CFDparser::IsActorInScene(CEBDoc* pDoc, CEBOScene* pScene, CString name )
{
	if ( !pDoc )
		return NULL ;

	if ( !pScene )
		return NULL ;

	for(CEBCell* Current = pScene->mCharacters->GetFirst(); Current;
													Current = Current->GetNext() )
	{
		CEBOChar* pActor = (CEBOChar*)Current->GetElem() ;

		if ( !stricmp( (LPCTSTR)name, pActor->GetName() ) )
			return pActor ;
	}

	return NULL ;
}
*/

//////////////////////////////////////////////////////////////////////
/// parse actors
void CFDparser::_Parse(LPCTSTR szName, CFDactor* ptr )
{
	CString name = szName ;

	unsigned pos = LEXFindPos( name ) ;

	// chercher dans les noms
	ptr->m_Sexe = ( ISNNPM(pos) ? _T(SC_GENDER_MALE) : ( ISNNPF(pos) ? _T(SC_GENDER_FEMALE) : "" ) ) ;

	// sinon chercher dans les métiers 
	if ( ptr->m_Sexe == "" )
	{
		ptr->m_Sexe = ( ISNNJM(pos) ? _T(SC_GENDER_MALE) : ( ISNNJF(pos) ? _T(SC_GENDER_FEMALE) : "" ) ) ;
	}

	// coloriser si reconnu
	if ( ptr->m_Sexe != "" )
	{
		ptr->m_CpStart = 0 ;
		ptr->m_CpEnd = ptr->m_CpStart + name.GetLength() ;
	}
	else
	{
		ptr->m_CpStart = ptr->m_CpEnd = -1 ;
	}

}

//////////////////////////////////////////////////////////////////////
/*
void CFDparser::_CreateDocIdCards( CEBDoc* pDoc )
{
	// Import actors - create IdCard
	CFDactor* pA = GetActor() ;
	while( pA )
	{
//		if ( pA->m_Sexe == SC_GENDER_FEMALE ||	pA->m_Sexe == SC_GENDER_MALE )
//		{
			// create IdCard
			CEBIdCard* pIdCard = (CEBIdCard*)pDoc->CreateObject(CEBIdCard::CreateInstanceRB, NULL ) ;

			// set IdCard parameters
			pDoc->ChangeObjectName( (CEBBase*)pIdCard, (LPCTSTR)pA->m_Name ) ;

			if ( pA->m_Sexe == SC_GENDER_FEMALE )
				pIdCard->mFemale = true ;
			else
			if ( pA->m_Sexe == SC_GENDER_MALE )
				pIdCard->mMale = true ;
//		}

		// next actor
		pA = (CFDactor*)pA->m_pNext ;
	}
}
*/

//////////////////////////////////////////////////////////////////////
/*
void CFDparser::_CreateDocActors( CEBDoc* pDoc, CFDscene* pS, CEBOScene* pScene )
{
	// placer les acteurs sur une grille SC_ACTORSTAGE_RANGExSC_ACTORSTAGE_RANGE
	CFDactor* position[SC_ACTORSTAGE_RANGE][SC_ACTORSTAGE_RANGE] ;

	// count = sécurité pour la position des acteurs
	int count = 0 ;

	// set actors' scene
	// clear positions
	for (register int j=0;j<SC_ACTORSTAGE_RANGE;j++)
		for (register int i=0;i<SC_ACTORSTAGE_RANGE;i++)
			position[i][j] = NULL ;

	// get current actor
	CFDactor* pA = GetActor() ;
	while( pA )
	{
		SFDdialog_group* pD = pA->m_Dialog ;
		while( pD )
		{
			if ( pS == pD->pScene )
			{
				CEBOChar* pActor = (CEBOChar*)pDoc->CreateObject(CEBOChar::CreateInstanceRB, pScene ) ;

				// set IdCard
				CEBIdCard* pIdCard = _FindIdCard( pDoc, pA ) ;

				// règle importante : si je n'ai pas trouvé d'IdCard
				// (nom ou sexe diff.) alors je ne créé pas l'acteur...
				if ( pIdCard ) 
				{
					pActor->SetId( pIdCard );

					// Set name
					pDoc->ChangeObjectName( (CEBBase*)pActor, (LPCTSTR)pA->m_Name ) ;

					// set geometry
					pIdCard->UpdateDefaultGeom();
					pIdCard->UpdateDefaultVoice();

					// set position
					// find a free position
					int i,j ;
					do
					{
						i = (SC_ACTORSTAGE_RANGE*rand()/(RAND_MAX- 1));
						j = (SC_ACTORSTAGE_RANGE*rand()/(RAND_MAX- 1));
					} while( position[i][j]!= NULL && count<SC_ACTORSTAGE_RANGE2 ) ;

					// freeze position 
					position[i][j] = pA ;
					count++ ;

					// set position
					float x = SC_ACTORSTAGE_RANGE - 2.f * i ;		// -SC_ACTORSTAGE_RANGE<x<SC_ACTORSTAGE_RANGE
					float z = SC_ACTORSTAGE_RANGE - 2.f * j ;		// -SC_ACTORSTAGE_RANGE<y<SC_ACTORSTAGE_RANGE
					float y = 0.f ;
					pDoc->SetObjectPos((CEBO3D*)pActor, x , y, z) ;
				}
				// si ce n'est pas un acteur le transformer en accessoire
				else
				{
					// détruire Actor
					pDoc->DeleteObject( (CEBBase*)pActor, pScene ) ;

					// créer un accessoire
					// ...
				}

				// parse replies
				SFDreply* pR = pD->pReply ;
				while( pR )
				{
					CEBDialogLine* pDialog = (CEBDialogLine*)pDoc->CreateDialogLine( pActor, pScene , (char*)(LPCTSTR)pR->rep ) ;
					ASSERT(pR->pDialogLine == NULL);
					pR->pDialogLine = pDialog ;
					pR=pR->pNext ;
				};

			}
			pD = pD->pNext ;
		}
		pA = (CFDactor*)pA->m_pNext ;
	}
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// NUMEROTATION
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
/// compute the id between two id's
void CFDparser::FindInBetweenId(CString string1, CString string2, CString& string )
{
	// build string1 id table
	CString N1[SC_MAX_NUM_CHUNK] ;
	int n1 = 0 ;
	int start1 = 0 ;
	int end1 = 0 ;
	while( start1>=0)
	{
		end1 = string1.Find(SC_NUMNAME_SEPARATOR_CHUNK,start1 ) ;
		if ( end1<0 )
		{
			N1[n1++] = string1.Mid(start1) ;
			start1 = -1 ;
		}
		else
		{
			N1[n1++] = string1.Mid( start1, end1 - start1 ) ;
			start1 = end1 + 1 ;
		}
	}
	// build string2 id table
	CString N2[SC_MAX_NUM_CHUNK] ;
	int n2 = 0 ;
	int start2 = 0 ;
	int end2 = 0 ;
	while( start2>=0)
	{
		end2 = string2.Find(SC_NUMNAME_SEPARATOR_CHUNK,start2 ) ;
		if ( end2<0 )
		{
			N2[n2++] = string2.Mid(start2) ;
			start2 = -1 ;
		}
		else
		{
			N2[n2++] = string2.Mid( start2, end2 - start2 ) ;
			start2 = end2 + 1 ;			
		}
	}
	// build intermediate (string) id
	string = "" ;
	for( register int i=0;i<n1;i++)
	{
		if ( N1[i]==N2[i])
		{
			string = string + SC_NUMNAME_SEPARATOR_CHUNK + N1[i] ;
		}
		else
		{
			CString M = N1[i] ;

			_IncreaseId(M) ;

			if ( M != N2[i] )
			{
				string = string + SC_NUMNAME_SEPARATOR_CHUNK + M ;
				break ;
			}
			else
			{
				if ( n1 == n2 )
				{
					if ( isalpha(*(LPCTSTR)N1[i].Left(1)) )
					{
						string = string + SC_NUMNAME_SEPARATOR_CHUNK + N1[i] + SC_NUMNAME_SEPARATOR_CHUNK + "1" ;
						break ;
					}
					else
					{
						string = string + SC_NUMNAME_SEPARATOR_CHUNK + N1[i] + SC_NUMNAME_SEPARATOR_CHUNK + "A" ;
						break ;
					}
				}
				else
				{
					_IncreaseId( N1[n1-1] ) ;

					for( register int j=i;j<n1;j++)
					{
						string = string + SC_NUMNAME_SEPARATOR_CHUNK + N1[j] ;
					}
					break ;
				}
			}
		}
	}
	// clean extra char.
	if ( string.Left(1) == SC_NUMNAME_SEPARATOR_CHUNK )
		string = string.Mid(1) ;
}

////////////////////////////////////////////////////////////////
/// increase string at pos
void CFDparser::IncreaseId(CString& string, int pos  )
{
	CString Id = _GetChunkId( string, pos ) ;
	if ( Id=="") 
		return ;
	int start = string.Find(Id) ;
	int end = start + Id.GetLength() ; 
	_IncreaseId(Id) ;
	CString left, right ;
	left = string.Left(start) ;
	if ( end>0)
		right = string.Mid(end) ;
	string = left + Id + right ;
}

////////////////////////////////////////////////////////////////
/// build scene + Id string
void CFDparser::SetSceneId(CString& string, CString Id )
{
	string = SC_ANTENUM_STRING + Id + SC_NUMNAME_SEPARATOR + string ;
}

////////////////////////////////////////////////////////////////
/// build scene + Id from number
void CFDparser::SetSceneNum(CString& string, int count )
{
	char buffer[5] ;
	itoa(count,buffer,10) ;
	CString Id = buffer ;
	string = SC_ANTENUM_STRING + Id + SC_NUMNAME_SEPARATOR + string ;
}

////////////////////////////////////////////////////////////////
/// Get scene Id
CString CFDparser::GetSceneId(CString string)
{
	int start = string.Find(SC_ANTENUM_STRING_SHORT) ;

	if ( start == -1 )
		return "" ;

	start += strlen(SC_ANTENUM_STRING_SHORT);

	int end = string.Find(SC_NUMNAME_SEPARATOR_SHORT) ;

	if ( end == -1 )
		return "" ;

	CString Id = string.Mid( start, end - start) ; 

	Id.TrimLeft() ;
	Id.TrimRight() ;

	return Id ;
}

////////////////////////////////////////////////////////////////
/// Get scene Id shift
int CFDparser::GetSceneIdShift(CString string)
{
	int start = string.Find(SC_NUMNAME_SEPARATOR) ;

	if ( start == -1 )
	{
		start = string.Find(SC_NUMNAME_SEPARATOR_SHORT) ;

		if ( start != -1 )
			return ( start + strlen(SC_NUMNAME_SEPARATOR_SHORT) ) ;

	}
	else
		return ( start + strlen(SC_NUMNAME_SEPARATOR) ) ;

	return 0 ;
}

////////////////////////////////////////////////////////////////
/// delete scene Id
void CFDparser::DeleteSceneId(CString& string)
{
	int start = string.Find(SC_ANTENUM_STRING_SHORT) ;
	if ( start == -1 )
		return ;
	start = string.Find(SC_NUMNAME_SEPARATOR_SHORT) ;
	if ( start == -1 )
		return ;
	string = string.Mid(start + 1) ; 
	string.TrimLeft() ;
	string.TrimRight() ;
}

////////////////////////////////////////////////////////////////
/// get id chunk at pos
CString CFDparser::_GetChunkId(CString string, int pos )
{
	int start = 0 ;
	while( pos && start>=0 )
	{
		start = string.Find(SC_NUMNAME_SEPARATOR_CHUNK, start + 1) ;
		pos-- ;
	};
	if ( start<0 ) 
		return "" ;
	if ( string.Mid(start,1) == SC_NUMNAME_SEPARATOR_CHUNK )
		start++ ;
	int end = string.Find(SC_NUMNAME_SEPARATOR_CHUNK, start) ;
	return ( end<0 ? string.Mid(start) : string.Mid( start, end - start) ) ;
}

////////////////////////////////////////////////////////////////
/// increase string
void CFDparser::_IncreaseId(CString& string )
{
	if ( isalpha(*(LPCTSTR)string.Left(1)) )
	{
		char c = *(LPCTSTR)string.Left(1) ;
		c++ ;
		string = c ;
	}
	else
	{
		int num = atoi(string) + 1 ;
		char buffer[SC_MAX_NUM_BUFFER] ;
		itoa(num, buffer, 10 ) ;
		string = buffer ;
	}
}

////////////////////////////////////////////////////////////////
/// compare two Ids
/// return	0	if Id1 = Id2 
///			<0	if Id1 < Id2
///			>0  if Id1 > Id2 
int	 CFDparser::_CompareId( CString Id1, CString Id2)
{
	int pos = 0 ;
	for(;;)
	{
		CString c1 = _GetChunkId(Id1, pos ) ;
		CString c2 = _GetChunkId(Id2, pos ) ;

		if ( c1=="" && c2!="" )	// Id1<Id2
			return -1 ;
		else
		if ( c1!="" && c2=="" )	// Id1>Id2
			return 1 ;
		else
		if ( c1=="" && c2=="" )	// Id1=Id2
			return 0 ;
		else
		if ( c1!="" && c2!="" )
		{
			if ( isalpha(*(LPCTSTR)c1.Left(1)) && isalpha(*(LPCTSTR)c2.Left(1)))
			{
				if ( c1<c2 )		// Id1<Id2
					return -1 ;
				else
				if ( c1>c2 )		// Id1>Id2
					return 1;
			}
			else
			{
				int x = atoi(c1) ;
				int y = atoi(c2) ;
				if ( x<y )		// Id1<Id2
					return -1 ;
				else
				if ( x>y )		// Id1>Id2
					return 1;
			}
		}

		pos++;
	}
	return 0 ;
}
