/* ******************************************************************** **
** @@ FBBT - FB2 book title renamer
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Replace file name with <book-title> tag
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include <stdafx.h>

#include "..\shared\mmf.h"
#include "..\shared\file.h"
#include "..\shared\file_walker.h"
#include "..\shared\search_bmh.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif

#define BEGIN_OF_TITLE        "book-title>"
#define END_OF_TITLE          "<"

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD   dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */
                  
static MMF        MF;
                  
/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ ForEach()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void ForEach(const char* const pszFileName)
{
   if (!MF.OpenReadOnly(pszFileName))
   {
      // Error !
      return;
   }

   BYTE*    pText  = MF.Buffer();
   DWORD    dwSize = MF.Size();

   int   iStart = BMH_ISearch(pText,dwSize,(BYTE*)BEGIN_OF_TITLE,sizeof(BEGIN_OF_TITLE) - 1);

   if (iStart == -1)
   {
      MF.Close();
      return;
   }

   iStart += sizeof(BEGIN_OF_TITLE) - 1;

   int   iStop = BMH_ISearch(pText + iStart,dwSize - iStart,(BYTE*)END_OF_TITLE,sizeof(END_OF_TITLE) - 1);

   char     pszTitle[MAX_PATH + 1];

   memset(pszTitle,0,sizeof(pszTitle));

   strncpy(pszTitle,(char*)(pText + iStart),iStop);
   pszTitle[iStop] = 0;    // Ensure ASCIIZ

   MF.Close();

   char     pszNewName[_MAX_PATH];
   char     pszDrive  [_MAX_DRIVE];
   char     pszDir    [_MAX_DIR];
   char     pszFName  [_MAX_FNAME];
   char     pszExt    [_MAX_EXT];

   _splitpath(pszFileName,pszDrive,pszDir,pszFName,pszExt);
   _makepath( pszNewName, pszDrive,pszDir,pszTitle,pszExt);

   rename(pszFileName,pszNewName);
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void ShowHelp()
{
   const char  pszCopyright[] = "-*-   FBBT 1.0   *   Copyright (c) Gazlan, 2014   -*-";
   const char  pszDescript [] = "Replace file name with <book-title> tag";
   const char  pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: fbbt.com wildcards\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char**argv)
{
   if (argc < 2)
   {
      ShowHelp();
      return 0;
   }

   if (argc == 2)
   {
      if ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h")))
      {
         ShowHelp();
         return 0;
      }
   }

   char  pszMask[MAX_PATH + 1];

   strncpy(pszMask,argv[1],MAX_PATH);
   pszMask[MAX_PATH] = 0; // ASCIIZ

   char     pszDrive   [_MAX_DRIVE];
   char     pszDir     [_MAX_DIR];
   char     pszFName   [_MAX_FNAME];
   char     pszExt     [_MAX_EXT];

   _splitpath(pszMask,pszDrive,pszDir,pszFName,pszExt);

   char     pszSrchMask[MAX_PATH + 1];
   char     pszSrchPath[MAX_PATH + 1];

   strcpy(pszSrchMask,pszFName);
   strcat(pszSrchMask,pszExt);

   Walker      Visitor;

   Visitor.Init(ForEach,pszSrchMask,false);

   strcpy(pszSrchPath,pszDrive);
   strcat(pszSrchPath,pszDir);

   Visitor.Run(*pszSrchPath  ?  pszSrchPath  :  ".");

   return 0;
}

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
