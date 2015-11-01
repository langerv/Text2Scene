#pragma once
#include <string>
#include <vector>
#include "Utils.h"
using namespace std;

namespace Text2Scene
{
	enum Description
	{
		Unformated = 0,
		Heading = 1,
		Action,
		Character,
		Parenthesis,
		Dialog,
		Transition,
		Animation
	};

	enum Numbering
	{
		Scene = 1
	};

	enum Location
	{
		Unknown = 0,
		Int = 1,
		Ext,
		IntExt,
		ExtInt
	};

	enum Time
	{
		Undefined = 0,
		Day = 1,
		Morning,
		Afternoon,
		Evening,
		Dusk,
		Night,
		Later,
		MomentLater,
		Space
	};

	enum Gender
	{
		None = 0,
		Male = 1,
		Female
	};

	class CScene;
	class CActor;
	class CDialog;

	typedef vector<pair<Description, string>*> TScreenplay;
	typedef vector<CScene*> TScene;
	typedef vector<CActor*> TActor;
	typedef vector<CDialog*> TDialog;

	class CDialog
	{
	public:
		CDialog() : m_pScene(NULL), m_Cue(""), m_Parenthesis("") {};
		virtual ~CDialog() {};

		CScene* m_pScene;
		string m_Cue;
		string m_Parenthesis;
	};

	class CActor
	{
	public:
		CActor() : m_Name(""), m_Gender(Gender::None) {};
		virtual ~CActor()
		{
			DeleteVector(&m_Dialogs);
		};

		string m_Name;
		Gender m_Gender;
		TDialog m_Dialogs;
	};

	class CScene
	{
	public:
		CScene() : m_Title(""), m_Location(Location::Unknown), m_Time(Time::Undefined) {};
		virtual ~CScene() {};

		string		m_Title;
		Location	m_Location;
		Time		m_Time;
		TActor		m_Actors;
	};

	class CStage
	{
	public:
		CStage() {};
		virtual ~CStage() {};

	public:
		TScene m_Scenes;
		TActor m_Actors;
	};

}