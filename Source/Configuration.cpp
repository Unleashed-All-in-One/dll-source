#include "Configuration.h"

#define INI_FILE "UnleashedAllInOne.ini"

//---------------Gameplay---------------
bool Configuration::m_bQSS = true;

void Configuration::load()
{
	INIReader reader(INI_FILE);
	if (reader.ParseError() != 0)
	{
		MessageBox(NULL, L"Failed to parse mod configuration", NULL, MB_ICONERROR);
		exit(-1);
		return;
	}

	//---------------Gameplay---------------
	m_bQSS = reader.GetBoolean("Gameplay", "bQSS", m_bQSS);
}
