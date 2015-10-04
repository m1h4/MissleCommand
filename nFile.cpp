// Copyright 2004/2006 Miha Software

#include "nGlobals.h"
#include "nFile.h"

nFile::nFile(void) :
m_Handle(NULL)
{
}

nFile::~nFile(void)
{
	if(this->IsOpened())
		this->Close();
}

bool nFile::Open(const nString& path,unsigned long flags)
{
	nAssert(!m_Handle);

	unsigned long access = 0;
	unsigned long share = 0;
	unsigned long create = 0;

	if(flags & OpenFlagShareRead)
		share |= FILE_SHARE_READ;
	if(flags & OpenFlagShareWrite)
		share |= FILE_SHARE_WRITE;
	if(flags & OpenFlagCreate)
		create |= CREATE_ALWAYS;
	if(flags & OpenFlagExisting)
		create |= OPEN_EXISTING;
	if(flags & OpenFlagRead)
		access |= GENERIC_READ;
	if(flags & OpenFlagWrite)
		access |= GENERIC_WRITE;

	m_Handle = CreateFile(path.c_str(),access,share,NULL,create,NULL,NULL);
	if(m_Handle == INVALID_HANDLE_VALUE)
	{
		m_Handle = NULL;

		return false;
	}

	return true;
}

void nFile::Close()
{
	nAssert(m_Handle);

	CloseHandle(m_Handle);
	m_Handle = NULL;
}

unsigned long nFile::Write(const void* buffer,unsigned long size)
{
	nAssert(m_Handle);

	unsigned long written = 0;

	WriteFile(m_Handle,buffer,size,&written,NULL);

	return written;
}

unsigned long nFile::Write(unsigned long value)
{
	return Write(&value,sizeof(value));
}

unsigned long nFile::Write(long value)
{
	return Write(&value,sizeof(value));
}

unsigned long nFile::Write(float value)
{
	return Write(&value,sizeof(value));
}

unsigned long nFile::Write(double value)
{
	return Write(&value,sizeof(value));
}

unsigned long nFile::Write(bool value)
{
	return Write(&value,sizeof(value));
}

unsigned long nFile::Write(const nString& value)
{
	return	Write((unsigned long)value.size()) + 
			Write(value.c_str(),sizeof(nString::value_type) * value.size());
}

unsigned long nFile::Read(void* buffer,unsigned long size)
{
	nAssert(m_Handle);

	unsigned long read = 0;

	ReadFile(m_Handle,buffer,size,&read,NULL);

	return read;
}

unsigned long nFile::Read(unsigned long* value)
{
	return Read(value,sizeof(unsigned long));
}

unsigned long nFile::Read(long* value)
{
	return Read(value,sizeof(long));
}

unsigned long nFile::Read(float* value)
{
	return Read(value,sizeof(float));
}

unsigned long nFile::Read(double* value)
{
	return Read(value,sizeof(double));
}

unsigned long nFile::Read(bool* value)
{
	return Read(value,sizeof(bool));
}

unsigned long nFile::Read(nString* value)
{
	unsigned long size = 0;
	char* buffer = NULL;

	unsigned long read = Read(&size);

	if(!size)
		return read;

	buffer = nMalloc(char[size + 1]);
	nAssert(buffer);

	read += Read(buffer,size);
	
	buffer[size] = NULL;

	*value = buffer;

	nSafeFree(buffer);

	return read;
}

bool nFile::Flush()
{
	nAssert(m_Handle);

	return FlushFileBuffers(m_Handle) ? true : false;
}

unsigned long nFile::GetSize()
{
	nAssert(m_Handle);

	return GetFileSize(m_Handle,NULL);	// TODO Won't work correctly with 64-bit file sizes
}

unsigned long nFile::Seek(unsigned long position,SeekFrom from)
{
	nAssert(m_Handle);

	unsigned long fr = 0;

	if(from == SeekFromStart)
		fr = FILE_BEGIN;
	else if(from == SeekFromCurrent)
		fr = FILE_CURRENT;
	else if(from == SeekFromEnd)
		fr = FILE_END;

	return SetFilePointer(m_Handle,position,NULL,fr);
}

unsigned long nFile::Tell()
{
	nAssert(m_Handle);

	return this->Seek(0,SeekFromCurrent);
}