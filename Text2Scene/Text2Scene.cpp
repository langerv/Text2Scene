#include "Text2Scene.h"

namespace Text2Scene
{
	CText2Scene::CText2Scene() : 
		m_pParser(new CFDParser()), 
		m_pStage(NULL)
	{
		CLexicon::Init("..\\Text2Scene\\Lexicon.xml");
	}

	CText2Scene::~CText2Scene()
	{
		DeleteStage();
		delete m_pParser;
	}

	void CText2Scene::DeleteStage()
	{
		if (m_pStage == NULL)
		{
			return;
		}
		DeleteVector(&m_pStage->m_Scenes);
		DeleteVector(&m_pStage->m_Actors);
		delete m_pStage;
		m_pStage = NULL;
	}

	bool CText2Scene::Parse(const char* filename)
	{
		if (!m_pParser->Read(filename))
		{
			return false;
		}
		DeleteStage();
		m_pStage = new CStage();
		CScene* pCurScene = NULL ;
		CActor* pCurActor = NULL; 
		string text;
		string parenthesis;
		const string contd("(CONT'D)");
		TScreenplay::iterator p;
		TScreenplay sp = m_pParser->GetScreenplay();
		for(p = sp.begin(); p != sp.end(); ++p)
		{
			text = (*p)->second;
			switch((*p)->first)
			{
			case Description::Action:
				break;
			case Description::Animation:
				break;
			case Description::Character:
				{
					ToUpper(text);
					// TODO:
					// temporary hack for "(CONT'D)"
					size_t found = text.find(contd);
					while (found != string::npos)
					{
						text.erase(found, contd.length());
						found = text.find(contd);
					}
					// end of hack
					Trim(text);

					// Add to stage if it does not exist
					TActor::iterator p;
					bool bExist = false;
					for(p = m_pStage->m_Actors.begin(); p != m_pStage->m_Actors.end(); ++p)
					{
						if ((*p)->m_Name == text)
						{
							pCurActor = *p;
							bExist = true;
							break;
						}
					}
					if (!bExist)
					{
						pCurActor = CreateActor(text.c_str());
						m_pStage->m_Actors.push_back(pCurActor);
					}
					// Add to scene if it does not exist
					bExist = false;
					for(p = pCurScene->m_Actors.begin(); p != pCurScene->m_Actors.end(); ++p)
					{
						if ((*p)->m_Name == text)
						{
							bExist = true;
							break;
						}
					}
					if (!bExist)
					{
						pCurScene->m_Actors.push_back(pCurActor);
					}
				}
				break;
			case Description::Dialog:
				if (pCurActor != NULL && pCurScene != NULL)
				{
					Trim(text);
					CDialog* pDialog = new CDialog();
					pDialog->m_pScene = pCurScene;
					pDialog->m_Cue = text;
					pDialog->m_Parenthesis = parenthesis;
					pCurActor->m_Dialogs.push_back(pDialog);
					parenthesis = "";
				}
				break;
			case Description::Heading:
				pCurScene = CreateScene(text.c_str()) ;
				m_pStage->m_Scenes.push_back(pCurScene);
				break;
			case Description::Parenthesis:
				parenthesis = text;
				Trim(parenthesis);
				break;
			case Description::Transition:
				break;
			default:
				// unformated text
				break;
			}
		}
		return true;
	}

	CScene* CText2Scene::CreateScene(const char* heading)
	{
		CScene *pScene = new CScene();
		string title(heading);
		pScene->m_Location = m_pParser->GetLocation(title.c_str(), title);
		pScene->m_Time = m_pParser->GetTime(title.c_str(), title);
		Trim(title);
		ToUpper(title);
		pScene->m_Title = title;
		return pScene;
	}

	CActor* CText2Scene::CreateActor(const char* name)
	{
		CActor* pActor = new CActor();
		pActor->m_Name = name;
		switch(GetPos(name))
		{
		case Pos::NNPF:
		case Pos::NNJF:
			pActor->m_Gender = Gender::Female;
			break;
		case Pos::NNPM:
		case Pos::NNJM:
			pActor->m_Gender = Gender::Male;
			break;
		default:
			break;
		}
		return pActor;
	}

}