// TestText2Scene.cpp : définit le point d'entrée pour l'application console.
//
// Memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
#include "stdafx.h"
#include "Text2Scene.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
using namespace Text2Scene;

int _tmain(int argc, _TCHAR* argv[])
{
	// Memory leak detection
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	/// Unit test
//	string *str = new string("..\\Scripts\\Sorti pour la Vie.rtf"); // french formatted - small
	string *str = new string("..\\Scripts\\A Family Game.rtf"); // english formatted - small
	cout << *str << endl;
	CText2Scene* t2s = new CText2Scene();
	if (t2s->Parse( str->c_str() ))
	{
		cout << "Success!!" << endl <<endl;
		// Display scenes and actors
		CStage* pStage = t2s->GetStage();
		TScene::iterator p;
		TActor::iterator q;
		for(p = pStage->m_Scenes.begin(); p != pStage->m_Scenes.end(); ++p)
		{
			printf("Title: %s\n", (*p)->m_Title.c_str());
			printf(" Location = %i, Time = %i\n", (*p)->m_Location, (*p)->m_Time);
			if (!(*p)->m_Actors.empty())
			{
				printf(" Actors:\n");
				for(q = (*p)->m_Actors.begin(); q != (*p)->m_Actors.end(); ++q)
				{
					printf("  %s, Gender = %i\n", (*q)->m_Name.c_str(), (*q)->m_Gender);
				}
			}
			cout << endl;
		}
/*
		// Display dialogs
		TDialog::iterator r;
		for(q = pStage->m_Actors.begin(); q != pStage->m_Actors.end(); ++q)
		{
			printf("DIALOGS:\n");
			printf(" %s:\n", (*q)->m_Name.c_str());
			for(r = (*q)->m_Dialogs.begin(); r != (*q)->m_Dialogs.end(); ++r)
			{
				printf("  %s %s - in [%s] \n", (*r)->m_Cue.c_str(), (*r)->m_Parenthesis.c_str(), (*r)->m_pScene->m_Title.c_str());
			}
		}
		// Display screenplay
		TScreenplay::iterator sp;
		TScreenplay screenplay = t2s->GetSceenplay();
		for(sp = screenplay.begin(); sp != screenplay.end(); ++sp)
		{
			printf("%s", (*sp)->second.c_str());
		}
*/
	}
	delete t2s;
	delete str;
	// Memory leak detection
	_CrtDumpMemoryLeaks();
	return 0;
}
