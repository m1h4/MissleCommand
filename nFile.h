// Copyright 2004/2006 Miha Software

#pragma once

#include "nString.h"

class nFile
{
public:
	nFile(void);
	~nFile(void);

	enum OpenFlag
	{
		OpenFlagShareRead = 0x00000001,
		OpenFlagShareWrite = 0x00000002,
		OpenFlagRead = 0x00000004,
		OpenFlagWrite = 0x00000008,
		OpenFlagExisting = 0x00000010,
		OpenFlagCreate = 0x00000020,
	};

	enum SeekFrom
	{
		SeekFromStart = 0x00000001,
		SeekFromCurrent = 0x00000002,
		SeekFromEnd = 0x00000004,
	};

	bool Open(const nString& path,unsigned long flags);
	bool IsOpened() { return m_Handle ? true : false; }
	void Close();

	unsigned long Write(const void* buffer,unsigned long size);
	unsigned long Write(unsigned long value);
	unsigned long Write(long value);
	unsigned long Write(float value);
	unsigned long Write(double value);
	unsigned long Write(bool value);
	unsigned long Write(const nString& value);
	
	unsigned long Read(void* buffer,unsigned long size);
	unsigned long Read(unsigned long* value);
	unsigned long Read(long* value);
	unsigned long Read(float* value);
	unsigned long Read(double* value);
	unsigned long Read(bool* value);
	unsigned long Read(nString* value);
	
	bool Flush();

	unsigned long GetSize();
	unsigned long Seek(unsigned long position,SeekFrom from);
	unsigned long Tell();

protected:
	HANDLE		m_Handle;
};
