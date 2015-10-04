#include "nGlobals.h"
#include "nCommandLine.h"

nCommandLine::nCommandLine(void)
{
}

nCommandLine::~nCommandLine(void)
{
}

void nCommandLine::Set(const char* cmdLine)
{
	nString cmd = cmdLine;
	
	// Check length
	if(!cmd.size())
		return;

	// Trim leading spaces
	cmd.trimLeft(" ");

	// Trim trailing spaces
	cmd.trimRight(" ");

	// Check if entire string was just spaces
	if(!cmd.size())
		return;

	// Quote monitoring
	bool quote = true;

	// Check for unmached quotes
	for(unsigned long i = 0; i < cmd.size(); i++)
	{
		if(cmd[i] == '"')
			quote = !quote;
	}

	// Check if ok
	if(!quote)
		return;

	// Clear old vars
	Clear();

	quote = false;

	nString varName;
	nString varValue;

	bool value = false;

	// Split the string
	while(cmd.size())
	{
		if(cmd[0] == '"')
		{
			quote = !quote;

			cmd.erase(cmd.begin());
		}
		else if(cmd[0] == ':' && !value)
		{
			value = true;

			cmd.erase(cmd.begin());
		}
		else if(isspace((unsigned char)cmd[0]))
		{
			if(quote)
			{
				if(value)
					varValue += cmd[0];
				else
					varName += cmd[0];
			}
			else if(cmd.size())
			{
				// Add var
				value = false;

				nCommandLineVar var;

				var.name = varName;
				var.data = varValue;

				m_Vars.push_back(var);

				varName.erase();
				varValue.erase();
			}

			cmd.erase(cmd.begin());
		}
		else
		{
			if(value)
				varValue += cmd[0];
			else
				varName += cmd[0];

			cmd.erase(cmd.begin());
		}
	}

	if(varName.size())
	{
		// Add var
		value = false;

		nCommandLineVar var;

		var.name = varName;
		var.data = varValue;

		m_Vars.push_back(var);

		varName.erase();
		varValue.erase();
	}

	for(unsigned long i = 0; i < m_Vars.size(); i++)
		Trace(__FUNCTION__" %d] Var name: '%s' Var value: '%s'\n",i,m_Vars[i].name.c_str(),m_Vars[i].data.c_str());
}

void nCommandLine::Clear()
{
	while(m_Vars.size())
		m_Vars.pop_back();
}

const nString& nCommandLine::GetVar(const char* var,const nString& defaul) const
{
	for(unsigned long i = 0; i < m_Vars.size(); i++)
		if(m_Vars[i].name == var)
			return m_Vars[i].data;

	return defaul;
}