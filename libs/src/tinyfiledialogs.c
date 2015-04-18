/*
tinyfiledialogs.c 
Unique code file of "tiny file dialogs" created [November 9, 2014]
Copyright (c) 2014 - 2015 Guillaume Vareille http://ysengrin.com
http://tinyfiledialogs.sourceforge.net

tiny file dialogs (cross-platform C C++) 
InputBox PasswordBox MessageBox ColorPicker
OpenFileDialog SaveFileDialog SelectFolderDialog
Native dialog library for WINDOWS MAC OSX GTK+ QT CONSOLE & more
v1.11.4 [April 16, 2015] zlib licence.

A single C file (add it to your C or C++ project) with 6 modal function calls:
- message box & question box
- input box & password box
- save file dialog
- open file dialog & multiple files
- select folder dialog
- color picker.

Complement to OpenGL GLUT GLUI VTK SDL Ogre3D Unity3D or any GUI-less program,
there is NO INIT & NO MAIN LOOP. The dialogs can be forced into console mode.

On Windows:
- native code & some vbs create the graphic dialogs
- enhanced console mode can use dialog.exe from
  http://andrear.altervista.org/home/cdialog.php
- basic console input.

On Unix (command line call attempts):
- applescript
- zenity
- kdialog
- Xdialog
- python2 tkinter
- dialog (enhanced console mode)
- basic console input.
The same executable can run across desktops & distributions.

tinyfd is tested with C & C++ compilers
on Windows Visual Studio 2013 Mac OSX LINUX FREEBSD ILLUMOS SOLARIS
using Gnome Kde Enlightenment Mate Cinnamon Unity
Lxde Lxqt Xfce WindowMaker IceWm Cde Jds

- License -

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.  If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tinyfiledialogs.h"
/* #define TINYFD_WIN_CONSOLE_ONLY //*/

#ifdef _WIN32
 #ifndef TINYFD_WIN_CONSOLE_ONLY
  #include <Windows.h>
  #include <Shlobj.h>
 #endif /* TINYFD_WIN_CONSOLE_ONLY */
 #include <sys/stat.h>
 #define SLASH "\\"
#else
 #include <limits.h>
 #include <unistd.h>
 #include <dirent.h> /* on old systems try <sys/dir.h> instead */
 #include <termios.h>
 #include <sys/utsname.h>
 #define SLASH "/"
#endif /* _WIN32 */

#define MAX_PATH_OR_CMD 1024 /* _MAX_PATH or MAX_PATH */
#define MAX_MULTIPLE 32

#ifdef TINYFD_WIN_CONSOLE_ONLY
/* on windows, if you don't compile the GUI, then you must use the console */
int tinyfd_forceConsole = 1 ;
#else
int tinyfd_forceConsole = 0 ; /* 0 (default) or 1
can be modified at run time.
for unix & windows: 0 (graphic mode) or 1 (console mode).
0: try to use a graphic solution, if it fails then it uses console mode.
1: forces all dialogs into console mode even when the X server is present,
   if the package dialog (and a console is present) or dialog.exe is installed.
on windows it only make sense for console applications */
#endif /* TINYFD_WIN_CONSOLE_ONLY */

#pragma warning(disable:4996) 
/* allow usage of strncpy, strcpy, strcat, sprintf, fopen */

static int gWarningDisplayed = 0 ;
static char gTitle[]= "missing software! (so we switch to basic console input)";
static char gMessageWin[] = "tiny file dialogs on Windows needs:\n\t\
a graphic display\nor\tdialog.exe (enhanced console mode)\
\nor\ta console for basic input" ;

static char gMessageUnix[] = "tiny file dialogs on UNIX needs:\n\tapplescript\
\nor\tzenity (version 3 for the color chooser)\nor\tkdialog\
\nor\tXdialog\nor\tpython 2 with tkinter\nor\tdialog (enhanced console mode)\
\nor\ta console for basic input" ;

static char * getPathWithoutFinalSlash(
	char * const aoDestination, /* make sure it is allocated, use _MAX_PATH */
	char const * const aSource) /* aoDestination and aSource can be the same */
{
	char const * lTmp ;
	if ( aSource )
	{
		lTmp = strrchr(aSource, '/');
		if (!lTmp)
		{
			lTmp = strrchr(aSource, '\\');
		}
		if (lTmp)
		{
			strncpy(aoDestination, aSource, lTmp - aSource);
			aoDestination[lTmp - aSource] = '\0';
		}
		else
		{
			* aoDestination = '\0';
		}
	}
	else
	{
		* aoDestination = '\0';
	}
	return aoDestination;
}


static char * getLastName(
	char * const aoDestination, /* make sure it is allocated */
	char const * const aSource)
{
	/* copy the last name after '/' or '\' */
	char const * lTmp ;
	if ( aSource )
	{
		lTmp = strrchr(aSource, '/');
		if (!lTmp)
		{
			lTmp = strrchr(aSource, '\\');
		}
		if (lTmp)
		{
			strcpy(aoDestination, lTmp + 1);
		}
		else
		{
			strcpy(aoDestination, aSource);
		}
	}
	else
	{
		* aoDestination = '\0';
	}
	return aoDestination;
}


static void ensureFinalSlash ( char * const aioString )
{
	if ( aioString && strlen ( aioString ) )
	{
		char * lastcar = aioString + strlen ( aioString ) - 1 ;
		if ( strncmp ( lastcar , SLASH , 1 ) )
		{
			strcat ( lastcar , SLASH ) ;
		}
	}
}


static void Hex2RGB( char const aHexRGB [ 8 ] ,
					 unsigned char aoResultRGB [ 3 ] )
{
	char lColorChannel [ 8 ] ;
	if ( aoResultRGB )
	{
		if ( aHexRGB )
		{
			strcpy(lColorChannel, aHexRGB ) ;
			aoResultRGB[2] = (unsigned char)strtoul(lColorChannel+5,NULL,16);
			lColorChannel[5] = '\0';
			aoResultRGB[1] = (unsigned char)strtoul(lColorChannel+3,NULL,16);
			lColorChannel[3] = '\0';
			aoResultRGB[0] = (unsigned char)strtoul(lColorChannel+1,NULL,16);
/* printf("%d %d %d\n", aoResultRGB[0], aoResultRGB[1], aoResultRGB[2]); //*/
		}
		else
		{
			aoResultRGB[0]=0;
			aoResultRGB[1]=0;
			aoResultRGB[2]=0;
		}
	}
}

static void RGB2Hex( unsigned char const aRGB [ 3 ] ,
					 char aoResultHexRGB [ 8 ] )
{
	if ( aoResultHexRGB )
	{
		if ( aRGB )
		{
			sprintf(aoResultHexRGB,"#%02hhx%02hhx%02hhx",
						aRGB[0],aRGB[1],aRGB[2]);
			/* printf("aoResultHexRGB %s\n", aoResultHexRGB); //*/
		}
		else
		{
			aoResultHexRGB[0]=0;
			aoResultHexRGB[1]=0;
			aoResultHexRGB[2]=0;
		}
	}
}


static void replaceSubStr ( char const * const aSource ,
						    char const * const aOldSubStr ,
						    char const * const aNewSubStr ,
						    char * const aoDestination )
{
	char const * pOccurence ;
	char const * p ;
	char const * lNewSubStr = "" ;
	
	if ( ! aSource )
	{
		* aoDestination = '\0' ;
		return ;
	}
	if ( ! aOldSubStr )
	{
		strcpy ( aoDestination , aSource ) ;
		return ;
	}
	if ( aNewSubStr )
	{
		lNewSubStr = aNewSubStr ;
	}
	p = aSource ;
	int lOldSubLen = strlen ( aOldSubStr ) ;
	* aoDestination = '\0' ;
	while ( ( pOccurence = strstr ( p , aOldSubStr ) ) != NULL )
	{
		strncat ( aoDestination , p , pOccurence - p ) ;
		strcat ( aoDestination , lNewSubStr ) ;
		p = pOccurence + lOldSubLen ;
	}
	strcat ( aoDestination , p ) ;
}


static int replaceChr ( char * const aString ,
						char const aOldChr ,
						char const aNewChr )
{
	char * p ;
	int lRes = 0 ;

	if ( ! aString )
	{
		return 0 ;
	}
	if ( aOldChr == aNewChr )
	{
		return 0 ;
	}

	p = aString ;
	while ( (p = strchr ( p , aOldChr )) )
	{
		* p = aNewChr ;
		p ++ ;
		lRes = 1 ;
	}
	return lRes ;
}


static int filenameValid( char const * const aFileNameWithoutPath )
{
	if ( ! aFileNameWithoutPath
	  || ! strlen(aFileNameWithoutPath)
	  || strpbrk(aFileNameWithoutPath , "\\/:*?\"<>|") )
	{
		return 0 ;
	}
	return 1 ;
}


static int fileExists( char const * const aFilePathAndName )
{
	FILE * lIn ;
	if ( ! aFilePathAndName || ! strlen(aFilePathAndName) )
	{
		return 0 ;
	}
	lIn = fopen( aFilePathAndName , "r" ) ;
	if ( ! lIn )
	{
		return 0 ;
	}
	fclose ( lIn ) ;
	return 1 ;
}


/* source and destination can be the same or ovelap*/
static char const * ensureFilesExist( char * const aDestination ,
							 		  char const * const aSourcePathsAndNames)
{
	char * lDestination = aDestination ;
	char const * p ;
	char const * p2 ;
	int lLen ;

	if ( ! aSourcePathsAndNames )
	{
		return NULL ;
	}
	lLen = strlen( aSourcePathsAndNames ) ;
	if ( ! lLen )
	{
		return NULL ;
	}
	
	p = aSourcePathsAndNames ;
	while ( (p2 = strchr(p, '|')) != NULL )
	{
		lLen = p2-p ;		
		memmove(lDestination,p,lLen);
		lDestination[lLen] = '\0';
		if ( fileExists ( lDestination ) )
		{
			lDestination += lLen ;
			* lDestination = '|';
			lDestination ++ ;
		}
		p = p2 + 1 ;
	}
	if ( fileExists ( p ) )
	{
		lLen = strlen(p) ;		
		memmove(lDestination,p,lLen);
		lDestination[lLen] = '\0';
	}
	else
	{
		* (lDestination-1) = '\0';
	}
	return aDestination ;
}

#ifdef _WIN32

static int dirExists ( char const * const aDirPath )
{
	struct stat lInfo;
	if ( ! aDirPath || ! strlen ( aDirPath ) )
		return 0 ;
	if ( stat ( aDirPath , & lInfo ) != 0 )
		return 0 ;
	else if ( lInfo.st_mode & S_IFDIR )
		return 1 ;
	else
		return 0 ;
}

#ifndef TINYFD_WIN_CONSOLE_ONLY

static int messageBoxWinGui (
    char const * const aTitle , /* NULL or "" */
    char const * const aMessage , /* NULL or ""  may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno" */
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) /* 0 for cancel/no , 1 for ok/yes */
{
	int lBoxReturnValue;
    UINT aCode ;
	
	if ( ! strcmp( "warning" , aIconType ) )
	{
		aCode = MB_ICONWARNING ;
	}
	else if (!strcmp("error", aIconType))
	{
		aCode = MB_ICONERROR ;
	}
	else if (!strcmp("question", aIconType))
	{
		aCode = MB_ICONQUESTION ;
	}
	else
	{
		aCode = MB_ICONINFORMATION ;
	}

	if ( ! strcmp( "okcancel" , aDialogType ) )
	{
		aCode += MB_OKCANCEL ;
		if ( ! aDefaultButton )
		{
			aCode += MB_DEFBUTTON2 ;
		}
	}
	else if ( ! strcmp( "yesno" , aDialogType ) )
	{
		aCode += MB_YESNO ;
		if ( ! aDefaultButton )
		{
			aCode += MB_DEFBUTTON2 ;
		}
	}
	else
	{
		aCode += MB_OK ;
	}

	lBoxReturnValue = MessageBox(NULL, aMessage, aTitle, aCode);
	if ( ( ! strcmp("ok", aDialogType) )
		|| (lBoxReturnValue == IDOK)
		|| (lBoxReturnValue == IDYES) )
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}


static char const * inputBoxWinGui(
	char * const aoBuff ,
	char const * const aTitle , /* NULL or "" */
	char const * const aMessage , /* NULL or "" may NOT contain \n nor \t */
	char const * const aDefaultInput ) /* "" , if NULL it's a passwordBox */
{
	char lDialogString[4*MAX_PATH_OR_CMD];
	FILE * lIn;
	int lResult;

	if (aDefaultInput)
	{
		sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfd.vbs",
			getenv("USERPROFILE"));
	}
	else
	{
		sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfd.hta",
			getenv("USERPROFILE"));
	}
	lIn = fopen(lDialogString, "w");
	if (!lIn)
	{
		return NULL;
	}

	if ( aDefaultInput )
	{
		strcpy(lDialogString, "Dim result:result=InputBox(\"");
		if (aMessage && strlen(aMessage))
		{
			strcat(lDialogString, aMessage);
		}
		strcat(lDialogString, "\",\"");
		if (aTitle && strlen(aTitle))
		{

			strcat(lDialogString, aTitle);
		}
		strcat(lDialogString, "\",\"");
		if (aDefaultInput && strlen(aDefaultInput))
		{
			strcat(lDialogString, aDefaultInput);
		}
		strcat(lDialogString, "\"):If IsEmpty(result) then:WScript.Echo 0");
		strcat(lDialogString, ":Else: WScript.Echo \"1\" & result : End If");
	}
	else
	{
		sprintf(lDialogString, "\n\
<html>\n\
<head>\n\
<title>%s</title>\n\
<HTA:APPLICATION\n\
ID = 'tinyfdHTA'\n\
APPLICATIONNAME = 'tinyfd_inputBox'\n\
BORDER = 'thin'\n\
SCROLL = 'no'\n\
SINGLEINSTANCE = 'yes'\n\
WINDOWSTATE = 'hidden'\n\
>\n\
\n\
<script language = 'VBScript'>\n\
\n\
'Dim arrCommands\n\
result = 0\n\
\n\
Sub Window_onLoad\n\
intWidth = 375\n\
intHeight = 165\n\
Me.ResizeTo intWidth, intHeight\n\
Me.MoveTo((Screen.Width/2)-(intWidth/2)),((Screen.Height/2)-(intHeight/2))\n\
txt_input.Focus\n\
' if the args are written each in ' ' \n\
'arrCommands = Split(tinyfdHTA.commandLine, chr(34)) \n\
'Msgbox arrCommands(0)\n\
'For i = 1 to (Ubound(arrCommands) - 1) Step 2\n\
'    Msgbox arrCommands(i)\n\
'Next\n\
End Sub\n\
\n\
Sub Window_onUnload\n\
Set objFSO = CreateObject(\"Scripting.FileSystemObject\")\n\
Set oShell = CreateObject(\"WScript.Shell\")\n\
strHomeFolder = oShell.ExpandEnvironmentStrings(\"%%USERPROFILE%%\")\n\
Set objFile = objFSO.CreateTextFile(strHomeFolder & \"\\AppData\\Local\\Temp\\tinyfd.txt\",True)\n\
If result = 1 Then\n\
objFile.Write 1 & txt_input.Value\n\
Else\n\
objFile.Write 0\n\
End If\n\
objFile.Close\n\
End Sub\n\
\n\
Sub Run_ProgramOK\n\
result = 1\n\
window.Close\n\
End Sub\n\
\n\
Sub Run_ProgramCancel\n\
window.Close\n\
End Sub\n\
\n\
Sub Default_Buttons\n\
If Window.Event.KeyCode = 13 Then\n\
btn_OK.Click\n\
ElseIf Window.Event.KeyCode = 27 Then\n\
btn_Cancel.Click\n\
End If\n\
End Sub\n\
\n\
</script>\n\
</head>\n\
<body style = 'background-color:#EEEEEE' onkeypress = 'vbs:Default_Buttons' align = 'top'>\n\
<table width = '100%%' height = '80%%' align = 'center' border = '0'>\n\
<tr border = '0'>\n\
<td align = 'left' style = 'font-family: arial; font-size: 11px;'>\n\
%s\n\
</td>\n\
<td align = 'right' style = 'margin-top: 0em;'>\n\
<table  align = 'right' style = 'margin-right: 0em;'>\n\
<tr align = 'right' 'margin-top: 5em;'>\n\
<input type = 'button' value = 'OK' name = 'btn_OK' onClick = 'vbs:Run_ProgramOK' style = 'font-size: 11px; width: 5em; margin-top: 2em;'><br>\n\
<input type = 'button' value = 'Cancel' name = 'btn_Cancel' onClick = 'vbs:Run_ProgramCancel' style = 'font-size: 11px; width: 5em;'><br><br>\n\
</tr>\n\
</table>\n\
</td>\n\
</tr>\n\
</table>\n\
<table width = '100%%' height = '100%%' align = 'center' border = '0'>\n\
<tr>\n\
<td align = 'left' style = 'font-family: arial; font-size: 16px; font-weight: bold;'>\n\
<input type = 'password' size = '61' id = 'txt_input'\n\
name = 'txt_input' style = 'font-size: 11px;' value = '' ><BR>\n\
</td>\n\
</tr>\n\
</table>\n\
</body>\n\
</html>\n\
"		, aTitle ? aTitle : "", aMessage ? aMessage : "") ;
	}
	fputs(lDialogString, lIn);
	fclose(lIn);

	if ( GetConsoleWindow() )
	{
		strcpy(lDialogString, "");
	}
	else
	{
		strcpy(lDialogString, "powershell -WindowStyle Hidden -Command \"");
	}
	if (aDefaultInput)
	{
		strcat(lDialogString,
			"cscript.exe %USERPROFILE%\\AppData\\Local\\Temp\\tinyfd.vbs");
	}
	else
	{
		strcat(lDialogString,
			"mshta.exe %USERPROFILE%\\AppData\\Local\\Temp\\tinyfd.hta");
	}
	if ( ! GetConsoleWindow() )
	{
		strcat(lDialogString, "\"");
	}

/*
strcpy(lDialogString, "mshta vbscript:Execute(\"resizeTo 0,0:moveTo -1000,0:\
dim result:result=InputBox(\"\"message\"\", \"\"title\"\", \"\"input\"\"):\
Set objFSO=CreateObject(\"\"Scripting.FileSystemObject\"\"):\
Set objFile=objFSO.CreateTextFile(\
\"\"%USERPROFILE%\\AppData\\Local\\Temp\\tinyfdInputBox.txt\"\",True):\
objFile.Write result & vbCrLf:objFile.Close:window.close\") ");
*/

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if (!(lIn = _popen(lDialogString,"r")))
	{
		return NULL ;
	}
	while ( fgets ( aoBuff , MAX_PATH_OR_CMD , lIn ) != NULL )
	{}
	_pclose ( lIn ) ;
	if ( aoBuff[ strlen ( aoBuff ) -1 ] == '\n' )
	{
		aoBuff[ strlen ( aoBuff ) -1 ] = '\0' ;
	}
	if (aDefaultInput)
	{
		sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfd.vbs",
			getenv("USERPROFILE"));
	}
	else
	{
		sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfd.txt",
			getenv("USERPROFILE"));
		if (!(lIn = fopen(lDialogString, "r")))
		{
			remove(lDialogString);
			sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfd.hta",
				getenv("USERPROFILE"));
			return NULL;
		}
		while (fgets(aoBuff, MAX_PATH_OR_CMD, lIn) != NULL)
		{}
		fclose(lIn);
		remove(lDialogString);
		sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfd.hta",
			getenv("USERPROFILE"));
	}
	remove(lDialogString);
	/* printf ( "aoBuff: %s\n" , aoBuff ) ; //*/
	lResult = strncmp(aoBuff, "1", 1) ? 0 : 1;
	/* printf ( "lResult: %d \n" , lResult ) ; //*/
	if (!lResult)
	{
		return NULL ;
	}
	/* printf ( "aoBuff+1: %s\n" , aoBuff+1 ) ; //*/
	return aoBuff + 1;
}


static char const * saveFileDialogWinGui (
	char * const aoBuff ,
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) /* NULL or {"*.jpg","*.png"} */
{
	char lDirname [ MAX_PATH_OR_CMD ] ;
	char lDialogString[MAX_PATH_OR_CMD];
	char lFileFilters[MAX_PATH_OR_CMD] = "";
    int i ;
    char * p;
	OPENFILENAME ofn ;

	getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(aoBuff, aDefaultPathAndFile);
    
	if (aNumOfFileFilters > 0)
	{
		strcat(lFileFilters, aFileFilters[0]);
		for (i = 1; i < aNumOfFileFilters; i++)
		{
			strcat(lFileFilters, ";");
			strcat(lFileFilters, aFileFilters[i]);
		}
		strcat(lFileFilters, "\n");
		strcpy(lDialogString, lFileFilters);
		strcat(lFileFilters, lDialogString);
		strcat(lFileFilters, "All Files\n*.*\n");
		p = lFileFilters;
		while ((p = strchr(p, '\n')) != NULL)
		{
			*p = '\0';
			p ++ ;
		}
	}
    
	ofn.lStructSize     = sizeof(OPENFILENAME) ;
	ofn.hwndOwner       = 0 ;
	ofn.hInstance       = 0 ;
	ofn.lpstrFilter     = lFileFilters ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter  = 0 ;
	ofn.nFilterIndex    = 0 ;
	ofn.lpstrFile		= aoBuff;

	ofn.nMaxFile        = MAX_PATH_OR_CMD ;
	ofn.lpstrFileTitle  = NULL ;
	ofn.nMaxFileTitle   = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir = lDirname;
	ofn.lpstrTitle      = aTitle ;
	ofn.Flags           = OFN_OVERWRITEPROMPT ;
	ofn.nFileOffset     = 0 ;
	ofn.nFileExtension  = 0 ;
	ofn.lpstrDefExt     = NULL ;
	ofn.lCustData       = 0L ;
	ofn.lpfnHook        = NULL ;
	ofn.lpTemplateName  = NULL ;

	if ( GetSaveFileName ( & ofn ) == 0 )
	{
		return NULL ;
	}
	else 
	{ 
		return aoBuff ;
	}
}


static char const * openFileDialogWinGui (
	char * const aoBuff ,
    char const * const aTitle , /*  NULL or "" */
    char const * const aDefaultPathAndFile , /*  NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
	char lDirname [ MAX_PATH_OR_CMD ] ;
	char lFileFilters[MAX_PATH_OR_CMD] = "";
	char lDialogString[MAX_PATH_OR_CMD] ;
	char * lPointers[MAX_MULTIPLE];
	size_t lLengths[MAX_MULTIPLE];
	int i , j ;
	char * p;
	OPENFILENAME ofn;
    size_t lBuffLen ;

	getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(aoBuff, aDefaultPathAndFile);

	if (aNumOfFileFilters > 0)
	{
		strcat(lFileFilters, aFileFilters[0]);
		for (i = 1; i < aNumOfFileFilters; i++)
		{
			strcat(lFileFilters, ";");
			strcat(lFileFilters, aFileFilters[i]);
		}
		strcat(lFileFilters, "\n");
		strcpy(lDialogString, lFileFilters);
		strcat(lFileFilters, lDialogString);
		strcat(lFileFilters, "All Files\n*.*\n");
		p = lFileFilters;
		while ((p = strchr(p, '\n')) != NULL)
		{
			*p = '\0';
			p ++ ;
		}
	}

	ofn.lStructSize     = sizeof ( OPENFILENAME ) ;
	ofn.hwndOwner       = 0 ;
	ofn.hInstance       = 0 ;
	ofn.lpstrFilter		= lFileFilters;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter  = 0 ;
	ofn.nFilterIndex    = 0 ;
	ofn.lpstrFile		= aoBuff ;
	ofn.nMaxFile        = MAX_PATH_OR_CMD ;
	ofn.lpstrFileTitle  = NULL ;
	ofn.nMaxFileTitle   = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir = lDirname ;
	ofn.lpstrTitle      = aTitle ;
	ofn.Flags			= OFN_EXPLORER ;
	ofn.nFileOffset     = 0 ;
	ofn.nFileExtension  = 0 ;
	ofn.lpstrDefExt     = NULL ;
	ofn.lCustData       = 0L ;
	ofn.lpfnHook        = NULL ;
	ofn.lpTemplateName  = NULL ;

	if ( aAllowMultipleSelects )
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	if ( GetOpenFileName ( & ofn ) == 0 )
	{
		return NULL ;
	}
	else 
	{
		lBuffLen = strlen(aoBuff) ;
		lPointers[0] = aoBuff + lBuffLen + 1 ;
		if ( !aAllowMultipleSelects || (lPointers[0][0] == '\0')  )
			return aoBuff ;
        
		i = 0 ;
		do
		{
			lLengths[i] = strlen(lPointers[i]);
			lPointers[i+1] = lPointers[i] + lLengths[i] + 1 ;
			i ++ ;
		}
		while ( lPointers[i][0] != '\0' );
		i--;
		p = aoBuff + MAX_MULTIPLE*MAX_PATH_OR_CMD - 1 ;
		* p = '\0';
		for ( j = i ; j >=0 ; j-- )
		{
			p -= lLengths[j];
			memmove(p, lPointers[j], lLengths[j]);
			p--;
			*p = '\\';
			p -= lBuffLen ;
			memmove(p, aoBuff, lBuffLen);
			p--;
			*p = '|';
		}
		p++;
		return p ;
	}
}


static char const * selectFolderDialogWinGui (
	char * const aoBuff ,
	char const * const aTitle , /*  NULL or "" */
	char const * const aDefaultPath ) /* NULL or "" */
{
	BROWSEINFO bInfo ;
	LPITEMIDLIST lpItem ;
	
	/* we can't use aDefaultPath */
	bInfo.hwndOwner = 0 ;
	bInfo.pidlRoot = NULL ;
	bInfo.pszDisplayName = aoBuff ;
	bInfo.lpszTitle = aTitle ;
	bInfo.ulFlags = 0 ;
	bInfo.lpfn = NULL ;
	bInfo.lParam = 0 ;
	bInfo.iImage = -1 ;

	lpItem = SHBrowseForFolder ( & bInfo ) ;
	if ( lpItem )
	{
		SHGetPathFromIDList ( lpItem , aoBuff ) ;
	}
	return aoBuff ;
}


static char const * colorChooserWinGui(
	char const * const aTitle, /* NULL or "" */
	char const * const aDefaultHexRGB, /* NULL or "#FF0000"*/
	unsigned char const aDefaultRGB[3], /* { 0 , 255 , 255 } */
	unsigned char aoResultRGB[3]) /* { 0 , 0 , 0 } */
{
	static CHOOSECOLOR cc;
	static COLORREF crCustColors[16];
	static char lResultHexRGB[8];
	unsigned char lDefaultRGB[3];
	int lRet;

	if ( aDefaultHexRGB )
	{
		Hex2RGB(aDefaultHexRGB, lDefaultRGB);
	}
	else
	{
		lDefaultRGB[0]=aDefaultRGB[0];
		lDefaultRGB[1]=aDefaultRGB[1];
		lDefaultRGB[2]=aDefaultRGB[2];
	}

	/* we can't use aTitle */
	cc.lStructSize = sizeof ( CHOOSECOLOR ) ;
	cc.hwndOwner = NULL ;
	cc.hInstance = NULL ;
	cc.rgbResult = RGB(lDefaultRGB[0], lDefaultRGB[1], lDefaultRGB[2]);
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.lCustData = 0;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	lRet = ChooseColor(&cc);

	if ( ! lRet )
	{
		return NULL;
	}

	aoResultRGB[0] = GetRValue(cc.rgbResult);
	aoResultRGB[1] = GetGValue(cc.rgbResult);
	aoResultRGB[2] = GetBValue(cc.rgbResult);

	RGB2Hex(aoResultRGB, lResultHexRGB);

	return lResultHexRGB;
}

#endif /* TINYFD_WIN_CONSOLE_ONLY */


static int dialogPresent ( )
{
	static int lDialogPresent = -1 ;
	char lBuff [ MAX_PATH_OR_CMD ] ;
	FILE * lIn ;
	char const * lString = "dialog.exe";
	if ( lDialogPresent < 0 )
	{
		if (!(lIn = _popen("where dialog.exe","r")))
		{
			lDialogPresent = 0 ;
			return 0 ;
		}
		while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
		{}
		_pclose ( lIn ) ;
		if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
		{
			lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
		}
		if ( strcmp(lBuff+strlen(lBuff)-strlen(lString),lString) )
		{
			lDialogPresent = 0 ;
		}
		else
		{
			lDialogPresent = 1 ;
		}
	}
	return lDialogPresent ;
}


static int messageBoxWinConsole (
    char const * const aTitle , /* NULL or "" */
    char const * const aMessage , /* NULL or ""  may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno" */
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) /* 0 for cancel/no , 1 for ok/yes */
{
	char lDialogString[MAX_PATH_OR_CMD];
	char lDialogFile[MAX_PATH_OR_CMD];
	FILE * lIn;
    char lBuff [ MAX_PATH_OR_CMD ] = "";
	
	strcpy ( lDialogString , "dialog " ) ;
	if ( aTitle && strlen(aTitle) )
	{
		strcat(lDialogString, "--title \"") ;
		strcat(lDialogString, aTitle) ;
		strcat(lDialogString, "\" ") ;
	}
	if ( ! strcmp( "okcancel" , aDialogType ) )
	{
		if ( ! aDefaultButton )
		{
			strcat ( lDialogString , "--defaultno " ) ;
		}
		strcat ( lDialogString ,
				"--yes-label \"Ok\" --no-label \"Cancel\" --yesno " ) ;
	}
	else if ( ! strcmp( "yesno" , aDialogType ) )
	{
		if ( ! aDefaultButton )
		{
			strcat ( lDialogString , "--defaultno " ) ;
		}
		strcat ( lDialogString , "--yesno " ) ;
	}
	else
	{
		strcat ( lDialogString , "--msgbox " ) ;
			
	}


	strcat ( lDialogString , "\"" ) ;
	if ( aMessage && strlen(aMessage) )
	{
		replaceSubStr ( aMessage , "\n" , "\\n" , lBuff ) ;
		strcat(lDialogString, lBuff) ;
		lBuff[0]='\0';
	}

	strcat(lDialogString, "\" 10 60");
	strcat(lDialogString, " && echo 1 > ");

	strcpy(lDialogFile, getenv("USERPROFILE"));
	strcat(lDialogFile, "\\AppData\\Local\\Temp\\tinyfd.txt");
	strcat(lDialogString, lDialogFile);

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	system ( lDialogString ) ;
		
	if (!(lIn = fopen(lDialogFile, "r")))
	{
		remove(lDialogFile);
		return 0 ;
	}
	while (fgets(lBuff, sizeof(lBuff), lIn) != NULL)
	{}
	fclose(lIn);
	remove(lDialogFile);
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( ! strlen(lBuff) )
	{
		return 0;
	}
	return 1;
}


static char const * inputBoxWinConsole(
	char * const aoBuff ,
	char const * const aTitle , /* NULL or "" */
	char const * const aMessage , /* NULL or "" may NOT contain \n nor \t */
	char const * const aDefaultInput ) /* "" , if NULL it's a passwordBox */
{
	char lDialogString[MAX_PATH_OR_CMD];
	char lDialogFile[MAX_PATH_OR_CMD];
	FILE * lIn;
	int lResult;

	strcpy(lDialogFile, getenv("USERPROFILE"));
	strcat(lDialogFile, "\\AppData\\Local\\Temp\\tinyfd.txt");
	strcpy(lDialogString , "echo|set /p=1 >" ) ;
	strcat(lDialogString, lDialogFile);
	strcat( lDialogString , " & " ) ;

	strcat ( lDialogString , "dialog " ) ;
	if ( aTitle && strlen(aTitle) )
	{
		strcat(lDialogString, "--title \"") ;
		strcat(lDialogString, aTitle) ;
		strcat(lDialogString, "\" ") ;
	}
	if ( ! aDefaultInput )
	{
		strcat ( lDialogString , "--passwordbox" ) ;
	}
	else
	{
		strcat ( lDialogString , "--inputbox" ) ;
	}
	strcat ( lDialogString , " \"" ) ;
	if ( aMessage && strlen(aMessage) )
	{
		strcat(lDialogString, aMessage) ;
	}
	strcat(lDialogString,"\" 10 60 ") ;
	if ( aDefaultInput && strlen(aDefaultInput) )
	{
		strcat(lDialogString, "\"") ;
		strcat(lDialogString, aDefaultInput) ;
		strcat(lDialogString, "\" ") ;
	}

	strcat(lDialogString, "2>>");
	strcpy(lDialogFile, getenv("USERPROFILE"));
	strcat(lDialogFile, "\\AppData\\Local\\Temp\\tinyfd.txt");
	strcat(lDialogString, lDialogFile);
	strcat(lDialogString, " || echo 0 > ");
	strcat(lDialogString, lDialogFile);

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	system ( lDialogString ) ;

	if (!(lIn = fopen(lDialogFile, "r")))
	{
		remove(lDialogFile);
		return 0 ;
	}
	while (fgets(aoBuff, MAX_PATH_OR_CMD, lIn) != NULL)
	{}
	fclose(lIn);
	remove(lDialogFile);
    if ( aoBuff[ strlen ( aoBuff ) -1 ] == '\n' )
    {
    	aoBuff[ strlen ( aoBuff ) -1 ] = '\0' ;
    }
	/* printf ( "aoBuff: %s\n" , aoBuff ) ; //*/

	/* printf ( "aoBuff: %s len: %lu \n" , aoBuff , strlen(aoBuff) ) ; //*/
    lResult =  strncmp ( aoBuff , "1" , 1) ? 0 : 1 ;
	/* printf ( "lResult: %d \n" , lResult ) ; //*/
    if ( ! lResult )
    {
		return NULL ;
	}
	/* printf ( "aoBuff+1: %s\n" , aoBuff+1 ) ; //*/
	return aoBuff+3 ;
}


static char const * saveFileDialogWinConsole (
	char * const aoBuff ,
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) /* NULL or {"*.jpg","*.png"} */
{
	char lDialogString[MAX_PATH_OR_CMD];
	char lPathAndFile[MAX_PATH_OR_CMD] = "";
	FILE * lIn;

	strcpy ( lDialogString , "dialog " ) ;
 	if ( aTitle && strlen(aTitle) )
	{
		strcat(lDialogString, "--title \"") ;
		strcat(lDialogString, aTitle) ;
		strcat(lDialogString, "\" ") ;
	}
	strcat ( lDialogString , "--fselect \"" ) ;
	if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
	{
		// dialog.exe uses unix separators even on windows
		strcpy(lPathAndFile, aDefaultPathAndFile);
		replaceChr ( lPathAndFile , '\\' , '/' ) ;
	}
		
	// dialog.exe needs at least one separator
	if ( ! strchr(lPathAndFile, '/') )
	{
		strcat(lDialogString, "./") ;
	}
	strcat(lDialogString, lPathAndFile) ;
	strcat(lDialogString, "\" 0 60 2>");
	strcpy(lPathAndFile, getenv("USERPROFILE"));
	strcat(lPathAndFile, "\\AppData\\Local\\Temp\\tinyfd.txt");
	strcat(lDialogString, lPathAndFile);

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	system ( lDialogString ) ;

	if (!(lIn = fopen(lPathAndFile, "r")))
	{
		remove(lPathAndFile);
		return NULL;
	}
	while (fgets(aoBuff, MAX_PATH_OR_CMD, lIn) != NULL)
	{}
	fclose(lIn);
	remove(lPathAndFile);
	replaceChr ( aoBuff , '/' , '\\' ) ;
	/* printf ( "aoBuff: %s\n" , aoBuff ) ; //*/
	getLastName(lDialogString,aoBuff);
	if ( ! strlen(lDialogString) )
	{
		return NULL;
	}
	return aoBuff;
}


static char const * openFileDialogWinConsole (
	char * const aoBuff ,
    char const * const aTitle , /*  NULL or "" */
    char const * const aDefaultPathAndFile , /*  NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
	char lFileFilters[MAX_PATH_OR_CMD] = "";
	char lDialogString[MAX_PATH_OR_CMD] ;
	FILE * lIn;


	strcpy ( lDialogString , "dialog " ) ;
 	if ( aTitle && strlen(aTitle) )
	{
		strcat(lDialogString, "--title \"") ;
		strcat(lDialogString, aTitle) ;
		strcat(lDialogString, "\" ") ;
	}
	strcat ( lDialogString , "--fselect \"" ) ;
	if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
	{
		// dialog.exe uses unix separators even on windows
		strcpy(lFileFilters, aDefaultPathAndFile);
		replaceChr ( lFileFilters , '\\' , '/' ) ;
	}
		
	// dialog.exe needs at least one separator
	if ( ! strchr(lFileFilters, '/') )
	{
		strcat(lDialogString, "./") ;
	}
	strcat(lDialogString, lFileFilters) ;
	strcat(lDialogString, "\" 0 60 2>");
	strcpy(lFileFilters, getenv("USERPROFILE"));
	strcat(lFileFilters, "\\AppData\\Local\\Temp\\tinyfd.txt");
	strcat(lDialogString, lFileFilters);

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	system ( lDialogString ) ;

	if (!(lIn = fopen(lFileFilters, "r")))
	{
		remove(lFileFilters);
		return NULL;
	}
	while (fgets(aoBuff, MAX_PATH_OR_CMD, lIn) != NULL)
	{}
	fclose(lIn);
	remove(lFileFilters);
	replaceChr ( aoBuff , '/' , '\\' ) ;
	/* printf ( "aoBuff: %s\n" , aoBuff ) ; //*/
	return aoBuff;
}


static char const * selectFolderDialogWinConsole (
	char * const aoBuff ,
	char const * const aTitle , /*  NULL or "" */
	char const * const aDefaultPath ) /* NULL or "" */
{
	char lDialogString [ MAX_PATH_OR_CMD ] ;
	char lString [ MAX_PATH_OR_CMD ] ;
	FILE * lIn ;
	
	strcpy ( lDialogString , "dialog " ) ;
 	if ( aTitle && strlen(aTitle) )
	{
		strcat(lDialogString, "--title \"") ;
		strcat(lDialogString, aTitle) ;
		strcat(lDialogString, "\" ") ;
	}
	strcat ( lDialogString , "--dselect \"" ) ;
	if ( aDefaultPath && strlen(aDefaultPath) )
	{
		// dialog.exe uses unix separators even on windows
		strcpy(lString, aDefaultPath) ;
		ensureFinalSlash(lString);
		replaceChr ( lString , '\\' , '/' ) ;
		strcat(lDialogString, lString) ;
	}
	else
	{
		// dialog.exe needs at least one separator
		strcat(lDialogString, "./") ;
	}
	strcat(lDialogString, "\" 0 60 2>");
	strcpy(lString, getenv("USERPROFILE"));
	strcat(lString, "\\AppData\\Local\\Temp\\tinyfd.txt");
	strcat(lDialogString, lString);

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	system ( lDialogString ) ;

	if (!(lIn = fopen(lString, "r")))
	{
		remove(lString);
		return NULL;
	}
	while (fgets(aoBuff, MAX_PATH_OR_CMD, lIn) != NULL)
	{}
	fclose(lIn);
	remove(lString);
	replaceChr ( aoBuff , '/' , '\\' ) ;
	/* printf ( "aoBuff: %s\n" , aoBuff ) ; //*/
	return aoBuff;
}


/* returns 0 for cancel/no , 1 for ok/yes */
int tinyfd_messageBox (
    char const * const aTitle , /* NULL or "" */
    char const * const aMessage , /* NULL or ""  may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno" */
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) /* 0 for cancel/no , 1 for ok/yes */
{
	char lBuff [ MAX_PATH_OR_CMD ] ;
	char aChar ;

#ifndef TINYFD_WIN_CONSOLE_ONLY
	if ( ( !tinyfd_forceConsole || !( GetConsoleWindow() || dialogPresent() ) )
	  && ( !getenv("SSH_CLIENT") || getenv("DISPLAY") ) )
	{
		return messageBoxWinGui(
					aTitle,aMessage,aDialogType,aIconType,aDefaultButton);
	}
	else
#endif /* TINYFD_WIN_CONSOLE_ONLY */
	if ( dialogPresent() )
	{
		return messageBoxWinConsole(
					aTitle,aMessage,aDialogType,aIconType,aDefaultButton);
	}
	else
	{
		if ( !gWarningDisplayed )
		{
			gWarningDisplayed = 1 ;
			printf ("\n\n%s\n", gTitle);
			printf ("%s\n\n", gMessageWin);
		}
 		if ( aTitle && strlen(aTitle) )
		{
			printf ("%s", aTitle);
		}
		if ( !strcmp("okcancel",aDialogType) || !strcmp("yesno",aDialogType) )
		{
			do
			{
				if ( aMessage && strlen(aMessage) )
				{
					printf("\n%s (y/n) ",aMessage);
				}
				else
				{
					printf("\n (y/n) ");
				}
				fgets(lBuff, MAX_PATH_OR_CMD, stdin);
				aChar = (char) tolower(lBuff[strlen(lBuff)-2]);
			}
			while ( strlen(lBuff) != 2 || (aChar != 'y' && aChar != 'n') );
			return aChar == 'y' ? 1 : 0 ;
		}
		else
		{
			if ( aMessage && strlen(aMessage) )
			{
				printf ("\n%s\n", aMessage);
			}
			else
			{
				printf("\n");
			}
			return 1 ;
		}
	}
}


/* returns NULL on cancel */
char const * tinyfd_inputBox(
	char const * const aTitle , /* NULL or "" */
	char const * const aMessage , /* NULL or "" may NOT contain \n nor \t */
	char const * const aDefaultInput ) /* "" , if NULL it's a passwordBox */
{
	static char lBuff [ MAX_PATH_OR_CMD ] ;

#ifndef TINYFD_WIN_CONSOLE_ONLY
	DWORD mode = 0;
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);;
	if ( ( !tinyfd_forceConsole || !( GetConsoleWindow() || dialogPresent() ) )
	  && ( !getenv("SSH_CLIENT") || getenv("DISPLAY") ) )
	{
		lBuff[0]='\0';
		return inputBoxWinGui(lBuff,aTitle,aMessage,aDefaultInput);
	}
	else
#endif /* TINYFD_WIN_CONSOLE_ONLY */
	if ( dialogPresent() )
	{
		lBuff[0]='\0';
		return inputBoxWinConsole(lBuff,aTitle,aMessage,aDefaultInput);
	}
	else 
	{
		lBuff[0]='\0';
		if ( !gWarningDisplayed )
		{
			gWarningDisplayed = 1 ;
			printf ("\n\n%s\n", gTitle);
			printf ("%s\n\n", gMessageWin);
		}
		if ( aTitle && strlen(aTitle) )
		{
			printf ("%s\n", aTitle);
		}
		if ( aMessage && strlen(aMessage) )
		{
			printf("%s",aMessage);
		}
		printf(" (esc+enter to cancel): ");
#ifndef TINYFD_WIN_CONSOLE_ONLY
		if ( ! aDefaultInput )
		{
			GetConsoleMode(hStdin,&mode);
			SetConsoleMode(hStdin,mode & (~ENABLE_ECHO_INPUT) );
		}
#endif /* TINYFD_WIN_CONSOLE_ONLY */
		fgets(lBuff, MAX_PATH_OR_CMD, stdin);
#ifndef TINYFD_WIN_CONSOLE_ONLY
		if ( ! aDefaultInput )
		{
			SetConsoleMode(hStdin,mode);
		}
#endif /* TINYFD_WIN_CONSOLE_ONLY */
		if ( strchr(lBuff,27) )
		{
			return NULL ;
		}
		if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
		{
			lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
		}
		return lBuff ;
	}
}


char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) /* NULL or {"*.jpg","*.png"} */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
	char lString[MAX_PATH_OR_CMD] ;
	char const * p ;
	lBuff[0]='\0';
#ifndef TINYFD_WIN_CONSOLE_ONLY
	if ( ( !tinyfd_forceConsole || !( GetConsoleWindow() || dialogPresent() ) )
	  && ( !getenv("SSH_CLIENT") || getenv("DISPLAY") ) )
	{
		p = saveFileDialogWinGui(lBuff,
				aTitle,aDefaultPathAndFile,aNumOfFileFilters,aFileFilters);
	}
	else
#endif /* TINYFD_WIN_CONSOLE_ONLY */
	if ( dialogPresent() )
	{
		p = saveFileDialogWinConsole(lBuff,
				aTitle,aDefaultPathAndFile,aNumOfFileFilters,aFileFilters);
	}
	else
	{
		p = tinyfd_inputBox(aTitle, "Save file","");
	}

	if ( ! p || ! strlen ( p )  )
	{
		return NULL;
	}
	getPathWithoutFinalSlash ( lString , p ) ;
	if ( strlen ( lString ) && ! dirExists ( lString ) )
	{
		return NULL ;
	}
	getLastName(lString,p);
	if ( ! filenameValid(lString) )
	{
		return NULL;
	}
	return p ;
}


/* in case of multiple files, the separator is | */
char const * tinyfd_openFileDialog (
    char const * const aTitle , /*  NULL or "" */
    char const * const aDefaultPathAndFile , /*  NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
	static char lBuff[MAX_MULTIPLE*MAX_PATH_OR_CMD];
	char const * p ;
#ifndef TINYFD_WIN_CONSOLE_ONLY
	if ( ( !tinyfd_forceConsole || !( GetConsoleWindow() || dialogPresent() ) )
	  && ( !getenv("SSH_CLIENT") || getenv("DISPLAY") ) )
	{
		p = openFileDialogWinGui(lBuff,
				aTitle,aDefaultPathAndFile,aNumOfFileFilters,
				aFileFilters,aAllowMultipleSelects);
	}
	else
#endif /* TINYFD_WIN_CONSOLE_ONLY */
	if ( dialogPresent() )
	{
		p = openFileDialogWinConsole(lBuff,
				aTitle,aDefaultPathAndFile,aNumOfFileFilters,
				aFileFilters,aAllowMultipleSelects);
	}
	else
	{
		p = tinyfd_inputBox(aTitle, "Open file","");
	}

	if ( ! p || ! strlen ( p )  )
	{
		return NULL;
	}
	if ( aAllowMultipleSelects && strchr(p, '|') )
	{
		p = ensureFilesExist( lBuff , p ) ;
	}
	else if ( ! fileExists (p) )
	{
		return NULL ;
	}
	/* printf ( "lBuff3: %s\n" , p ) ; //*/
	return p ;
}


char const * tinyfd_selectFolderDialog (
	char const * const aTitle , /*  NULL or "" */
	char const * const aDefaultPath ) /* NULL or "" */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
	char const * p ;
#ifndef TINYFD_WIN_CONSOLE_ONLY
	if ( ( !tinyfd_forceConsole || !( GetConsoleWindow() || dialogPresent() ) )
	  && ( !getenv("SSH_CLIENT") || getenv("DISPLAY") ) )
	{
		p = selectFolderDialogWinGui(lBuff,aTitle,aDefaultPath);
	}
	else
#endif /* TINYFD_WIN_CONSOLE_ONLY */
	if ( dialogPresent() )
	{
		p = selectFolderDialogWinConsole(lBuff,aTitle,aDefaultPath);
	}
	else
	{
		p = tinyfd_inputBox(aTitle, "Select folder","");
	}
	
	if ( ! p || ! strlen ( p ) || ! dirExists ( p ) )
	{
		return NULL ;
	}
	return p ;
}


/* returns the hexcolor as a string "#FF0000" */
/* aoResultRGB also contains the result */
/* aDefaultRGB is used only if aDefaultHexRGB is NULL */
/* aDefaultRGB and aoResultRGB can be the same array */
char const * tinyfd_colorChooser(
	char const * const aTitle, /* NULL or "" */
	char const * const aDefaultHexRGB, /* NULL or "#FF0000"*/
	unsigned char const aDefaultRGB[3], /* { 0 , 255 , 255 } */
	unsigned char aoResultRGB[3]) /* { 0 , 0 , 0 } */
{
	char lDefaultHexRGB[8];
	char * lpDefaultHexRGB;
	int i;
	char const * p ;

#ifndef TINYFD_WIN_CONSOLE_ONLY
	if ( (!tinyfd_forceConsole || !( GetConsoleWindow() || dialogPresent()) )
	  && (!getenv("SSH_CLIENT") || getenv("DISPLAY")) )
	{
		return colorChooserWinGui(
						aTitle,aDefaultHexRGB,aDefaultRGB,aoResultRGB);
	}
	else
#endif /* TINYFD_WIN_CONSOLE_ONLY */
	if ( aDefaultHexRGB )
	{
		lpDefaultHexRGB = (char *) aDefaultHexRGB ;
	}
	else
	{
		RGB2Hex( aDefaultRGB , lDefaultHexRGB ) ;
		lpDefaultHexRGB = (char *) lDefaultHexRGB ;
	}
	p = tinyfd_inputBox(aTitle,
			"Enter hex rgb color (i.e. #f5ca20)",lpDefaultHexRGB);
	if ( !p || (strlen(p) != 7) || (p[0] != '#') )
	{
		return NULL ;
	}
	for ( i = 1 ; i < 7 ; i ++ )
	{
		if ( ! isxdigit( p[i] ) )
		{
			return NULL ;
		}
	}
	Hex2RGB(p,aoResultRGB);
	return p ;
}

#else /* unix */

static char gPython2Name[16];

							
static int isDarwin ( )
{
	static int lsIsDarwin = -1 ;
	struct utsname lUtsname ;
	if ( lsIsDarwin < 0 )
	{
		lsIsDarwin = uname(&lUtsname) && !strcmp(lUtsname.sysname,"Darwin") ;
	}
	return lsIsDarwin ;
}


static int dirExists ( char const * const aDirPath )
{
	DIR * lDir ;
	if ( ! aDirPath || ! strlen ( aDirPath ) )
		return 0 ;
	lDir = opendir ( aDirPath ) ;
	if ( ! lDir )
	{
		return 0 ;
	}
	closedir ( lDir ) ;
	return 1 ;
}

									
static int detectPresence ( char const * const aExecutable )
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    char lTestedString [ MAX_PATH_OR_CMD ] = "which " ;
	FILE * lIn ;

    strcat ( lTestedString , aExecutable ) ;
    lIn = popen ( lTestedString , "r" ) ;
    if ( ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
        && ( ! strchr ( lBuff , ':' ) ) )
    {	/* present */
    	pclose ( lIn ) ;
    	return 1 ;
    }
    else
    {
    	pclose ( lIn ) ;
    	return 0 ;
    }
}


static int tryCommand ( char const * const aCommand )
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;

    lIn = popen ( aCommand , "r" ) ;
    if ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) == NULL )
    {	/* present */
    	pclose ( lIn ) ;
    	return 1 ;
    }
    else
    {
    	pclose ( lIn ) ;
    	return 0 ;
    }

}


static int whiptailPresent ( )
{
    static int lWhiptailPresent = -1 ;
    if ( lWhiptailPresent < 0 )
    {
        lWhiptailPresent = detectPresence ( "whiptail" ) ;
    }
    return lWhiptailPresent && isatty ( 1 ) ; /* verify console presence */
}


static int dialogPresent ( )
{
    static int lDialogPresent = -1 ;
    if ( lDialogPresent < 0 )
    {
        lDialogPresent = detectPresence("dialog");/*if not tty,not on osxpath*/
    }
	return lDialogPresent && ( !isDarwin() || isatty(1) ) ; /*no xterm on osx*/
}


static int graphicMode()
{
	return !( tinyfd_forceConsole && (isatty(1) || dialogPresent()) )
			&& getenv ( "DISPLAY" );
}


static int xmessagePresent ( )
{
    static int lXmessagePresent = -1 ;
    if ( lXmessagePresent < 0 )
    {
        lXmessagePresent = detectPresence("xmessage") ;
    }
    return lXmessagePresent && graphicMode ( ) ;
}


static int notifysendPresent ( )
{
    static int lNotifysendPresent = -1 ;
    if ( lNotifysendPresent < 0 )
    {
        lNotifysendPresent = detectPresence("notify-send") ;
    }
    return lNotifysendPresent && graphicMode ( ) ;
}


static int xdialogPresent ( )
{
    static int lXdialogPresent = -1 ;
    if ( lXdialogPresent < 0 )
    {
        lXdialogPresent = detectPresence("Xdialog") ;
    }
    return lXdialogPresent && graphicMode ( ) ;
}


static int gdialogPresent ( )
{
    static int lGdialoglPresent = -1 ;
    if ( lGdialoglPresent < 0 )
    {
        lGdialoglPresent = detectPresence ( "gdialog" ) ;
    }
    return lGdialoglPresent && graphicMode ( ) ;
}


static int osascriptPresent ( )
{
    static int lOsascriptPresent = -1 ;
    if ( lOsascriptPresent < 0 )
    {
        lOsascriptPresent = detectPresence ( "osascript" ) ;
    }
	return lOsascriptPresent && graphicMode ( ) ;
}


static int kdialogPresent ( )
{
	static int lKdialogPresent = -1 ;
	if ( lKdialogPresent < 0 )
	{
		lKdialogPresent = detectPresence("kdialog") ;
	}
	return lKdialogPresent && graphicMode ( ) ;
}


static int zenityPresent ( )
{
	static int lZenityPresent = -1 ;
	if ( lZenityPresent < 0 )
	{
		lZenityPresent = detectPresence("zenity") ;
	}
	return lZenityPresent && graphicMode ( ) ;
}


static int zenity3Present ( )
{
    static int lZenity3Present = -1 ;
    char lBuff [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;
	

	if ( lZenity3Present < 0 )
	{
		if ( ! zenityPresent() )
		{
			lZenity3Present = 0 ;
		}
	 	else
		{
			lIn = popen ( "zenity --version" , "r" ) ;
			if ( ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
			  && ( atoi(lBuff) >= 3 )
			  && ( atoi(strtok(lBuff,".")+1) >= 0 ) )
			{
				lZenity3Present = 1 ;
			}
			else
			{
				lZenity3Present = 0 ;
			}
			pclose ( lIn ) ;
		}
	}
    return lZenity3Present && graphicMode ( ) ;
}


static int tkinter2Present ( )
{
    static int lTkinter2Present = -1 ;
	char lPythonCommand[256];
	char lPythonParams[256] =
"-c \"try:\n\timport Tkinter;\nexcept:\n\tprint(0);\"";
	int i;

    if ( lTkinter2Present < 0 )
    {
		strcpy(gPython2Name , "python" ) ;
		sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
	    lTkinter2Present = tryCommand(lPythonCommand);		
        if ( ! lTkinter2Present )
	    {
			strcpy(gPython2Name , "python2" ) ;
			if ( detectPresence(gPython2Name) )
			{
sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
				lTkinter2Present = tryCommand(lPythonCommand);
			}
			else
			{
				for ( i = 9 ; i >= 0 ; i -- )
				{
					sprintf ( gPython2Name , "python2.%d" , i ) ;
					if ( detectPresence(gPython2Name) )
					{
sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
						lTkinter2Present = tryCommand(lPythonCommand);
						break ;
					}
				}
			}
	    }
    }
    /* printf ("gPython2Name %s\n", gPython2Name) ; //*/
    return lTkinter2Present && graphicMode ( ) ;
}


/* returns 0 for cancel/no , 1 for ok/yes */
int tinyfd_messageBox (
    char const * const aTitle , /* NULL or "" */
    char const * const aMessage , /* NULL or ""  may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno"*/
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) /* 0 for cancel/no , 1 for ok/yes */
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;
	int lWasXterm = 0 ;
    int lResult ;
	char aChar ;
    lBuff[0]='\0';

	if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString , "osascript -e 'try' -e 'display dialog \"") ;
	    if ( aMessage && strlen(aMessage) )
	    {
			strcat(lDialogString, aMessage) ;
	    }
		strcat(lDialogString, "\" ") ;
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		strcat(lDialogString, "with icon ") ;
		if ( ! strcmp( "error" , aIconType ) )
		{
			strcat(lDialogString, "stop " ) ;
		}
		else if ( ! strcmp( "warning" , aIconType ) )
		{
			strcat(lDialogString, "caution " ) ;
		}
		else /* question or info */
		{
			strcat(lDialogString, "note " ) ;
		}
		if ( ! strcmp( "okcancel" , aDialogType ) )
		{
			if ( ! aDefaultButton )
			{
				strcat ( lDialogString ,"default button \"Cancel\" " ) ;
			}
		}
		else if ( ! strcmp( "yesno" , aDialogType ) )
		{
			strcat ( lDialogString ,"buttons {\"No\", \"Yes\"} " ) ;
			if (aDefaultButton) 
			{
				strcat ( lDialogString ,"default button \"Yes\" " ) ;
			}
			else
			{
				strcat ( lDialogString ,"default button \"No\" " ) ;
			}
			strcat ( lDialogString ,"cancel button \"No\"" ) ;
		}
		else
		{
			strcat ( lDialogString ,"buttons {\"OK\"} " ) ;
			strcat ( lDialogString ,"default button \"OK\" " ) ;
		}
		strcat(lDialogString, "' ") ;
		strcat(lDialogString, "-e '1' " );
		strcat(lDialogString, "-e 'on error number -128' " ) ;
		strcat(lDialogString, "-e '0' " );
		strcat(lDialogString, "-e 'end try'") ;
	}
    else if ( zenityPresent() )
    {
        strcpy ( lDialogString , "zenity --" ) ;
        if ( ! strcmp( "okcancel" , aDialogType ) )
        {
            strcat ( lDialogString ,
            		"question --ok-label=Ok --cancel-label=Cancel" ) ;
        }
        else if ( ! strcmp( "yesno" , aDialogType ) )
        {
            strcat ( lDialogString , "question" ) ;
        }
        else if ( ! strcmp( "error" , aIconType ) )
		{
            strcat ( lDialogString , "error" ) ;
        }
        else if ( ! strcmp( "warning" , aIconType ) )
		{
            strcat ( lDialogString , "warning" ) ;
        }
        else
		{
            strcat ( lDialogString , "info" ) ;
        }
		if ( aTitle && strlen(aTitle) ) 
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aMessage && strlen(aMessage) ) 
		{
			strcat(lDialogString, " --text=\"") ;
			strcat(lDialogString, aMessage) ;
			strcat(lDialogString, "\"") ;
		}
		if ( zenity3Present ( ) )
		{
			strcat ( lDialogString , " --icon-name=dialog-" ) ;
			if ( ! strcmp( "question" , aIconType )
			  || ! strcmp( "error" , aIconType )
			  || ! strcmp( "warning" , aIconType ) )
			{
				strcat ( lDialogString , aIconType ) ;
			}
			else
			{
				strcat ( lDialogString , "info" ) ;
			}
		}
        strcat ( lDialogString , ";if [ $? = 0 ];then echo 1;else echo 0;fi");
    }
	else if ( kdialogPresent() )
	{
		strcpy ( lDialogString , "kdialog --" ) ;
		if ( ( ! strcmp( "okcancel" , aDialogType ) )
		  || ( ! strcmp( "yesno" , aDialogType ) ) )
		{
			if ( ( ! strcmp( "warning" , aIconType ) )
			  || ( ! strcmp( "error" , aIconType ) ) )
			{
				strcat ( lDialogString , "warning" ) ;
			}
			strcat ( lDialogString , "yesno" ) ;
		}
		else if ( ! strcmp( "error" , aIconType ) )
		{
			strcat ( lDialogString , "error" ) ;
		}
		else if ( ! strcmp( "warning" , aIconType ) )
		{
			strcat ( lDialogString , "sorry" ) ;
		}
		else
		{
			strcat ( lDialogString , "msgbox" ) ;
		}
		strcat ( lDialogString , " \"" ) ;
		if ( aMessage )
		{
			strcat ( lDialogString , aMessage ) ;
		}
		strcat ( lDialogString , "\"" ) ;
		if ( ! strcmp( "okcancel" , aDialogType ) )
		{
			strcat ( lDialogString ,
				" --yes-label Ok --no-label Cancel" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		strcat ( lDialogString , ";if [ $? = 0 ];then echo 1;else echo 0;fi");
	}
    else if ( ! xdialogPresent() && tkinter2Present ( ) )
    {
        strcpy ( lDialogString , gPython2Name ) ;
        if ( ! isatty ( 1 ) && isDarwin ( ) )
        {
           	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
        }
		
		strcat ( lDialogString ,
" -c \"import Tkinter,tkMessageBox;root=Tkinter.Tk();root.withdraw();");
		
		if ( osascriptPresent ( ) )
		{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString ,"res=tkMessageBox." ) ;
        if ( ! strcmp( "okcancel" , aDialogType ) )
        {
            strcat ( lDialogString , "askokcancel(" ) ;
            if ( aDefaultButton )
			{
				strcat ( lDialogString , "default=tkMessageBox.OK," ) ;
			}
			else
			{
				strcat ( lDialogString , "default=tkMessageBox.CANCEL," ) ;
			}
        }
        else if ( ! strcmp( "yesno" , aDialogType ) )
        {
            strcat ( lDialogString , "askyesno(" ) ;
            if ( aDefaultButton )
			{
				strcat ( lDialogString , "default=tkMessageBox.YES," ) ;
			}
			else
			{
				strcat ( lDialogString , "default=tkMessageBox.NO," ) ;
			}
        }
        else
        {
            strcat ( lDialogString , "showinfo(" ) ;
        }
        strcat ( lDialogString , "icon='" ) ;
        if ( ! strcmp( "question" , aIconType )
          || ! strcmp( "error" , aIconType )
          || ! strcmp( "warning" , aIconType ) )
        {
            strcat ( lDialogString , aIconType ) ;
        }
        else
        {
            strcat ( lDialogString , "info" ) ;
        }
		strcat(lDialogString, "',") ;
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
		if ( aMessage && strlen(aMessage) )
		{
			replaceSubStr ( aMessage , "\n" , "\\n" , lBuff ) ;
			strcat(lDialogString, "message='") ;
			strcat(lDialogString, lBuff) ;
			strcat(lDialogString, "'") ;
			lBuff[0]='\0';
		}
		strcat(lDialogString, ");\n\
if res==False :\n\tprint 0\n\
else :\n\tprint 1\n\"" ) ;
    }
	else if (!xdialogPresent() && !gdialogPresent() && notifysendPresent()
			 && !strcmp("ok" , aDialogType) )
	{
		strcpy ( lDialogString , "notify-send \"" ) ;
		strcat(lDialogString, aTitle) ;
		strcat ( lDialogString , " | " ) ;
		strcat(lDialogString, aMessage) ;
		strcat ( lDialogString , "\"" ) ;
	}
	else if (!xdialogPresent() && !gdialogPresent() && xmessagePresent ( )
			 && ! strcmp("ok" , aDialogType) )
	{
		strcpy ( lDialogString , "xmessage -center \"");
		strcat(lDialogString, aTitle) ;
		strcat ( lDialogString , "\n\n" ) ;
		strcat(lDialogString, aMessage) ;
		strcat ( lDialogString , "\"" ) ;
	}
	else if ( xdialogPresent() || gdialogPresent()
		   || dialogPresent() || whiptailPresent() )
	{
		if ( xdialogPresent ( ) )
		{
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( gdialogPresent ( ) )
		{
			strcpy ( lDialogString , "(gdialog " ) ;
		}
		else if ( dialogPresent ( ) )
		{
			if ( ! isatty ( 1 ) )
			{
				lWasXterm = 1 ;
				strcpy ( lDialogString , "xterm -e '(dialog " ) ;
			}
			else
			{
				strcpy ( lDialogString , "(dialog " ) ;
			}
		}
		else
		{
			if ( ! isatty ( 1 ) )
			{
				lWasXterm = 1 ;
				strcpy ( lDialogString , "xterm -e '(whiptail " ) ;
			}
			else
			{
				strcpy ( lDialogString , "(whiptail " ) ;
			}
		}

 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		if ( ! strcmp( "okcancel" , aDialogType ) )
		{
			if ( ! aDefaultButton )
			{
				strcat ( lDialogString , "--defaultno " ) ;
			}
			strcat ( lDialogString ,
					"--yes-label \"Ok\" --no-label \"Cancel\" --yesno " ) ;
		}
		else if ( ! strcmp( "yesno" , aDialogType ) )
		{
			if ( ! aDefaultButton )
			{
				strcat ( lDialogString , "--defaultno " ) ;
			}
			strcat ( lDialogString , "--yesno " ) ;
		}
		else
		{
			strcat ( lDialogString , "--msgbox " ) ;

		}
		strcat ( lDialogString , "\"" ) ;
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, aMessage) ;
		}
		strcat(lDialogString, "\" 10 60 >/dev/tty) 2>&1;if [ $? = 0 ];");
		if ( lWasXterm )
		{
		   strcat ( lDialogString ,
"then echo 1 >/tmp/tinyfd.txt;else echo 0 >/tmp/tinyfd.txt;fi';\
cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
		}
		else
		{
		   strcat(lDialogString, "then echo 1;else echo 0;fi;clear >/dev/tty");
		}
	}
	else if ( isatty ( 1 ) )
	{
		if ( !gWarningDisplayed )
		{
			gWarningDisplayed = 1 ;
			printf ("\n\n%s\n", gTitle);
			printf ("%s\n\n", gMessageUnix);
		}
 		if ( aTitle && strlen(aTitle) )
		{
			printf ("%s", aTitle);
		}
		if ( !strcmp("okcancel",aDialogType) || !strcmp("yesno",aDialogType) )
		{
			do
			{
				if ( aMessage && strlen(aMessage) )
				{
					printf("\n%s (y/n) ",aMessage);
				}
				else
				{
					printf("\n (y/n) ");
				}
				fgets(lBuff, MAX_PATH_OR_CMD, stdin);
				aChar = tolower(lBuff[strlen(lBuff)-2]);
			}
			while ( strlen(lBuff) != 2 || (aChar != 'y' && aChar != 'n') );
			return aChar == 'y' ? 1 : 0 ;
		}
		else
		{
			if ( aMessage && strlen(aMessage) )
			{
				printf ("\n%s\n", aMessage);
			}
			else
			{
				printf("\n");
			}
			return 1 ;
		}
	}
	else
	{
		if ( !gWarningDisplayed )
		{
			gWarningDisplayed = 1 ;
			tinyfd_messageBox ( gTitle , gMessageUnix , "ok", "error" , 1 ) ;
		}
		return 0 ;
	}

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return 0 ;
    }
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	pclose ( lIn ) ;
	/* printf ( "lBuff: %s len: %lu \n" , lBuff , strlen(lBuff) ) ; //*/
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff1: %s len: %lu \n" , lBuff , strlen(lBuff) ) ; //*/
    lResult =  strcmp ( lBuff , "1" ) ? 0 : 1 ;
	/* printf ( "lResult: %d\n" , lResult ) ; //*/
    return lResult ;
}


/* returns NULL on cancel */
char const * tinyfd_inputBox(
	char const * const aTitle , /* NULL or "" */
	char const * const aMessage , /* NULL or "" may NOT contain \n nor \t */
	char const * const aDefaultInput ) /* "" , if NULL it's a passwordBox */
{
	static char lBuff[MAX_PATH_OR_CMD];
	char lDialogString[MAX_PATH_OR_CMD];
	FILE * lIn ;
	int lResult ;
	int lWasGdialog = 0 ;
	int lWasXterm = 0 ;
	struct termios oldt ;
	struct termios newt ;
	lBuff[0]='\0';

    if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString , "osascript -e 'try' -e 'display dialog \"") ;
	    if ( aMessage && strlen(aMessage) )
	    {
			strcat(lDialogString, aMessage) ;
	    }
		strcat(lDialogString, "\" ") ;
		strcat(lDialogString, "default answer \"") ;
		if ( aDefaultInput && strlen(aDefaultInput) )
		{
			strcat(lDialogString, aDefaultInput) ;
		}
		strcat(lDialogString, "\" ") ;
		if ( ! aDefaultInput )
		{
			strcat(lDialogString, "hidden answer true ") ;
		}
		if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		strcat(lDialogString, "with icon note' ") ;
		strcat(lDialogString, "-e '\"1\" & text returned of result' " );
		strcat(lDialogString, "-e 'on error number -128' " ) ;
		strcat(lDialogString, "-e '0' " );
		strcat(lDialogString, "-e 'end try'") ;
	}
	else if ( zenityPresent() )
	{
		strcpy ( lDialogString , "szAnswer=$(zenity --entry" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, " --text=\"") ;
			strcat(lDialogString, aMessage) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultInput )
		{
			if ( strlen(aDefaultInput) )
			{
				strcat(lDialogString, " --entry-text=\"") ;
				strcat(lDialogString, aDefaultInput) ;
				strcat(lDialogString, "\"") ;
			}
		}
		else
		{
			strcat(lDialogString, " --hide-text") ;
		}
		strcat ( lDialogString ,
				");if [ $? = 0 ];then echo 1$szAnswer;else echo 0$szAnswer;fi");
	}
	else if ( kdialogPresent() )
	{
		strcpy ( lDialogString , "szAnswer=$(kdialog" ) ;
		if ( ! aDefaultInput )
		{
			strcat(lDialogString, " --password ") ;
		}
		else
		{
			strcat(lDialogString, " --inputbox ") ;
			
		}
		strcat(lDialogString, "\"") ;
		if ( aMessage && strlen(aMessage) )

		{
			strcat(lDialogString, aMessage ) ;
		}
		strcat(lDialogString , "\" \"" ) ;
		if ( aDefaultInput && strlen(aDefaultInput) )
		{
			strcat(lDialogString, aDefaultInput ) ;
		}
		strcat(lDialogString , "\"" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		strcat ( lDialogString ,
				");if [ $? = 0 ];then echo 1$szAnswer;else echo 0$szAnswer;fi");
	}
	else if ( ! xdialogPresent() && tkinter2Present ( ) )
	{
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ) )
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
		
		strcat ( lDialogString ,
" -c \"import Tkinter,tkSimpleDialog;root=Tkinter.Tk();root.withdraw();");
		
		if ( osascriptPresent ( ) )
		{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}
		
		strcat ( lDialogString ,"res=tkSimpleDialog.askstring(" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
		}
		if ( aMessage && strlen(aMessage) )
		{
			replaceSubStr ( aMessage , "\n" , "\\n" , lBuff ) ;
			strcat(lDialogString, "prompt='") ;
			strcat(lDialogString, lBuff) ;
			strcat(lDialogString, "',") ;
			lBuff[0]='\0';
		}
		if ( aDefaultInput )
		{
			if ( strlen(aDefaultInput) )
			{
				strcat(lDialogString, "initialvalue='") ;
				strcat(lDialogString, aDefaultInput) ;
				strcat(lDialogString, "',") ;
			}
		}
		else
		{
			strcat(lDialogString, "show='*'") ;
		}
		strcat(lDialogString, ");\nif res is None :\n\tprint 0");
		strcat(lDialogString, "\nelse :\n\tprint '1'+res\n\"" ) ;
	}
	else if ( xdialogPresent() || gdialogPresent()
		   || dialogPresent() || whiptailPresent() )
	{
		if ( xdialogPresent ( ) )
		{
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( gdialogPresent ( ) )
		{
			lWasGdialog = 1 ;
			strcpy ( lDialogString , "(gdialog " ) ;
		}
		else if ( dialogPresent ( ) )
		{
			if ( ! isatty ( 1 ) )
			{
				lWasXterm = 1 ;
				strcpy ( lDialogString , "xterm -e '(dialog " ) ;
			}
			else
			{
				strcpy ( lDialogString , "(dialog " ) ;
			}
		}
		else
		{
			if ( ! isatty ( 1 ) )
			{
				lWasXterm = 1 ;
				strcpy ( lDialogString , "xterm -e '(whiptail " ) ;
			}
			else
			{
				strcpy ( lDialogString , "(whiptail " ) ;
			}
		}

		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		if ( aDefaultInput || lWasGdialog )
		{
			strcat ( lDialogString , "--inputbox" ) ;
		}
		else
		{
			strcat ( lDialogString , "--passwordbox" ) ;
		}
		strcat ( lDialogString , " \"" ) ;
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, aMessage) ;
		}
		strcat(lDialogString,"\" 10 60 ") ;
		if ( aDefaultInput && strlen(aDefaultInput) )
		{
			strcat(lDialogString, "\"") ;
			strcat(lDialogString, aDefaultInput) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat(lDialogString,">/dev/tty ) 2>/tmp/tinyfd.txt;\
if [ $? = 0 ];then tinyfdBool=1;else tinyfdBool=0;fi;\
tinyfdRes=$(cat /tmp/tinyfd.txt);\
rm /tmp/tinyfd.txt;echo $tinyfdBool$tinyfdRes") ;
		if ( lWasXterm )
		{
			strcat ( lDialogString ,
				" >/tmp/tinyfd0.txt';cat /tmp/tinyfd0.txt;rm /tmp/tinyfd0.txt");
		}
		else
		{
			strcat(lDialogString, "; clear >/dev/tty") ;
		}
	}
	else if ( isatty ( 1 ) )
	{
		if ( !gWarningDisplayed )
		{
			gWarningDisplayed = 1 ;
			printf ("\n\n%s\n", gTitle);
			printf ("%s\n\n", gMessageUnix);
		}
		if ( aTitle && strlen(aTitle) )
		{
			printf ("%s\n", aTitle);
		}
		if ( aMessage && strlen(aMessage) )
		{
			printf("%s",aMessage);
		}
		printf(" (esc+enter to cancel): ");
		if ( ! aDefaultInput )
		{
			tcgetattr(STDIN_FILENO, & oldt) ;
			newt = oldt ;
			newt.c_lflag &= ~ECHO ;
			tcsetattr(STDIN_FILENO, TCSANOW, & newt);
		}
		fgets(lBuff, MAX_PATH_OR_CMD, stdin);
		if ( ! aDefaultInput )
		{
			tcsetattr(STDIN_FILENO, TCSANOW, & oldt);
		}
		if ( strchr(lBuff,27) )
		{
			return NULL ;
		}
		if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
		{
			lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
		}
		return lBuff ;
	}
	else
	{
		if ( !gWarningDisplayed )
		{
			gWarningDisplayed = 1 ;
			tinyfd_messageBox ( gTitle , gMessageUnix , "ok", "error" , 1 ) ;
		}
		return NULL ;
	}

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
	{
		return NULL ;
	}
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	pclose ( lIn ) ;
	/* printf ( "len Buff: %lu\n" , strlen(lBuff) ) ; //*/
	/* printf ( "lBuff0: %s\n" , lBuff ) ; //*/
	if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
	{
		lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
	}
	/* printf ( "lBuff1: %s len: %lu \n" , lBuff , strlen(lBuff) ) ; //*/
    lResult =  strncmp ( lBuff , "1" , 1) ? 0 : 1 ;
	/* printf ( "lResult: %d \n" , lResult ) ; //*/
    if ( ! lResult )
    {
		return NULL ;
	}
	/* printf ( "lBuff+1: %s\n" , lBuff+1 ) ; //*/
	return lBuff+1 ;
}


char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) /* NULL or {"*.jpg","*.png"} */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    char lString [ MAX_PATH_OR_CMD ] ;
	int i ;
	int lWasXterm = 0 ;
	char const * p ;
    DIR * lDir ;
    FILE * lIn ;
	lBuff[0]='\0';

	if ( osascriptPresent ( ) )
	{
		strcpy ( lDialogString ,
				"osascript -e 'POSIX path of ( choose file name " );
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		getLastName ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default name \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		strcat ( lDialogString , ")'" ) ;
	}
    else if ( zenityPresent() )
    {
		strcpy ( lDialogString ,
				"zenity --file-selection --save --confirm-overwrite" ) ;
		if ( aTitle && strlen(aTitle) ) 
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) ) 
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPathAndFile) ;
			strcat(lDialogString, "\"") ;
		}		
		if ( aNumOfFileFilters > 0 )
		{
			strcat ( lDialogString , " --file-filter='" ) ;
			for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , aFileFilters [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			strcat ( lDialogString , "' --file-filter='All files | *'" ) ;
		}
    }
    else if ( kdialogPresent() )
    {
		strcpy ( lDialogString , "kdialog --getsavefilename" ) ;
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPathAndFile ) ;
			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
	    if ( aNumOfFileFilters > 0 )
	    {
			strcat(lDialogString , " \"" ) ;
			for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , aFileFilters [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			strcat ( lDialogString , "\"" ) ;
	    }
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
	    }
    }
    else if ( ! xdialogPresent() && tkinter2Present ( ) )
    {

		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ))
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
	    strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( osascriptPresent ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set\
 frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "print tkFileDialog.asksaveasfilename(");
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
		}
	    if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
	    {
			getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
			getLastName ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialfile='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
		}
	    if ( ( aNumOfFileFilters > 1 )
		  || ( (aNumOfFileFilters == 1) /* test because poor osx behaviour */
	            && ( aFileFilters[0][strlen(aFileFilters[0])-1] != '*' ) ) )
	    {
			strcat(lDialogString , "filetypes=(" ) ;
	        for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
	        {
	            strcat ( lDialogString , "('','" ) ;
	            strcat ( lDialogString , aFileFilters [ i ] ) ;
	            strcat ( lDialogString , "')," ) ;
	        }
	        strcat ( lDialogString , "('All files','*'))" ) ;
	    }
		strcat ( lDialogString , ")\"" ) ;
	}
	else if ( xdialogPresent() || dialogPresent() )
	{
		if ( xdialogPresent ( ) )
		{
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( ! isatty ( 1 ) )
		{
			lWasXterm = 1 ;
			strcpy ( lDialogString , "xterm -e '(dialog " ) ;
		}
		else
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}

 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--fselect \"" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			if ( ! strchr(aDefaultPathAndFile, '/') )
			{
				strcat(lDialogString, "./") ;
			}
			strcat(lDialogString, aDefaultPathAndFile) ;
		}
		strcat(lDialogString, "\" 0 60  >/dev/tty) ") ;
		if ( lWasXterm )
		{
			strcat ( lDialogString ,
				"2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
		}
		else
		{
			strcat(lDialogString, "2>&1 ; clear >/dev/tty") ;
		}
	}
	else
	{
		p = tinyfd_inputBox ( aTitle , "Save file" , "" ) ;
		getPathWithoutFinalSlash ( lString , p ) ;
		if ( strlen ( lString ) && ! dirExists ( lString ) )
		{
			return NULL ;
		}
		getLastName(lString,p);
		if ( ! strlen(lString) )
		{
			return NULL;
		}
		return p ;
	}

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
    while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
    {}
    pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( ! strlen(lBuff) )
	{
		return NULL;
	}
    getPathWithoutFinalSlash ( lString , lBuff ) ;
    if ( strlen ( lString ) && ! dirExists ( lString ) )
    {
        return NULL ;
    }
	getLastName(lString,lBuff);
	if ( ! filenameValid(lString) )
	{
		return NULL;
	}
    return lBuff ;
}

                 
/* in case of multiple files, the separator is | */
char const * tinyfd_openFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
    static char lBuff [ MAX_MULTIPLE*MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
	char lString [ MAX_PATH_OR_CMD ] ;
	int i ;
    FILE * lIn ;
	char * p ;
	char const * p2 ;
	int lWasKdialog = 0 ;
	int lWasXterm = 0 ;
    lBuff[0]='\0';

    if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString , "osascript -e '" );
	    if ( ! aAllowMultipleSelects )
	    {
			strcat ( lDialogString , "POSIX path of ( " );
		}
		else
		{
			strcat ( lDialogString , "set mylist to " );
		}
		strcat ( lDialogString , "choose file " );
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		if ( aNumOfFileFilters > 0 )
		{
			strcat(lDialogString , "of type {\"" );
			strcat ( lDialogString , aFileFilters [ 0 ] + 2 ) ;
			strcat ( lDialogString , "\"" ) ;
			for ( i = 1 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , ",\"" ) ;
				strcat ( lDialogString , aFileFilters [ i ] + 2) ;
				strcat ( lDialogString , "\"" ) ;
			}
			strcat ( lDialogString , "} " ) ;
		}
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , "multiple selections allowed true ' " ) ;
			strcat ( lDialogString ,
					"-e 'set mystring to POSIX path of item 1 of mylist' " );
			strcat ( lDialogString ,
					"-e 'repeat with  i from 2 to the count of mylist' " );
			strcat ( lDialogString , "-e 'set mystring to mystring & \"|\"' " );
			strcat ( lDialogString ,
			"-e 'set mystring to mystring & POSIX path of item i of mylist' " );
			strcat ( lDialogString , "-e 'end repeat' " );
			strcat ( lDialogString , "-e 'mystring'" );
		}
		else
		{
			strcat ( lDialogString , ")'" ) ;
		}
    }
    else if ( zenityPresent() )
    {
        strcpy ( lDialogString ,"zenity --file-selection" ) ;
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , " --multiple" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPathAndFile) ;
			strcat(lDialogString, "\"") ;
		}
        if ( aNumOfFileFilters > 0 )
        {
	        strcat ( lDialogString , " --file-filter='" ) ; 
            for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
            {
                strcat ( lDialogString , aFileFilters [ i ] ) ;
                strcat ( lDialogString , " " ) ;
            }
 	        strcat ( lDialogString , "' --file-filter='All files | *'" ) ;
        }
    }
	else if ( kdialogPresent() )
	{
		lWasKdialog = 1 ;
		strcpy ( lDialogString , "kdialog --getopenfilename" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPathAndFile ) ;

			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
		if ( aNumOfFileFilters > 0 )
		{
			strcat(lDialogString , " \"" ) ;
			for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , aFileFilters [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			strcat ( lDialogString , "\"" ) ;
		}
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , " --multiple --separate-output" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
    else if ( ! xdialogPresent() && tkinter2Present ( ) )
    {
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ) )
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
        strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( osascriptPresent ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}
		strcat ( lDialogString , "lFiles=tkFileDialog.askopenfilename(");
        if ( aAllowMultipleSelects )
        {
	        strcat ( lDialogString , "multiple=1," ) ;
        }
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
			getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
			getLastName ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialfile='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
		}
        if ( ( aNumOfFileFilters > 1 )
          || ( ( aNumOfFileFilters == 1 ) /* test because poor osx behaviour */
                && ( aFileFilters[0][strlen(aFileFilters[0])-1] != '*' ) ) )
        {
			strcat(lDialogString , "filetypes=(" ) ;
            for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
            {
                strcat ( lDialogString , "('','" ) ;
                strcat ( lDialogString , aFileFilters [ i ] ) ;
                strcat ( lDialogString , "')," ) ;
            }
            strcat ( lDialogString , "('All files','*'))" ) ;
        }
		strcat ( lDialogString , ");\
\nif not isinstance(lFiles, tuple):\n\tprint lFiles\nelse:\
\n\tlFilesString=''\n\tfor lFile in lFiles:\n\t\tlFilesString+=str(lFile)+'|'\
\n\tprint lFilesString[:-1]\n\"" ) ;
    }
	else if ( xdialogPresent() || dialogPresent() )
	{
		if ( xdialogPresent ( ) )
		{
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( ! isatty ( 1 ) )
		{
			lWasXterm = 1 ;
			strcpy ( lDialogString , "xterm -e '(dialog " ) ;
		}
		else
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}

		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--fselect \"" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			if ( ! strchr(aDefaultPathAndFile, '/') )
			{
				strcat(lDialogString, "./") ;
			}
			strcat(lDialogString, aDefaultPathAndFile) ;
		}
		strcat(lDialogString, "\" 0 60  >/dev/tty) ") ;
		if ( lWasXterm )
		{
			strcat ( lDialogString ,
				"2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
		}
		else
		{
			strcat(lDialogString, "2>&1 ; clear >/dev/tty") ;
		}
	}
	else
	{
		p2 = tinyfd_inputBox(aTitle, "Open file","");
		if ( ! fileExists (p2) )
		{
			return NULL ;
		}
		return p2 ;
	}

    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	lBuff[0]='\0';
	p=lBuff;
	while ( fgets ( p , sizeof ( lBuff ) , lIn ) != NULL )
	{
		p += strlen ( p );
	}
    pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
    /* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( lWasKdialog && aAllowMultipleSelects )
	{
		p = lBuff ;
		while ( ( p = strchr ( p , '\n' ) ) )
			* p = '|' ;
	}
	/* printf ( "lBuff2: %s\n" , lBuff ) ; //*/
	if ( ! strlen ( lBuff )  )
	{
		return NULL;
	}
	if ( aAllowMultipleSelects && strchr(lBuff, '|') )
	{
		p2 = ensureFilesExist( lBuff , lBuff ) ;
	}
	else if ( fileExists (lBuff) )
	{
		p2 = lBuff ;
	}
	else
	{
		return NULL ;
	}
	/* printf ( "lBuff3: %s\n" , p2 ) ; //*/
	return p2 ;
}


char const * tinyfd_selectFolderDialog (
	char const * const aTitle , /* "" */
	char const * const aDefaultPath ) /* "" */
{
	static char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    DIR * lDir ;
	FILE * lIn ;
	char const * p ;
	int lWasXterm = 0 ;
    lBuff[0]='\0';

    if ( osascriptPresent ( ))
    {
		strcpy(lDialogString ,"osascript -e 'POSIX path of ( choose folder ");
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, aDefaultPath ) ;
			strcat(lDialogString , "\" " ) ;
		}
		strcat ( lDialogString , ")'" ) ;
    }
	else if ( zenityPresent() )
	{
        strcpy ( lDialogString , "zenity --file-selection --directory" ) ;
		if ( aTitle && strlen(aTitle) ) 
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPath && strlen(aDefaultPath) ) 
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPath) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( kdialogPresent() )
	{
		strcpy ( lDialogString , "kdialog --getexistingdirectory" ) ;
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPath ) ;
			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( ! xdialogPresent() && tkinter2Present ( ) )
	{
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ) )
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
        strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( osascriptPresent ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "print tkFileDialog.askdirectory(");
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
        if ( aDefaultPath && strlen(aDefaultPath) )
        {
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, aDefaultPath ) ;
				strcat(lDialogString , "'" ) ;
		}
		strcat ( lDialogString , ")\"" ) ;
	}
	else if ( xdialogPresent() || dialogPresent() )
	{
		if ( xdialogPresent ( ) )
		{
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( ! isatty ( 1 ) )
		{
			lWasXterm = 1 ;
			strcpy ( lDialogString , "xterm -e '(dialog " ) ;
		}
		else
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}

		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--dselect \"" ) ;
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, aDefaultPath) ;
			ensureFinalSlash(lDialogString);
		}
		if ( lWasXterm )
		{
			strcat ( lDialogString ,
				"2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
		}
		else
		{
			strcat(lDialogString, "2>&1 ; clear >/dev/tty") ;
		}
	}
	else
	{
		p = tinyfd_inputBox(aTitle, "Select folder","");
		if ( !p || ! strlen ( p ) || ! dirExists ( p ) )
		{
			return NULL ;
		}
		return p ;
	}
    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( ! strlen ( lBuff ) || ! dirExists ( lBuff ) )
	{
		return NULL ;
	}
	return lBuff ;
}


/* returns the hexcolor as a string "#FF0000" */
/* aoResultRGB also contains the result */
/* aDefaultRGB is used only if aDefaultHexRGB is NULL */
/* aDefaultRGB and aoResultRGB can be the same array */
char const * tinyfd_colorChooser(
	char const * const aTitle , /* NULL or "" */
	char const * const aDefaultHexRGB , /* NULL or "#FF0000"*/
	unsigned char const aDefaultRGB[3] , /* { 0 , 255 , 255 } */
	unsigned char aoResultRGB[3] ) /* { 0 , 0 , 0 } */
{
	static char lBuff [ 16 ] ;
	char lTmp [ 16 ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
	char lDefaultHexRGB[8];
	char * lpDefaultHexRGB;
	unsigned char lDefaultRGB[3];
	char const * p;
 	FILE * lIn ;
	int i ;
	int lWasZenity3 = 0 ;
	int lWasOsascript = 0 ;
	int lWasXdialog = 0 ;
	lBuff[0]='\0';

	if ( aDefaultHexRGB )
	{
		Hex2RGB ( aDefaultHexRGB , lDefaultRGB ) ;
		lpDefaultHexRGB = (char *) aDefaultHexRGB ;
	}
	else
	{
		lDefaultRGB[0]=aDefaultRGB[0];
		lDefaultRGB[1]=aDefaultRGB[1];
		lDefaultRGB[2]=aDefaultRGB[2];
		RGB2Hex( aDefaultRGB , lDefaultHexRGB ) ;
		lpDefaultHexRGB = (char *) lDefaultHexRGB ;
	}

	if ( osascriptPresent ( ) )
    {
    	lWasOsascript = 1 ;
		strcpy ( lDialogString , "osascript -e 'tell app (path to frontmost \
application as Unicode text) to set mycolor to choose color default color {");
		sprintf(lTmp, "%d", 256 * lDefaultRGB[0] ) ;
		strcat(lDialogString, lTmp ) ;
		strcat(lDialogString, "," ) ;
		sprintf(lTmp, "%d", 256 * lDefaultRGB[1] ) ;
		strcat(lDialogString, lTmp ) ;
		strcat(lDialogString, "," ) ;
		sprintf(lTmp, "%d", 256 * lDefaultRGB[2] ) ;
		strcat(lDialogString, lTmp ) ;
		strcat(lDialogString, "}' " ) ;
		strcat ( lDialogString ,
"-e 'set mystring to ((item 1 of mycolor)/256 as integer) as string' " );
		strcat ( lDialogString ,
"-e 'repeat with i from 2 to the count of mycolor' " );
		strcat ( lDialogString ,
"-e 'set mystring to mystring & \" \" & \
((item i of mycolor)/256 as integer) as string' " );
		strcat ( lDialogString , "-e 'end repeat' " );
		strcat ( lDialogString , "-e 'mystring'");
    }
    else if ( zenity3Present() )
	{
		lWasZenity3 = 1 ;
        sprintf ( lDialogString ,
"zenity --color-selection --show-palette --color=%s" , lpDefaultHexRGB ) ;
		if ( aTitle && strlen(aTitle) ) 
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( kdialogPresent() )
	{
        sprintf ( lDialogString ,
"kdialog --getcolor --default '%s'" , lpDefaultHexRGB ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( xdialogPresent() )
	{
		lWasXdialog = 1 ;
		strcpy ( lDialogString , "Xdialog --colorsel \"" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, aTitle) ;
		}
		strcat(lDialogString, "\" 0 60 ") ;
		sprintf(lTmp,"%hhu %hhu %hhu",lDefaultRGB[0],
				lDefaultRGB[1],lDefaultRGB[2]);
		strcat(lDialogString, lTmp) ;
		strcat(lDialogString, " 2>&1");
	}
	else if ( tkinter2Present ( ) )
	{
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ) )
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
		
		strcat ( lDialogString ,
" -c \"import Tkinter,tkColorChooser;root=Tkinter.Tk();root.withdraw();");

		if ( osascriptPresent ( ) )
		{
			strcat ( lDialogString ,
"import os;os.system('''osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "res=tkColorChooser.askcolor(color='" ) ;
		strcat(lDialogString, lpDefaultHexRGB ) ;
		strcat(lDialogString, "'") ;
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, ",title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "'") ;
	    }
		strcat ( lDialogString , ");\
\nif res[1] is not None:\n\tprint res[1]\"" ) ;
	}
	else
	{
		p = tinyfd_inputBox(aTitle,
				"Enter hex rgb color (i.e. #f5ca20)",lpDefaultHexRGB);
		if ( !p || (strlen(p) != 7) || (p[0] != '#') )
		{
			return NULL ;
		}
		for ( i = 1 ; i < 7 ; i ++ )
		{
			if ( ! isxdigit( p[i] ) )
			{
				return NULL ;
			}
		}
		Hex2RGB(p,aoResultRGB);
		return p ;
	}

    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{
	}
	pclose ( lIn ) ;
    if ( ! strlen ( lBuff ) )
    {
        return NULL ;
    }
	/* printf ( "len Buff: %lu\n" , strlen(lBuff) ) ; //*/
	/* printf ( "lBuff0: %s\n" , lBuff ) ; //*/
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
    if ( lWasZenity3 )
    {
        lBuff[3]=lBuff[5];
        lBuff[4]=lBuff[6];
        lBuff[5]=lBuff[9];
        lBuff[6]=lBuff[10];
        lBuff[7]='\0';
        Hex2RGB(lBuff,aoResultRGB);
    }
    else if ( lWasOsascript || lWasXdialog )
    {
    	sscanf(lBuff,"%hhu %hhu %hhu",
			   & aoResultRGB[0], & aoResultRGB[1],& aoResultRGB[2]);
    	RGB2Hex(aoResultRGB,lBuff);
    }
    else
    {
		Hex2RGB(lBuff,aoResultRGB);
	}
	/* printf("%d %d %d\n", aoResultRGB[0],aoResultRGB[1],aoResultRGB[2]); //*/
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	return lBuff ;
}


#endif /* _WIN32 */

#pragma warning(default:4996)

/*
int main()
{
  tinyfd_forceConsole = tinyfd_messageBox("Hello World",
    "force dialogs into console mode?\n\t(it's better if dialog is installed)",
    "yesno", "question", 0);

  char const * lThePassword =  tinyfd_inputBox(
    "a password box","your password will be revealed",NULL);

  if ( lThePassword )
	tinyfd_messageBox("your password is", lThePassword, "ok", "info", 1);
}
//*/

/*
on OSX :
$ gcc -o main.app main.c tinyfiledialogs.c
 
on UNIX :
$ gcc -o main main.c tinyfiledialogs.c
 
on WINDOWS just create a console application project with visual studio.
*/
