#pragma once

struct nCommandLineVar
{
	nString	name;
	nString	data;
};

class nCommandLine
{
public:
	nCommandLine(void);
	~nCommandLine(void);

	void Set(const char* cmdLine);
	void Clear();

	const nString& GetVar(const char* var,const nString& defaul) const;

protected:
	nArray<nCommandLineVar>		m_Vars;
};
