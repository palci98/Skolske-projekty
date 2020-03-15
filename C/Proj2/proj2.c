/*
 * Soubor:  proj2.c
 * Datum:   November 2017
 * Autor:   xpodlu01, xpodlu01@stud.fit.vutbr.cz
 * Projekt: Iteracne vypocty
 * Popis:   Program je v podstate jednoducha kalkulacka, ktora pocita tangens roznymy sposobmi,
 *          porovnava vysledne hodnoty a vypisuje s vysledkami zaroven aj ich odchylku. Dalej
 *          program meria vzdialenosti, dokaze odmerat vzdialenost meriacieho pristroja od
 *          objektu a nasledne pomocou tejto hodnoty vie vypocitat aj jeho vysku.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TAYLOR_MAX_I 13 // maximalna povolena iteracia Taylorovho polynomu
#define VYSKA 1.5 // primarne zadana hodnota
#define MAX_VYSKA 100 //maximalna vyska zadana uzivatelom
#define MAX_RADIANOV 1.4 //maximum radianov zadanych uzivatelom

/**
 *  vlastna funkcia na vypocet absolutnej hodnoty
 *  \param  x hodnota, z ktorej sa vypocita absolutna hodnota
 *  \return   vracia absolutnu hodnutu a program s nou moze dalej pracovat
 */
double my_fabs(double x)
{
        return x>=0.0?x:-x;
}

/**
 *  funkcia pocita tangens pomocou Taylorovho polynomu
 *  \param x je hodnota uhla zadana v radianoch
 *  \param n je hodnota poslednej iteracie
 *  \return vracia vysledok tangensu pocitaneho pomocou Taylorovho polynomu
 */
double taylor_tan(double x, unsigned int n)
{
    const double citatel[TAYLOR_MAX_I] = {1, 1, 2, 17, 62, 1382, 21844, 929569,
        6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    const double menovatel[TAYLOR_MAX_I] = {1, 3, 15, 315, 2835, 155925, 6081075,
        638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};

    double vysledok,xx;
    xx=x*x;
    for(unsigned int i=0 ; i < n && i<TAYLOR_MAX_I; i++)
    {
        vysledok+=((x*citatel[i])/menovatel[i]);
        x*=xx;
    }
        return vysledok;
}

/**
 *  funkcia pocita tangens pomocou zretazenych zlomkov
 *  \param x je hodnota uhla zadana v radianoch
 *  \param n je hodnota poslednej iteracie
 *  \return vracia vysledok tangensu pocitaneho pomocou zretazenych zlomkov
 */

double cfrac_tan(double x, unsigned int n)
{
    double cf, xx;
    cf=0.; xx=x*x;
    unsigned int k=(2*n)-1;
    for(;n>1;k-=2,n--)
        {
            cf=(xx/(k-cf));

        }
        return (x/(k-cf));
}

/**
 *  Funkcia na vypocet dlzky medzi meranym objektom a zariadenim a naslednym meranim
 *  vysky daneho objektu
 *
 */

double vypocet_vysky(double alfa, double beta, double v)
{
    double vysledok, vysledokv;//vysledok je vysledok dlzky, vysledokv je vysledok vysky a v je vyska
    int i=10;//pocet iteraci, ktory staci na dostatocnu presnost
    if(((alfa <= 0)||(beta <= 0) || (alfa > MAX_RADIANOV)||(beta > MAX_RADIANOV)))
    {
        fprintf(stderr,"Nespravne zadany uhol, pre napovedu zadajte --help");
        return 0;
    }
    vysledok= v/cfrac_tan(alfa,i);
    vysledokv= (vysledok*cfrac_tan(beta,i))+v;
    printf("%.10e\n%.10e\n",vysledok,vysledokv);
    return 0;
}

/**
 *  Funkcia na vypocet dlzky medzi meranym objektom a zariadenim
 *  \param alfa je uhol v radianoch
 *  \param v je vyska meraneho objektu
 */

double vypocet_dlzky(double alfa, double v)
{
    double vysledok;//vysledok je vysledok dlzky,
    int i=10;//pocet iteraci, ktory staci na dostatocnu presnost
    if((alfa <= 0) || (alfa > MAX_RADIANOV))
    {
        fprintf(stderr,"Nespravne zadany uhol, pre napovedu zadajte --help");
        return 0;
    }
    vysledok= v/cfrac_tan(alfa,i);
    printf("%.10e\n",vysledok);
    return 0;
}

/**
 *  Funkcia vypise pomoc
 */

void pomoc()
{
    printf(" --tan porovna presnosti vypoctou tangens uhlu A (v radianoch) medzi volanim tan z matematickej kniznice,\n"
           " vypoctu tangens pomocou Taylorovho polynomu a zretazeneho zlomku.\n"
           "\n"
           " -m vypocita a zmera vzdialenosti.\n"
           " Uhol alfa je dany argumentom A v radianoch. Program vypocita a vypise vzdialenost meraneho objektu. 0 < A <= 1.4 < π/2.\n"
           " Pokial je zadany, uhol beta udava argument B v radianoch. Program vypocita a vypise aj vysku meraneho objektu. 0 < B <= 1.4 < π/2\n"
           "\n"
           " Argument -c nastavuje vysku meriacieho pristroja c pre vypocet. Vyska c je dana argumentom X (0 < X <= 100).\n"
           " Argument je volitelny - implicitna vyska je 1.5 metrov.\n")  ;

}




/**
 *  Funkcia porovnava vypocty matematickej kniznice s taylorovym polynom a zretazenym zlomkom
 *  \param rads uhol v radianoch
 *  \param i1 je zaciatocny pocet iteraci
 *  \param i2 je posledna iteracia
 */

void tan_por_vyp(double rads, int i1, int i2)
{
    double t_tan, cf_tan, tan_m;

    tan_m = tan(rads);
    for(; i1<=i2; i1++)
    {
        t_tan  = taylor_tan(rads, i1);
        cf_tan = cfrac_tan(rads, i1);
        printf("%d %e %e %e %e %e\n", i1, tan_m, t_tan,
        my_fabs(tan_m-t_tan), cf_tan, my_fabs(tan_m-cf_tan));
    }

}

/**
 *  Funkcia, ktora pracuje s argumentami, zistuje ake argumenty
 *  boli zadane a podla toho dalej pracuje s inymi funkciami
 *  \param argc je pocet argumentov programu
 *  \param argv je konkretny argument programu zapisany ako retazec znakov
 *  \param pole pomocna premenna, do ktorej zapise retazce z argv
 */
int argumenty(int argc, char** argv, double* pole)
{
    double v;  // vyska objektu
    if (argc>=2)
    {
        if (strcmp(argv[1],"--help")==0)
        {
            pomoc();
            return 0;
        }
        if (strcmp(argv[1],"--tan")==0 && argc==5)
        {
            char *u1,*u2;
            int i1,i2;
            pole[0] = strtod(argv[2],NULL);
            i1 = strtol(argv[3],&u1,10);
            i2 = strtol(argv[4],&u2,10);
            tan_por_vyp(pole[0],i1,i2);
            return 0;
        }
        else if(strcmp(argv[1],"--tan")==0 && (argc!=5))
            fprintf(stderr,"Error chybny pocet argumentov. Pre napovedu zadajte --help");
        if ((strcmp(argv[1],"-m")==0) && ((argc==3) || (argc==4)))
        {
            v=VYSKA;
            pole[0] = strtod(argv[2],NULL);
            if (argc==4)
            {
                pole[1] = strtod(argv[3],NULL);
                vypocet_vysky(pole[0],pole[1],v) ;
            }
            else
                vypocet_dlzky(pole[0],v);
        }
        else if((strcmp(argv[1],"-m")==0) && ((argc!=3)||(argc!=4)))
            fprintf(stderr,"Error chybny pocet argumentov. Pre napovedu zadajte --help");

        if ((strcmp(argv[1],"-c")==0) && ((argc==5)||(argc==6)))
        {
            v = strtod(argv[2],NULL);
            if(0>=v || v>MAX_VYSKA)
            {
                fprintf(stderr,"Vyska nesplna poziadavky, pre viac informacii zadajte --help");
                return 0;
            }
            pole[0] = strtod(argv[4],NULL);
            if(argc==6)
            {
                pole[1] = strtod(argv[5],NULL);
                vypocet_vysky(pole[0],pole[1],v);
            }
            else
                vypocet_dlzky(pole[0],v) ;
        }
        else if((strcmp(argv[1],"-c")==0) && ((argc!=5)||(argc!=6)))
            fprintf(stderr,"Error chybny pocet argumentov");

    }
    return 0;
}

int main(int argc, char *argv[])
{
    double pole[2]={0.0};
    if(argc==1)
        {
            fprintf(stderr,"Nezadany argument, zadajte --help pre pomoc\n");
            return 0;
        }
        if(argc>6)
        {
            fprintf(stderr,"Prekroceny limit argumentov, zadajte --help pre pomoc");
            return 0;
        }
    argumenty(argc,argv,pole);



    return 0;
}
