/*
 * Soubor:  proj1.c
 * Datum:   oktober 2017
 * Autor:   Pavel Podluzansky, xpodlu01@stud.fit.vutbr.cz
 * Projekt: Praca s textom
 * Popis:   Program pracuje s databazou adries, z ktorej nacita adresy
 *          do dvojrozmerneho pola, z ktoreho ich vyhlada a vypise, ci
 *          adresu naslo, nenaslo alebo povoli nasledujuci znak adresy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define max 100
#define maxMesto 42

unsigned int dlzka,i=0,j=0;
char povolene[max],mesto[maxMesto][max];

/* funkcne prototypy */
void nacitaj_pole(); // funkcia nacita adresy do pola
void zorad_pole();    // funkcia triedi pole podla abecedy
void bez_argumentov(); // funkcia bez argumentov zobrazi prve znaky jednotlivych adries bez opakovania
void jeden_argument();  // funkcia pracuje s argumentom, bud vypise najdeny znak, adresu, alebo adresu nenajde
void porovnaj(char *argv[]); // funkcia porovnava argument s adresami
void velke_pismena_argv(char *argv[]); // funkcia zmeni argument na velke pismena
int odstran_duplikaty(char *str); // funkcia odstrani duplikaty znakov

int main(int argc,char *argv[])
{
    nacitaj_pole();
    if (i>maxMesto) // v pripade, ze je adries viac ako maximalna velkost pola vypise error
    {
        fprintf(stderr,"prekroceny limit adries\n");
        return 0;
    }
    zorad_pole();

    if(argc == 1)
        bez_argumentov();

    if(argc == 2)
    {
        velke_pismena_argv(argv);
        porovnaj(argv);
        jeden_argument();
    }

    if(argc> 2)
        fprintf(stderr,"Prekroceny limit argumentov\n");

    return 0;
}

 /*  Funkcia nacitaj_pole nacita data z textoveho
  *  suboru a ulozi ich do pola, pricom prerobi
  *  kazde pismeno na velke pre lahsie porovnanie
  */

void nacitaj_pole()
 {
    while(fgets(mesto[i],max,stdin))
    {
        for(j=0;j<strlen(mesto[i]);j++)
            {
                mesto[i][j] = toupper(mesto[i][j]);

            }
        i++;
    }
    dlzka=i; // dlzka je pomocna premenna, v ktorej je zapisana dlzka nacitaneho retazca
}
 // Funkcia zoradi pole podla abecedy
void zorad_pole()
{
    char pom[max]; // pomocna premenna pri zoradovani
    for(i=0 ; i < dlzka; i++)
    {
        for(j=i+1 ; j < dlzka; j++)
        {
            if(strcmp(mesto[i],mesto[j])>0)
            {
                strcpy(pom,mesto[j]);
                strcpy(mesto[j],mesto[i]);
                strcpy(mesto[i],pom);
            }
        }
    }
}

/*  Funkcia vypisuje povolene znaky bez zadania argumentu,
 *  vypise zaciatocne pismena povolenych adries a vypise ich
 *  prave raz.
 */

void bez_argumentov()
{   char pov[maxMesto]; // pomocna premenna pri triedeni duplicit
    unsigned int povLen = 0; // pomocna premenna pri triedeni duplicit
    printf("ENABLE: ");
    for(i=0;i<dlzka;i++)
    {
        int duplicita =  1;
        for(j=0;j<povLen + 1;j++)
        {
            if(pov[j] == mesto[i][0])
            {
                duplicita = 0;
            }
        }
        if(duplicita == 1)
        {
            pov[povLen] = mesto[i][0];
            povLen++;
        }

    }
    printf("%s\n",pov);
}
 /* Funkcia toupper_argv urobi zo vsetkych pismen
  * argumentu velke pismena pre lahsie porovnanie.
  */
void velke_pismena_argv(char *argv[])
{
    i=0;
    while(i<strlen(argv[1]))
        {
            argv[1][i] = toupper(argv[1][i]);
            i++;
        }
}

 /* Funkcia compare porovnava zadany argument s adresami,
  * ulozenymi v poli.
  */

void porovnaj(char *argv[])
{   int u,por;
    j=0;
    for(i=0;i<dlzka;i++)
        {
            por=(strncmp(argv[1],mesto[i],strlen(argv[1])));
            if(por == 0)
                {
                    char p;
                    p=mesto[i][strlen(argv[1])];
                    povolene[j]=povolene[j]+p;
                    j++;
                    u=i;
                }
                }
    if(j==1)
    {
       strcpy(povolene,mesto[u]);
    }
}

 /*  Funkcia vykonava hladanie zadanej adresy ak najde prave
  *  jednu moznost, vypise hladanu adresu, ak ma na vyber,
  *  s viacerych moznosti vypise povolene nasledujuce znaky,
  *  a ak nenajde zadanu adresu vypise "NOT FOUND"
  */

void jeden_argument()
{
    if(j == 1) // Ak podmienka platí, program vypise adresu ktora vyhovuje argumentu, v pripade ze bola najdena iba jedna adresa
        printf("FOUND: %s",povolene);

    if(j>1) // Ak podmienka platí, program vypise nasledujuce povolene znaky
        odstran_duplikaty(povolene);

    if(j==0) // Ak podmienka plati, znamena to ze program nenasiel ziadnu vyhovujucu adresu
        printf("NOT FOUND\n");

}
/*  Funkcia odstrani duplikaty najdenych znakov,
 *  funkcia prechadza polom a ak najde zhodu znak
 *  nezapise ak nenajde zapise znak do pola
 */
int odstran_duplikaty(char *str)
{
    int znak=0;
    char z;
    while(str[znak])
    {
        z = str[znak];
        i = j = znak + 1;
        while(str[i])
        {
            if(str[i] != z)
            {
                str[j] = str[i];
                j++;
            }
            i++;
        }
        str[j]='\0';
        znak++;
    }
    printf("ENABLE: %s\n",str);
return 1;
}







