#pragma once
#include "pugixml\pugixml.hpp"
#include <string>
using namespace std;
using namespace pugi;

namespace Text2Scene
{
	class CXmlReader
	{
	public:
		CXmlReader(void) {};
		virtual ~CXmlReader(void) {};
		bool Read(const char* filename);
		void ParseId(void* ptr, const xml_node tokens, void (*callback)(void* ptr, int value, string text));
		xml_document& Doc() { return m_Doc; };

	protected:
		 xml_document m_Doc;
	};
}
