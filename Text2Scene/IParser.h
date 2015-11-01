#pragma once
#include "Model.h"
using namespace std;

namespace Text2Scene
{
	class IParser
	{
		public:
			virtual ~IParser() {}
			virtual bool Read(const char* filename) = 0;
			virtual TScreenplay& GetScreenplay() = 0;
			virtual Location GetLocation(const char*, string&) = 0;
			virtual Time GetTime(const char*, string&) = 0;
	};
}