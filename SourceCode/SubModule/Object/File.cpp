/*******************************************************************
 *         Advanced 3D Game Programming with DirectX 10.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	
 *	See license.txt for modification and distribution information
 *		copyright (c) 2007 by Peter Walsh, Wordware
 ******************************************************************/

#include "File.h"

using namespace std;

const static TCHAR _n = '\r';

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

void cFile::Create( const TCHAR* filename )
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


void cFile::ReadBuff( void* pBuffer, int buffSize )
{
	if( m_fp == NULL )
		return; // error

	//int numRead = fread( pBuffer, buffSize, 1, m_fp );
	int numRead = fread( pBuffer, sizeof(TCHAR), buffSize, m_fp );

	if( numRead != buffSize )
	{
		if( 0 != feof( m_fp ) )
		{
			throw cFileEOF();
		}
		else if( ferror( m_fp ) )
		{
			throw cFileReadError();
		}
		else
		{
			int foo = 0;
		}
	}
}


void cFile::WriteBuff( void* pBuffer, int buffSize )
{
	if( m_fp == NULL )
		return; // error

	int numWritten = fwrite( pBuffer, buffSize, 1, m_fp );

	if (numWritten == 0)
	{
		throw cFileWriteError();
	}
}


/**
 * Not overrun safe
 */
void cFile::ReadLine( TCHAR* pBuffer )
{
	TCHAR currChar;
	bool done = false;

	int nRead = 0;
	while( !done )
	{
		try
		{
			ReadBuff( &currChar, 1 );
			nRead++;
		}
		catch( cFileEOF )
		{
			// break from the loop, we reached the end-of-file.
			if( nRead == 0 )
			{
				/**
				 * We started at the EOF and can't read a single line.
				 */
				throw;
			}
			break;
		}

		
		if( currChar == '\0' || currChar == '\n' || currChar == '\r')
		{
			break;
		}
		*pBuffer++ = currChar;
	}
	// end the string with a \n\0
	*pBuffer++ = '\n';
	*pBuffer++ = '\0';
}

/**
 * Overrun safe
 */
wstring cFile::ReadLine()
{
	TCHAR currChar;
	bool done = false;

	int nRead = 0;

	wstring out;

	while( !done )
	{
		try
		{
			ReadBuff( &currChar, 1 );
			nRead++;
		}
		catch( cFileEOF )
		{
			// break from the loop, we reached the end-of-file.
			if( nRead == 0 )
			{
				/**
				 * We started at the EOF and can't read a single line.
				 */
				throw;
			}
			break;
		}

		if (currChar == '\0' || currChar == '\n' || currChar == '\r')
		{
			break;
		}

		out += currChar;
	}
	return out;
}


void cFile::ReadNonCommentedLine( TCHAR* pBuffer, TCHAR commentChar )
{
	TCHAR buff[1024];
	buff[0] = 0;
	while( 1 )
	{
		ReadLine( buff );
		if( buff[0] != commentChar )
			break;
	}
	wcscpy_s( pBuffer, 1024, buff );
}


void cFile::TokenizeNextNCLine( queue< wstring >* pList, TCHAR commentChar )
{
	wstring str;
	while(1)
	{
		str = ReadLine();
		if( str[0] != commentChar && str[1] != commentChar)
			break;
	}

	// now curr has our string.
	// first, strip off any comments on the end.
	//DP1(L"parsing [%s]\n", str.c_str() );
	unsigned int commentLoc = str.find( commentChar, 0);
	if( commentLoc != str.npos )
	{
		str = str.erase( commentLoc );
	}

	TCHAR sep[] = L" \t\n\r";

	unsigned int tokStart = str.find_first_not_of( (TCHAR*)sep, 0 );
	unsigned int tokEnd;
	while( tokStart != str.npos )
	{
		tokEnd = str.find_first_of( (TCHAR*)sep, tokStart );
		wstring token = str.substr( tokStart, tokEnd-tokStart );
		pList->push( token );

		tokStart = str.find_first_not_of( (TCHAR*)sep, tokEnd );
	}
}

void cFile::Write_n()
{
	if (m_fp == NULL)
		return;
	int numWritten = fwrite(&_n, sizeof(TCHAR), 1, m_fp);

	if (numWritten == 0)
	{
		throw cFileWriteError();
	}
}