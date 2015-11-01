#pragma once
#include "IParser.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace Text2Scene
{
	typedef map<Description, vector<string>> TDescMap;
	typedef map<Location, vector<string>> TLocMap;
	typedef map<Time, vector<string>> TTimeMap;

	class CFDParser : public IParser
	{
		public:
			CFDParser();
			virtual ~CFDParser();
			virtual bool Read(const char*);
			virtual TScreenplay& GetScreenplay();
			virtual Location GetLocation(const char*, string&);
			virtual Time GetTime(const char*, string&);

		protected:
			Description GetDescription(const char*);
			TScreenplay m_Screenplay;
			TDescMap m_Descriptions;
			TLocMap m_Locations;
			TTimeMap m_Times;

	private:
		// Callbacks
		static void SetDescription(void* ptr, int value, string str)
		{
			CFDParser* self = (CFDParser*) ptr;
			ToUpper(str);
			self->m_Descriptions[(Description)value].push_back(str);
		}

		static void SetLocation(void* ptr, int value, string str)
		{
			CFDParser* self = (CFDParser*) ptr;
			ToUpper(str);
			self->m_Locations[(Location)value].push_back(str);
		}

		static void SetTime(void* ptr, int value, string str)
		{
			CFDParser* self = (CFDParser*) ptr;
			ToUpper(str);
			self->m_Times[(Time)value].push_back(str);
		}

	};
}