/*******************************************************************
 *         Advanced 3D Game Programming with DirectX 10.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	
 *	See license.txt for modification and distribution information
 *		copyright (c) 2007 by Peter Walsh, Wordware
 ******************************************************************/

#include "File.h"

using namespace std;

static TCHAR _n = '\r';

cFile::cFile() :
	m_fp( NULL )
{
	// nothing to do
}

cFile::~cFile()
{
	// double check to make sure the user closed the file.
	if( m_fp != NULL )
	{
		Close();
	}
}

void cFile::Create(const TCHAR* filename)
{
	// Create the file handle
	m_fp = _wfopen( filename, L"w+" );

	// make sure everything went well
	if( m_fp == NULL )
	{
		throw cFileCannotCreate( filename );
	}

	m_filename = wstring( filename );
}

void cFile::Append( const TCHAR* filename )
{
	// Create the file handle
	m_fp = _wfopen( filename, L"ab" );

	// make sure everything went well
	if( m_fp == NULL )
	{
		throw cFileCannotCreate( filename );
	}

	m_filename = wstring( filename );
}

void cFile::Open( const TCHAR* filename )
{
	// Create the file handle
	m_fp = _wfopen( filename, L"r+b" );

	// make sure everything went well
	if( m_fp == NULL )
	{
		throw cFileCannotCreate( filename );
	}

	m_filename = wstring( filename );

}


void cFile::Close()
{
	if( m_fp == NULL )
		return; // the file is already closed

	if( 0 != fclose( m_fp ) )
	{
		DP(L"Error in cFile::Close\n");
	}

	m_fp = NULL;
}



bool cFile::Exists( const TCHAR* filename )
{
	// try to open the file.  if we can't, it must not exist
	cFile temp;

	temp.m_fp = _wfopen( filename, L"r" );

	// make sure everything went well
	if( temp.m_fp == NULL )
	{
		return false;
	}
	temp.Close();
	return true;
}


void cFile::WriteBuff( void *pBuffer, int buffSize )
{
	if( m_fp == NULL )
		return; // error

	int numWritten = fwrite(pBuffer,buffSize, 1, m_fp);

	if (numWritten == 0)
	{
		throw cFileWriteError();
	}
}

void cFile::Write_n()
{
	if (m_fp == NULL)
		return;
	int numWritten = fwrite(&_n,sizeof(TCHAR),1, m_fp);

	if (numWritten == 0)
	{
		throw cFileWriteError();
	}
}


/**
 * Not overrun safe
 */

