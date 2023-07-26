/* ==========================================================================
	CDiskObject

	Author :		Johan Rosengren, Abstrakt Mekanik AB

	Date :			2004-03-12

	Purpose :		CDiskObject encapsulates several high-level file- 
					and directory operations. 	

	Description :	All error handling is managed internally, and the 
					different API-functions return a BOOL to signal success 
					or failure. In case of failure, FALSE returned, the 
					member function GetErrorMessage can be called to get a
					CString with the error message.

	Usage :			The class is used by creating a CDiskObject, and 
					calling the methods of the class. No other setup is 
					necessary. If a CWnd-pointer is submitted to the ctor, 
					CDiskObject will give feedback by calling SetWindowText 
					with, for example, filenames during processing.This 
					means that a CStatic ( or other appropriate control) can 
					be set up to display the file currently copied, for 
					example.
   ========================================================================
	Author :		Johan Rosengren, Abstrakt Mekanik AB

	Date :			2004-04-14

	Purpose :		1. Added const-correctness for inparams.
					2. Replacing "/" with "\" when qualifying file names/
					   directories
					3. Skipping directory qualify when the path starts 
					   with \\. Can't add a drive letter then... The path 
					   is assumed to be fully qualified.
					4. Added pragma to get rid of C4706 assignment warning.
					5. Added RemoveFile for reasons of symmetry.
   ========================================================================*/

#include "pch.h"
#include <tchar.h>
#include "DiskObject.h"

#pragma warning( disable : 4706 )

#define Trigger( a)	if( m_feedbackWindow ) m_feedbackWindow->SetWindowText( a );

////////////////////////////////////////////////////////////////////
// CDiskObject construction/destruction/initialization

CDiskObject::CDiskObject( CWnd* feedbackWindow )
/* ============================================================
	Function :		CDiskObject::CDiskObject
	Description :	constructor
					
	Return :		void
	Parameters :	HWND hwndFeedback	-	HWND to feedback 
											window

   ============================================================*/
{
	m_feedbackWindow = feedbackWindow;
}

CDiskObject::~CDiskObject( )
/* ============================================================
	Function :		CDiskObject::~CDiskObject
	Description :	destructor
					
	Return :		void
	Parameters :	none

   ============================================================*/
{
}

////////////////////////////////////////////////////////////////////
// CDiskObject operations
//

// File operations

BOOL CDiskObject::CopyFiles( const CString& sourceDirectory,const CString& destDirectory )
/* ============================================================
	Function :		CDiskObject::CopyFiles
	Description :	The member copies all files from 
					sourceDirectory to destDirectory. 
					Subdirectories will not be copied.
					
	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will return 
												errors.
	Parameters :	CString sourceDirectory	-	Copy from. If
												empty, current.
					CString destDirectory	-	Copy to. If 
												empty, current.
					
   ============================================================*/
{
	ClearError( );
	CString source( sourceDirectory );
	CString dest( destDirectory );
	QualifyPath( source );
	QualifyPath( dest );

	// First, we enumerate all files
	CStringArray files;
	BOOL result = EnumFilesInDirectory( source, files );
	if( result )
	{
		// Create the destination directory, if necessary
		if( ( result = CreateDirectory( dest ) ) )
		{
			int max = (int)files.GetSize( );
			for( int t = 0 ; t < max ; t++ )
			{
				// Copy the files
				CString file;
				file = files[ t ];
				Trigger( file );
				if( !( result = ::CopyFile( source + file, dest + file, FALSE ) ) )
				{
					// Set error message
					SetSystemErrorMessage( ::GetLastError( ), 
											source + 
											file + 
											_T( " -> " ) + 
											dest + 
											file );
					t = max;
				}
			}
		}
	}

	return result;
}

BOOL CDiskObject::CopyFiles( CStringArray& files,const CString& destDirectory )
/* ============================================================
	Function :		CDiskObject::CopyFiles
	Description :	The function copies the files in the 
					CStringArray files to the directory 
					destDirectory. Existing files will be
					overwritten. The dest will be created if 
					it doesn't already exist.
					
	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will return 
												errors.
	Parameters :	CStringArray& files		-	a string array 
												with the files 
												to copy
					CString destDirectory	-	destination
					
   ============================================================*/
{
	ClearError( );
	CString dest( destDirectory );

	BOOL result = TRUE;
	if( files.GetSize( ) )
	{
		QualifyPath( dest );

		// Create destination, if necessary
		if( ( result = CreateDirectory( dest ) ) )
		{
			int max = (int)files.GetSize( );
			for( int t = 0 ; t < max ; t++ )
			{
				// Loop and copy the files
				CString file;
				file = files[ t ];
				if( file.GetLength( ) )
				{
					Trigger( file );
					QualifyFile( file );

					// Create destination filename
					CString to = dest + GetFileName( file );
					if( !( result = ::CopyFile( file, to, FALSE ) ) )
					{
						// Set error message
						SetSystemErrorMessage( ::GetLastError( ),
												file + 
												_T( " -> " ) + 
												dest + 
												file );
						t = max;
					}
				}
			}
		}
	}
	else
	{
		SetInternalErrorMessage( );
		result =  FALSE;
	}

	return result;
}

BOOL CDiskObject::CopyFile( const CString& sourceFile,const CString& destDirectory )
/* ============================================================
	Function :		CDiskObject::CopyFile
	Description :	Will copy sourceFile to dest. An existing 
					file will be overwritten. The directory 
					will be created if it doesn't exist.
					
	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will contain 
												errors
	Parameters :	CString sourceFile		-	file to copy
					CString destDirectory	-	destination
					
   ============================================================*/
{
	ClearError( );
	CString source( sourceFile );
	CString dest( destDirectory );

	BOOL result = TRUE;
	if( sourceFile.GetLength( ) )
	{
		QualifyFile( source );
		QualifyPath( dest );

		// Creating destDirectory if necessary.
		if( ( result = CreateDirectory( dest ) ) )
		{
			CString filePart = GetFileName( source );
			// Copy the file
			Trigger( filePart );
			if( !( result = ::CopyFile( source, dest + filePart, FALSE ) ) )
				SetSystemErrorMessage( ::GetLastError( ), source );
		}
	}
	else
	{
		SetInternalErrorMessage( );
		result = FALSE;
	}

	return result;
}

BOOL CDiskObject::RemoveFile( const CString& sourceFile )
/* ============================================================
	Function :		CDiskObject::RemoveFile
	Description :	Will remove sourceFile
					
	Return :		BOOL						-	TRUE if 
													removed ok
	Parameters :	const CString& sourceFile	-	File to
													remove
					
	Usage :			Added for reasons of symmetry

   ============================================================*/
{
	ClearError( );
	CString source( sourceFile );

	BOOL result = TRUE;
	if( sourceFile.GetLength( ) )
	{
		QualifyFile( source );

		if( !( result = ::DeleteFile( source ) ) )
			SetSystemErrorMessage( ::GetLastError( ), source );
	}
	else
	{
		SetInternalErrorMessage( );
		result = FALSE;
	}

	return result;
}

////////////////////////////////////////
// Directory operations
//

BOOL CDiskObject::CreateDirectory( const CString& directory )
/* ============================================================
	Function :		CDiskObject::CreateDirectory
	Description :	Will recursively create the directory 
					directory.

	Return :		BOOL				-	TRUE if OK. 
											GetErrorMessage 
											will get an 
											error string if 
											FALSE
	Parameters :	CString directory	-	directory to 
											create
					
   ============================================================*/
{
	ClearError( );

	BOOL result = TRUE;
	CString indir( directory );
	if( indir.GetLength( ) )
	{
		QualifyPath( indir );

		_TCHAR drive[ _MAX_DRIVE ];
		_TCHAR dir[ _MAX_DIR ];
		_TCHAR fname[ _MAX_FNAME ];
		_TCHAR ext[ _MAX_EXT ];

		// Split directory into parts
		_tsplitpath_s( indir, drive, dir, fname, ext );

		TCHAR currentDirectory[ _MAX_PATH ];
		::GetCurrentDirectory( _MAX_PATH, currentDirectory );

		CStringArray directories;
		CString parts = dir;

		if( parts.GetLength( ) > 2 )
		{
			if( parts.Left( 2 ) == _T( "\\\\" ) )
			{
				// We have an UNC name
				CString strComputer;
				parts = parts.Right( parts.GetLength( ) - 2 );
				int findDir = parts.Find( _TCHAR( '\\' ) );
				if( findDir!=-1)
				{
					strComputer = _T( "\\\\" ) + parts.Left( findDir );
					parts = parts.Right( parts.GetLength( ) - ( findDir + 1 ) );
				}
				_tcscpy_s( drive, strComputer );
			}
		}

		CString strRoot( drive );

		// Strip leading \'s
		while( parts.GetLength( ) && parts[0] == _TCHAR( '\\' ) )
			parts = parts.Right( parts.GetLength( ) - 1 );

		// Cut into separate directories
		int find = parts.Find( _TCHAR( '\\' ) );
		while( find != -1 )
		{
			directories.Add( parts.Left( find ) );
			parts = parts.Right( parts.GetLength( ) - ( find + 1 ) );
			find = parts.Find( _TCHAR( '\\' ) );
		}

		if( parts.GetLength( ) )
			directories.Add( parts );

		if( fname )
			directories.Add( fname );

		// Loop directories one-by-one, creating as necessary
		int max = (int)directories.GetSize( );
		CString strCurrentDirectory( strRoot );

		for( int t = 0 ; t < max ; t++ )
		{
			strCurrentDirectory += _TCHAR( '\\' ) + directories[ t ];
			Trigger( strCurrentDirectory );
			if( !( result = ::SetCurrentDirectory( strCurrentDirectory ) ) )
			{
				if( !( result = ::CreateDirectory( strCurrentDirectory, NULL ) ) )
				{
					SetSystemErrorMessage( ::GetLastError( ), strCurrentDirectory );
					t = max;
				}
			}
		}

		::SetCurrentDirectory( currentDirectory );

	}
	else
	{
		SetInternalErrorMessage( );
		result = FALSE;
	}
	return result;
}

BOOL CDiskObject::EmptyDirectory( const CString& directory )
/* ============================================================
	Function :		CDiskObject::EmptyDirectory
	Description :	Will delete all files in directory. 

	Return :		BOOL				-	TRUE if OK. 
											GetErrorMessage will 
											get an error 
											string if FALSE
	Parameters :	CString directory	-	the directory to
											empty.

   ============================================================*/
{
	ClearError( );
	CString indir( directory );
	QualifyPath( indir );

	// Enumerate all files
	CStringArray files;
	BOOL result = EnumFilesInDirectory( indir, files );
	if( result )
	{
		int max = (int)files.GetSize( );
		for( int t = 0 ; t < max ; t++ )
		{
			// Loop and delete
			CString file = files[ t ];
			Trigger( file );
			if( !( result = ::DeleteFile( indir + file ) ) )
			{
				SetSystemErrorMessage( ::GetLastError( ), indir + file );
				t = max;
			}
		}
	}

	return result;
}

BOOL CDiskObject::RemoveDirectory( const CString& directory )
/* ============================================================
	Function :		CDiskObject::RemoveDirectory
	Description :	Will remove the directory directory, even
					if not empty. Will not remove 
					subdirectories.
					
	Return :		BOOL				-	TRUE if OK. 
											GetErrorMessage
											will get an error 
											string if FALSE
	Parameters :	CString directory	-	directory to 
											remove.

   ============================================================*/
{
	ClearError( );

	BOOL result = TRUE;
	CString indir( directory );
	if( indir.GetLength( ) )
	{
		QualifyPath( indir );

		// Wipe and remove directory
		if( ( result = EmptyDirectory( indir ) ) )
		{
			Trigger( indir );
			if( !( result = ::RemoveDirectory( indir ) ) )
				SetSystemErrorMessage( ::GetLastError( ), indir );
		}
	}
	else
	{
		// Small sanity check, we can't
		// delete the current directory.
		SetInternalErrorMessage( );
		result = FALSE;
	}

	return result;
}

BOOL CDiskObject::RemoveDirectories( const CString& directory )
/* ============================================================
	Function :		CDiskObject::RemoveDirectories
	Description :	Will remove the directory directory, even
					if not empty. Will remove subdirectories.

	Return :		BOOL				-	TRUE if OK. 
											GetErrorMessage 
											will get an error 
											string if FALSE
	Parameters :	CString directory	-	root directory to 
											remove.

   ============================================================*/
{
	ClearError( );

	BOOL result = TRUE;

	CString indir( directory );
	if( indir.GetLength( ) )
	{
		QualifyPath( indir );

		CStringArray directories;

		// Get all directories
		;
		if( ( result = EnumDirectories( indir, directories ) ) )
		{
			// Loop and remove
			int max = (int)directories.GetSize( );
			for( int t = max - 1; t >= 0 ; t-- )
				if( !( result = RemoveDirectory( directories[ t ] ) ) )
					t = -1;
			
			if( result )
				result = RemoveDirectory( indir );
		}
	}
	else
	{
		// Small sanity check, we can't
		// delete the current directory.
		SetInternalErrorMessage( );
		result = FALSE;
	}

	return result;
}

BOOL CDiskObject::EmptyDirectories( const CString& directory )
/* ============================================================
	Function :		CDiskObject::EmptyDirectories
	Description :	Will delete all files in directory. Will 
					also empty subdirectories.
					
	Return :		BOOL				-	TRUE if OK. 
											GetErrorMessage 
											will get an error 
											string if FALSE
	Parameters :	CString directory	-	the root directory 
											to empty.

   ============================================================*/
{
	ClearError( );

	CString indir( directory );

	QualifyPath( indir );
	CStringArray directories;

	// Get all directories
	BOOL result = EnumDirectories( indir, directories );
	if( result )
	{
		int max = (int)directories.GetSize( );
		// Loop and empty
		for( int t = max - 1 ; t >= 0 ; t-- )
			if( !( result = EmptyDirectory( directories[ t ] ) ) )
				t = -1;

		if( result )
			result = EmptyDirectory( indir );
	}

	return result;
}

BOOL CDiskObject::CopyDirectory( const CString& sourceDirectory,const CString& destDirectory )
/* ============================================================
	Function :		CDiskObject::CopyDirectory
	Description :	Copies all the files from sourceDirectory 
					to destDirectory. Existing files will be
					overwritten. destDirectory will be created
					if necessary. Subdirectories will not be
					copied.
					
	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will get an error 
												string if FALSE
	Parameters :	CString sourceDirectory	-	copy from.
					CString destDirectory	-	copy to.

   ============================================================*/
{
	ClearError( );
	CString source( sourceDirectory );
	CString dest( destDirectory );
	QualifyPath( source );
	QualifyPath( dest );

	Trigger( dest );

	// We don't care if this fails - CopyFiles will 
	// return an error in that case.
	::CreateDirectory( dest, NULL );

	return CopyFiles( source, dest );

}

BOOL CDiskObject::CopyDirectories( const CString& sourceDirectory,const CString& destDirectory )
/* ============================================================
	Function :		CDiskObject::CopyDirectories
	Description :	Copies all the files and subdirectories 
					from  sourceDirectory to destDirectory, 
					keeping the directory structure. Existing 
					files will be overwritten. destDirectory 
					and subdirectories will be created if 
					necessary.

					
	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will get an 
												error string 
												if FALSE
	Parameters :	CString sourceDirectory	-	copy from.
					CString destDirectory	-	copy to.

   ============================================================*/
{
	ClearError( );

	CString source( sourceDirectory );
	CString dest( destDirectory );
	QualifyPath( source );
	QualifyPath( dest );

	// Enumerate all directories and files below sourceDirectory
	CStringArray directories;
	directories.Add( source );

	BOOL result = EnumDirectories( source, directories );
	if( result )
	{
		// Create and copy directories
		int max = (int)directories.GetSize( );
		for( int t = 0 ; t < max ; t++ )
		{
			// Create names and copy
			CString from = directories[ t ];
			CString part = from.Right( from.GetLength( ) - source.GetLength( ) );
			CString to = dest + part;

			if( !( result = CopyFiles( from, to ) ) )
				t = max;
		}
	}

	return result;
}

////////////////////////////////////////
// File-oriented operations
//

BOOL CDiskObject::FileExists( const CString& file )
/* ============================================================
	Function :		CDiskObject::FileExists
	Description :	Returns TRUE if the file file exists
					
	Return :		BOOL			-	TRUE if found
	Parameters :	CString file	-	file to check

   ============================================================*/
{
	ClearError( );

	BOOL result = TRUE;

	if( file.GetLength( ) )
	{
		CString infile( file );
		QualifyFile( infile );
		HANDLE filehandle = ::CreateFile( 
								infile,
								GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL
							);

		if( filehandle == INVALID_HANDLE_VALUE )
			result = FALSE;
		else
			CloseHandle( filehandle );
	}
	else
	{
		SetInternalErrorMessage( );
		result = FALSE;
	}

	return result;
}

BOOL CDiskObject::CreateFile( const CString& file )
/* ============================================================
	Function :		CDiskObject::CreateFile
	Description :	Creates the file file, as well as the 
					directories necessary

	Return :		BOOL			-	TRUE if ok
	Parameters :	CString file	-	file to create

   ============================================================*/
{
	ClearError( );

	BOOL result = TRUE;

	if( file.GetLength( ) )
	{

		CString infile( file );
		QualifyFile( infile );

		// Split into directory and file
		CString directory = GetDirectoryName( infile );
		CString filename = GetFileName( infile );

		if( ( result = CreateDirectory( directory ) ) )
		{
			Trigger( file );
			HANDLE filehandle = ::CreateFile( 
									infile,
									GENERIC_WRITE,
									0,
									NULL,
									CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL
								);

			if( filehandle == INVALID_HANDLE_VALUE )
				result = FALSE;
			else
				CloseHandle( filehandle );
		}
	}
	else
	{
		SetInternalErrorMessage( );
		result = FALSE;
	}

	return result;
}

////////////////////////////////////////
// Helpers
//

BOOL CDiskObject::EnumFilesInDirectory( const CString& sourceDirectory, CStringArray& files, int mode )
/* ============================================================
	Function :		CDiskObject::EnumFilesInDirectory
	Description :	Enumerates all files in directory 
					sourceDirectory, and adds them to the 
					CStringArray files. Will not add 
					subdirectories

	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will contain errors
	Parameters :	CString sourceDirectory	-	The directory to 
												enumerate
					CStringArray& files		-	The resulting files
					
   ============================================================*/
{
	return EnumFilesInDirectoryWithFilter( _T( "*.*" ), sourceDirectory, files, mode );
}

BOOL CDiskObject::EnumFilesInDirectoryWithFilter( const CString& strFilter,const CString& sourceDirectory, CStringArray& files, int mode )
/* ============================================================
	Function :		CDiskObject::EnumFilesInDirectoryWithFilter
	Description :	Enumerates all files matching strFilter in 
					the directory sourceDirectory, and adds 
					them to the CStringArray files. Will not 
					add subdirectories.
					
	Return :		BOOL					-	TRUE if OK. 
												GetErrorMessage 
												will contain errors
	Parameters :	CString strFilter		-	the file name filter
					CString sourceDirectory	-	source directory
					CStringArray& files		-	output array
					
   ============================================================*/
{
	ClearError( );
	CString source( sourceDirectory );
	QualifyPath( source );
	CString sourceFiles( source + strFilter );
	BOOL result = TRUE;
	WIN32_FIND_DATA ff;

	HANDLE findhandle = FindFirstFile( sourceFiles, &ff );
	if( findhandle != INVALID_HANDLE_VALUE )
	{
		BOOL res = TRUE;
		while( res )
		{
			// We only want files
			if( ff.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
			{
				CString file;
				file = ff.cFileName;

				if( mode == EF_FULLY_QUALIFIED )
					file = source + file;

				files.Add( file );
				Trigger( file );
			}

			res = FindNextFile( findhandle, &ff );
		}

		FindClose( findhandle );
	}
	else
	{
		// Set error message
		SetSystemErrorMessage( ::GetLastError( ), sourceFiles );
		result = FALSE;
	}

	return result;
}
 
BOOL CDiskObject::EnumDirectories( const CString& sourceDirectory, CStringArray& directories )
/* ============================================================
	Function :		CDiskObject::EnumDirectories
	Description :	Enumerates all directories in directory 
					sourceDirectory, and adds them to the 
					CStringArray directories. Each entry will
					be a fully qualified name

	Return :		BOOL						-	TRUE if OK. 
													GetErrorMessage 
													will contain errors
	Parameters :	CString sourceDirectory		-	start directory
					CStringArray& directories	-	CStringArray to be 
													filled with the 
													directory names.

   ============================================================*/
{
	ClearError( );

	CString source( sourceDirectory );
	QualifyPath( source );
	CString sourceFiles( source + _T( "*.*" ) );
	BOOL result = TRUE;
	WIN32_FIND_DATA ff;

	HANDLE findhandle = FindFirstFile( sourceFiles, &ff );
	if( findhandle != INVALID_HANDLE_VALUE)
	{

		BOOL res = TRUE;
		while( res)
		{
			// We only want directories
			if( ff.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && _tcscmp( ff.cFileName, _T( "." ) ) && _tcscmp( ff.cFileName, _T( ".." ) ) )
			{
				CString directory( source + ff.cFileName );
				directories.Add( directory + _TCHAR( '\\' ) );
				Trigger( ff.cFileName );
				if( !( result = EnumDirectories( directory, directories ) ) )
					res = FALSE;
			}

			if( res )
				res = FindNextFile( findhandle, &ff );
			else
				result = FALSE;
		}

		FindClose( findhandle );
	}
	else
	{
		// Set error message
		SetSystemErrorMessage( ::GetLastError( ), sourceFiles );
		result = FALSE;
	}

	return result;
}
 
BOOL CDiskObject::EnumAllFiles( const CString& sourceDirectory, CStringArray& files )
/* ============================================================
	Function :		CDiskObject::EnumAllFiles
	Description :	Returns, recursively, the fully qualified 
					names of all files found under 
					sourceDirectory.

	Return :		BOOL					-	TRUE if OK.
	Parameters :	CString sourceDirectory	-	Root dir	
					CStringArray& files		-	output list 
												of all files 
												found

   ============================================================*/
{
	return EnumAllFilesWithFilter( _T( "*.*" ), sourceDirectory, files );
}

BOOL CDiskObject::EnumAllFilesWithFilter( const CString& filter,const CString& sourceDirectory, CStringArray& files )
/* ============================================================
	Function :		CDiskObject::EnumAllFiles
	Description :	Returns, recursively, the fully qualified 
					names of all files matching the filter
					found under sourceDirectory.

	Return :		BOOL					-	TRUE if OK.
	Parameters :	CString filter			-	Filter for files
												to find
					CString sourceDirectory	-	Root dir	
					CStringArray& files		-	output list 
												of all files 
												found

   ============================================================*/
{
	CStringArray dirs;
	BOOL result = EnumDirectories( sourceDirectory, dirs );
	if( result )
	{
		int max1 = (int)dirs.GetSize( );
		for( int t = 0 ; t < max1 ; t++ )
		{
			CString dir = dirs[ t ];
			QualifyPath( dir );
			CStringArray dirfiles;
			if( EnumFilesInDirectoryWithFilter( filter, dir, dirfiles, EF_FULLY_QUALIFIED ) ) 
			{
				int max2 = (int)dirfiles.GetSize( );
				for( int i = 0 ; i < max2 ; i++ )
				{
					CString file = dirfiles[ i ];
					Trigger( file );
					files.Add( file );
				}
			}
		}
	}

	return result;
}

////////////////////////////////////////
// Error handling

CString CDiskObject::GetErrorMessage( )
/* ============================================================
	Function :		CDiskObject::GetErrorMessage
	Description :	Will return the current error message for 
					the CDiskObject.
					
	Return :		CString	-	current error message
	Parameters :	none

   ============================================================*/
{
	return m_errorMessage;
}

// Private methods

void CDiskObject::ClearError( )
/* ============================================================
	Function :		CDiskObject::ClearError
	Description :	Clears the internal error message string
					
	Return :		void
	Parameters :	none

   ============================================================*/
{
	m_errorMessage = _T( "" );
}

void CDiskObject::SetSystemErrorMessage( int error,const CString& elements)
/* ============================================================
	Function :		CDiskObject::SetSystemErrorMessage
	Description :	Sets the error message string with the 
					system error message as gotten from 
					GetLastError. elements will be appended.

	Return :		void
	Parameters :	int error			-	error number from 
											GetLastError
					CString elements	-	files or directories 
											to add to the message
					
   ============================================================*/
{
	LPVOID msgBuff;

	// Format error message from system
	::FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		( LPTSTR ) &msgBuff,
		0,
		NULL
	);

	// Create the error string
	m_errorMessage = CString( ( LPTSTR ) msgBuff );
	m_errorMessage.TrimLeft( );
	m_errorMessage.TrimRight( );

	if( elements.GetLength( ) )
		m_errorMessage += _TCHAR( ' ' ) + elements;

	Trigger ( m_errorMessage );

	// Free the buffer.
	LocalFree( msgBuff );
}

void CDiskObject::SetInternalErrorMessage( )
/* ============================================================
	Function :		CDiskObject::SetInternalErrorMessage
	Description :	Sets the error message to the internal error
					message

	Return :		void
	Parameters :	none

   ============================================================*/
{
	m_errorMessage = _T( "CDiskObject internal error" );
	Trigger ( m_errorMessage );
}

////////////////////////////////////////
// Helpers

void CDiskObject::QualifyPath( CString& str )
/* ============================================================
	Function :		CDiskObject::QualifyPath
	Description :	Creates a fully qualified path from str. 
					
	Return :		void
	Parameters :	CString& str	-	the path to qualify

   ============================================================*/
{

	str.Replace( _TCHAR( '/' ), _TCHAR( '\\' ) );

	// Append a trailing backslash to directory name
	if( str.GetLength( ) )
		if( str[ str.GetLength( ) - 1 ] != _TCHAR( '\\' ) )
			str += _TCHAR( '\\' );

	if( str.GetLength( ) < 2 || str.Left( 2) != "\\\\" )
		QualifyFile( str );

}

void CDiskObject::QualifyFile( CString& str )
/* ============================================================
	Function :		CDiskObject::QualifyFile
	Description :	Creates a fully qualified path from str. 

	Return :		void
	Parameters :	CString& str	-	the filename to qualify

   ============================================================*/
{
	str.Replace( _TCHAR( '/' ), _TCHAR( '\\' ) );

	_TCHAR drive[ _MAX_DRIVE ];
	_TCHAR dir[ _MAX_DIR ];
	_TCHAR fname[ _MAX_FNAME ];
	_TCHAR ext[ _MAX_EXT ];

	_tsplitpath_s( str, drive, dir, fname, ext );

	if( !_tcsclen( drive ) )
	{
		CString test( dir );
		// The name is not fully qualified.
		// We assume it to be relative the 
		// current directory

		TCHAR currentDirectory[ _MAX_PATH ];
		::GetCurrentDirectory( _MAX_PATH, currentDirectory );
		if( test.GetLength( ) > 2 && test.Left( 3 ) == _T( "..\\" ) )
		{

			TCHAR upDirectory[ _MAX_PATH ];
			while( test.GetLength( ) > 2 && test.Left( 3 ) == _T( "..\\" ) )
			{
				::SetCurrentDirectory( _T( ".." ) );
				test = test.Right( test.GetLength( ) - 3 );
			}

			::GetCurrentDirectory( _MAX_PATH, upDirectory );

			str = upDirectory;
			str += _TCHAR( '\\' );
			str += fname;
			str += ext;
			::SetCurrentDirectory( currentDirectory );
		}
		else if ( test.GetLength( ) && test[ 0 ] == _TCHAR( '\\' ) )
		{
			// Start from root
			_tsplitpath_s( currentDirectory, drive, dir, fname, ext );
			str = drive + str;
		}
		else
		{
			str = _TCHAR( '\\' ) + str;
			str = currentDirectory + str;
		}
	}
}

CString CDiskObject::GetFileName( const CString& str )
/* ============================================================
	Function :		CDiskObject::GetFileName
	Description :	Returns the file name part of str
					
	Return :		CString	-	
	Parameters :	CString str	-	The complete name to check

   ============================================================*/
{
	CString instr( str );
	QualifyFile( instr );
	CString file;
	int found = instr.ReverseFind( _TCHAR( '\\' ) );
	if( found != -1 )
		file = instr.Right( str.GetLength( ) - ( found + 1 ) );

	return file;
}

CString CDiskObject::GetDirectoryName( const CString& str )
/* ============================================================
	Function :		CDiskObject::GetDirectoryName
	Description :	Returns the directory name part of str
					
	Return :		CString	-	
	Parameters :	CString str	-	The complete name to check	

   ============================================================*/
{
	CString instr( str );
	QualifyFile( instr );
	CString directory;
	int found = instr.ReverseFind( _TCHAR( '\\' ) );
	if( found != -1 )
		directory = instr.Left( found );

	return directory;
}

#pragma warning( default : 4706 )
