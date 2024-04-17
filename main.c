/***************************************************
*  Programm zum Senden von IDOK an alle Fenster,   *
*  die einen angegebenen Text entalten             *
****************************************************
* Änderungen *                                     *
****************************************************
* 28.03.2024 *  aN * .05 * bei PostMessage Parameter WM_COMMAND ergänzt
* 17.04.2024 *  aN * .11 * ignore case, vollständige Hilfe
* **************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "..\\tiny-regex-c\\re.h"

//** Prototypen ***********************************/
int StrInStr(char *sub, char *str);
int RegEx(char *sub, char *str);
int CloseWindowByText(char *suche);

//** Variablen ************************************/
int (*compare)(char *sub,char *str);
char wie_t[] = "text";
char wie_r[] = "regex";
char *wie = wie_t;
char suchtit[] = "~S~e~a~r~c~h~";
HWND selbst = NULL;
int ignore_case = 0;
int dummy = 0;
char titel[500];

//*************************************************/

/*
 Suche String in String
*/
int StrInStr(char *sub, char *str)
{
    char *pos = strstr(sub,str);
    return (pos != NULL);
}

/*
 regular expression
*/
int RegEx(char *sub, char *str)
{
    int l;
    int res = re_match(str, sub, &l);
    return (res != -1);
}
/*
  Schließen eines beliebigen Fensters
 */
int CloseWindowByText(char *txt)
{
    HWND hwnd;
    int treffer = 0;
    static char hStr[500];
    static char vg_suche[500];
    static char vg_titel[500];
    
    printf("Suche (%s): %s\n",wie,txt);
    
    strcpy(vg_suche, txt);
    if(ignore_case)
    {
        _strlwr(vg_suche);
    }

    hwnd = FindWindow(NULL, NULL);
    
    while(hwnd != NULL)
    {
        // - Fenster auswerten
        GetWindowText(hwnd, hStr, sizeof(hStr));
        
        strcpy(vg_titel, hStr);
        if(ignore_case)
        {
            _strlwr(vg_titel);
        }

        if (selbst != hwnd)
        {
            if ((*compare)(vg_titel, vg_suche) != 0)
            {
                printf("gefunden: %s\n", hStr);
                if(dummy == 0)
                {
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
                treffer++;
            }
        }
        else
        {
            // printf("geigenes Fenster: %s\n", hStr);
        }
        hwnd = GetWindow(hwnd,GW_HWNDNEXT);
    }
    printf("Treffer: %d\n", treffer);
    return treffer;
}

int main(int argc, char *argv[])
{
    selbst = GetWindow(GetConsoleWindow(),GW_OWNER);
    char titel[350];
    char *srch = NULL;
    int treffer_ges = 0;
   
    compare = &StrInStr;

    GetConsoleTitle(titel, sizeof(titel));

    SetConsoleTitle(suchtit);
    Sleep(100);
    
    for(int i=1;i<argc;i++)
    {
        if (strcmp(argv[i],"-r")==0)
        {
            compare = &RegEx;
            wie = wie_r;
        }
        else if (strcmp(argv[i],"-t")==0)
        {
            compare = &StrInStr;
            wie = wie_t;
        }
        else if (strcmp(argv[i],"-i")==0)
        {
            ignore_case = 1;
        }
        else if (strcmp(argv[i],"-d")==0)
        {
            dummy = 1;
        }
        else
        {
            srch = argv[i];
            treffer_ges += CloseWindowByText(srch);
        }
    }

    if (srch != NULL)
    {
        printf("WM_CLOSE zu %d Fenster(n) gesendet\n", treffer_ges);
    }
    else
    {
        printf("Verwendung:\nCloseWnd [[-r][-t][-i][-d] <Suchtext>] ...\n"
            "    -r   ... regular Expression\n"
            "    -t   ... Text direkt\n"
            "    -i   ... ignoriere Groß-/Kleinschreibung\n"
            "    -d   ... dummy - CloseWnd wird nicht gesendet\n"
            "%%ERRORLEVEL%% ist Anzahl Treffer\n");
    }
    
    SetConsoleTitle(titel);

    return treffer_ges;
}
