#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

#define MAX_POLE 100000

typedef struct vrchol
{
    struct vrchol* horny_sused;
    struct vrchol* dolny_sused;
    struct vrchol* pravy_sused;
    struct vrchol* lavy_sused;
    struct vrchol* predchodca;
    int x;
    int y;
    int cena_od_zaciatku;
    char typ;
    int cena_policka;
    int navstiveny;
    int index_v_min_strukture;
} VRCHOL;

typedef struct min_struktura
{
    int dlzka_aktualna;
    VRCHOL* pole[MAX_POLE];
} MIN_HALDA;

//nasledujúca funkcia prevzatá a upravená z: https://www.geeksforgeeks.org/generating-random-number-range-c/
// C program for generating a 
// random number in a given range.  
// numbers in range [lower, upper]. 
int retRandoms(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

int bublaj_dole(MIN_HALDA* halda, int index)
{
    int next = 0, aktualny_index = next, pom;

    if ((2 * aktualny_index) + 1 <= halda->dlzka_aktualna - 1)
        next = (2 * aktualny_index) + 1;
    if ((2 * aktualny_index) + 2 <= halda->dlzka_aktualna - 1)
        if (halda->pole[(2 * aktualny_index) + 2]->cena_od_zaciatku < halda->pole[(2 * aktualny_index) + 1]->cena_od_zaciatku)
            next = (2 * aktualny_index) + 2;

    while (next && halda->pole[next]->cena_od_zaciatku < halda->pole[aktualny_index]->cena_od_zaciatku)
    {
        pom = halda->pole[next];
        halda->pole[next] = halda->pole[aktualny_index];
        halda->pole[next]->index_v_min_strukture = next;
        halda->pole[aktualny_index] = pom;
        halda->pole[aktualny_index]->index_v_min_strukture = aktualny_index;

        next = 0;

        if ((2 * aktualny_index) + 1 <= halda->dlzka_aktualna - 1)
            next = (2 * aktualny_index) + 1;
        if ((2 * aktualny_index) + 2 <= halda->dlzka_aktualna - 1)
            if (halda->pole[(2 * aktualny_index) + 2]->cena_od_zaciatku < halda->pole[(2 * aktualny_index) + 1]->cena_od_zaciatku)
                next = (2 * aktualny_index) + 2;
    }

    return 1;

}

int bublaj_hore(MIN_HALDA* halda, int index)
{
    int aktualny_index = index;
    int pom;

    while (aktualny_index > 0 && halda->pole[(aktualny_index - 1) / 2] > halda->pole[aktualny_index])
    {
        pom = halda->pole[(aktualny_index - 1) / 2];
        halda->pole[(aktualny_index - 1) / 2] = halda->pole[aktualny_index];
        halda->pole[(aktualny_index - 1) / 2]->index_v_min_strukture = (aktualny_index - 1) / 2;
        halda->pole[aktualny_index] = pom;
        halda->pole[aktualny_index]->index_v_min_strukture = aktualny_index;

        aktualny_index = (aktualny_index - 1) / 2;
    }

    return 0;
}

int halda_init(MIN_HALDA* halda)
{
    halda->dlzka_aktualna = 0;
}

int pridaj_do_haldy(MIN_HALDA* halda, VRCHOL* hodnota)
{
    if (halda->dlzka_aktualna < MAX_POLE)
        halda->pole[halda->dlzka_aktualna] = hodnota;
    else
    {
        printf("Prekrocena kapacita haldy!\n");
        return 0;
    }

    halda->dlzka_aktualna++;

    bublaj_hore(halda, halda->dlzka_aktualna - 1);

    return 1;
}

int vyber_z_haldy(MIN_HALDA* halda, VRCHOL** hodnota)
{
    if (halda->dlzka_aktualna == 0)
    {
        *hodnota = NULL;
        return 0;
    }

    *hodnota = halda->pole[0];
    halda->dlzka_aktualna--;
    halda->pole[0] = halda->pole[halda->dlzka_aktualna];

    bublaj_dole(halda, 0);

    return 1;
}

int vykresli_cestu(char** mapa, int* dlzka_cesty, int** suradnice_cesty, VRCHOL* aktualny)
{
    int* nove_suradnice = NULL;

    mapa[aktualny->x][aktualny->y] = 'X';
    (*dlzka_cesty)++;
    nove_suradnice = (int*)realloc(*suradnice_cesty, sizeof(int) * (*dlzka_cesty) * 2);
    *suradnice_cesty = nove_suradnice;
    (*suradnice_cesty)[(*dlzka_cesty) * 2 - 2] = aktualny->x;
    (*suradnice_cesty)[(*dlzka_cesty) * 2 - 1] = aktualny->y;

    if (aktualny->predchodca && aktualny->predchodca->predchodca)
        vykresli_cestu(mapa, dlzka_cesty, suradnice_cesty, aktualny->predchodca);

    return 0;
}

int najdi_draka_v_bludisku(MIN_HALDA* halda, VRCHOL* aktualny, char ciel, char** mapa, int* cena_cesty, int* dlzka_cesty, int** suradnice_cesty, VRCHOL** posledny)
{
    VRCHOL* nasledujuci;

    aktualny->navstiveny = 1;

    if (aktualny->typ == ciel)
    {
        aktualny->typ = aktualny->typ + 32;
        *cena_cesty = aktualny->cena_od_zaciatku;
        *posledny = aktualny;
        vykresli_cestu(mapa, dlzka_cesty, suradnice_cesty, aktualny);
        aktualny->predchodca = NULL;
        return 0;
    }

    if (aktualny->pravy_sused)
        if (!aktualny->pravy_sused->navstiveny)
        {
            aktualny->pravy_sused->navstiveny = 1;
            aktualny->pravy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->pravy_sused->cena_policka;
            aktualny->pravy_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->pravy_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->pravy_sused->cena_policka < aktualny->pravy_sused->cena_od_zaciatku)
            {
                aktualny->pravy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->pravy_sused->cena_policka;
                aktualny->pravy_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->pravy_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->pravy_sused->index_v_min_strukture);
            }

    if (aktualny->lavy_sused)
        if (!aktualny->lavy_sused->navstiveny)
        {
            aktualny->lavy_sused->navstiveny = 1;
            aktualny->lavy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->lavy_sused->cena_policka;
            aktualny->lavy_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->lavy_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->lavy_sused->cena_policka < aktualny->lavy_sused->cena_od_zaciatku)
            {
                aktualny->lavy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->lavy_sused->cena_policka;
                aktualny->lavy_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->lavy_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->lavy_sused->index_v_min_strukture);
            }

    if (aktualny->horny_sused)
        if (!aktualny->horny_sused->navstiveny)
        {
            aktualny->horny_sused->navstiveny = 1;
            aktualny->horny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->horny_sused->cena_policka;
            aktualny->horny_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->horny_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->horny_sused->cena_policka < aktualny->horny_sused->cena_od_zaciatku)
            {
                aktualny->horny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->horny_sused->cena_policka;
                aktualny->horny_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->horny_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->horny_sused->index_v_min_strukture);
            }

    if (aktualny->dolny_sused)
        if (!aktualny->dolny_sused->navstiveny)
        {
            aktualny->dolny_sused->navstiveny = 1;
            aktualny->dolny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->dolny_sused->cena_policka;
            aktualny->dolny_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->dolny_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->dolny_sused->cena_policka < aktualny->dolny_sused->cena_od_zaciatku)
            {
                aktualny->dolny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->dolny_sused->cena_policka;
                aktualny->dolny_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->dolny_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->dolny_sused->index_v_min_strukture);
            }

    if (!vyber_z_haldy(halda, &nasledujuci))
        return 1;

    return najdi_draka_v_bludisku(halda, nasledujuci, ciel, mapa, cena_cesty, dlzka_cesty, suradnice_cesty, posledny);
}

int najdi_princezne_v_bludisku(MIN_HALDA* halda, VRCHOL* aktualny, char ciel, char** mapa, int* cena_cesty, int* dlzka_cesty, int** suradnice_cesty, VRCHOL** posledny)
{
    VRCHOL* nasledujuci;

    aktualny->navstiveny = 1;

    if (aktualny->typ == ciel)
    {
        aktualny->typ = aktualny->typ + 32;
        *cena_cesty = aktualny->cena_od_zaciatku;
        *posledny = aktualny;
        vykresli_cestu(mapa, dlzka_cesty, suradnice_cesty, aktualny);
        aktualny->predchodca = NULL;
        return 0;
    }

    if (aktualny->pravy_sused && aktualny->pravy_sused->typ != 'N')
        if (!aktualny->pravy_sused->navstiveny)
        {
            aktualny->pravy_sused->navstiveny = 1;
            aktualny->pravy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->pravy_sused->cena_policka;
            aktualny->pravy_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->pravy_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->pravy_sused->cena_policka < aktualny->pravy_sused->cena_od_zaciatku)
            {
                aktualny->pravy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->pravy_sused->cena_policka;
                aktualny->pravy_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->pravy_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->pravy_sused->index_v_min_strukture);
            }

    if (aktualny->lavy_sused && aktualny->lavy_sused->typ != 'N')
        if (!aktualny->lavy_sused->navstiveny)
        {
            aktualny->lavy_sused->navstiveny = 1;
            aktualny->lavy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->lavy_sused->cena_policka;
            aktualny->lavy_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->lavy_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->lavy_sused->cena_policka < aktualny->lavy_sused->cena_od_zaciatku)
            {
                aktualny->lavy_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->lavy_sused->cena_policka;
                aktualny->lavy_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->lavy_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->lavy_sused->index_v_min_strukture);
            }

    if (aktualny->horny_sused && aktualny->horny_sused->typ != 'N')
        if (!aktualny->horny_sused->navstiveny)
        {
            aktualny->horny_sused->navstiveny = 1;
            aktualny->horny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->horny_sused->cena_policka;
            aktualny->horny_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->horny_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->horny_sused->cena_policka < aktualny->horny_sused->cena_od_zaciatku)
            {
                aktualny->horny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->horny_sused->cena_policka;
                aktualny->horny_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->horny_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->horny_sused->index_v_min_strukture);
            }

    if (aktualny->dolny_sused && aktualny->dolny_sused->typ != 'N')
        if (!aktualny->dolny_sused->navstiveny)
        {
            aktualny->dolny_sused->navstiveny = 1;
            aktualny->dolny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->dolny_sused->cena_policka;
            aktualny->dolny_sused->predchodca = aktualny;
            pridaj_do_haldy(halda, aktualny->dolny_sused);
        }
        else
            if (aktualny->cena_od_zaciatku + aktualny->dolny_sused->cena_policka < aktualny->dolny_sused->cena_od_zaciatku)
            {
                aktualny->dolny_sused->cena_od_zaciatku = aktualny->cena_od_zaciatku + aktualny->dolny_sused->cena_policka;
                aktualny->dolny_sused->predchodca = aktualny;
                bublaj_dole(halda, aktualny->dolny_sused->index_v_min_strukture);
                bublaj_hore(halda, aktualny->dolny_sused->index_v_min_strukture);
            }

    if (!vyber_z_haldy(halda, &nasledujuci))
        return 1;

    return najdi_princezne_v_bludisku(halda, nasledujuci, ciel, mapa, cena_cesty, dlzka_cesty, suradnice_cesty, posledny);
}

int cisti_struktury(VRCHOL* start, int n, int m)
{
    int i, j;
    VRCHOL* aktualny = start;
    VRCHOL* prvy_v_riadku = start;

    for (i = 0; i < n; i++)
    {
        aktualny = prvy_v_riadku;
        for (j = 0; j < m; j++)
        {
            aktualny->navstiveny = 0;
            aktualny->predchodca = NULL;
            aktualny = aktualny->pravy_sused;
        }
        prvy_v_riadku = prvy_v_riadku->dolny_sused;
    }
    return 0;

}

int ohodnot_policko(char typ)
{
    switch (typ)
    {
    case 'C': return 1; break;
    case 'H': return 2; break;
    case 'N': return 100000; break;
    case 'D': return 1; break;
    case 'P': return 1; break;
    }
    return -1;
}

int inicializuj_policko(VRCHOL* aktualny, int x, int y, char typ)
{
    aktualny->lavy_sused = NULL;
    aktualny->pravy_sused = NULL;
    aktualny->horny_sused = NULL;
    aktualny->dolny_sused = NULL;
    aktualny->x = x;
    aktualny->y = y;
    aktualny->cena_od_zaciatku = 0;
    aktualny->cena_policka = ohodnot_policko(typ);
    aktualny->typ = typ;
    aktualny->predchodca = NULL;
    aktualny->navstiveny = 0;

    return 0;
}

int generuj_policka(char** mapa, VRCHOL* start, int n, int m)
{
    int i, j;
    VRCHOL* aktualny = start;
    VRCHOL* aktualny_nad = start;
    VRCHOL* prvy_v_riadku = start;

    for (i = 0; i < n; i++)
    {
        aktualny = prvy_v_riadku;
        for (j = 0; j < m; j++)
        {
            if (aktualny->x != n - 1)
            {
                aktualny->dolny_sused = (VRCHOL*)calloc(sizeof(VRCHOL), 1);
                inicializuj_policko(aktualny->dolny_sused, i + 1, j, mapa[i + 1][j]);
                aktualny->dolny_sused->horny_sused = aktualny;
            }
            if (aktualny->y != m - 1 && i == 0)
            {
                aktualny->pravy_sused = (VRCHOL*)calloc(sizeof(VRCHOL), 1);
                inicializuj_policko(aktualny->pravy_sused, i, j + 1, mapa[i][j + 1]);
                aktualny->pravy_sused->lavy_sused = aktualny;
            }
            else
                if (j < m - 1)
                {
                    aktualny_nad = aktualny_nad->pravy_sused;
                    aktualny->pravy_sused = aktualny_nad->dolny_sused;
                    aktualny->pravy_sused->lavy_sused = aktualny;
                }
            aktualny = aktualny->pravy_sused;
        }
        aktualny_nad = prvy_v_riadku;
        prvy_v_riadku = prvy_v_riadku->dolny_sused;
    }
    return 0;
}

int prevrat_cestu(int** cesta, int dlzka, int od)
{
    int i, j = 0;
    int* pom_pole = NULL, * pom;

    pom_pole = (int*)calloc(sizeof(int), dlzka * 2);

    for (i = 0; i < od; i++)
        pom_pole[i] = (*cesta)[i];

    for (i = od; i < dlzka * 2; i++)
        pom_pole[i] = (*cesta)[(dlzka * 2) - j++ - 1];

    free(*cesta);
    *cesta = pom_pole;

    return 1;
}

int* zachran_princezne(char** mapa, int n, int m, int t, int* dlzka_cesty)
{
    MIN_HALDA halda;
    int cas = ohodnot_policko(mapa[0][0]);
    int od;
    int cena_cesty;
    int* suradnice_cesty = (int*)calloc(sizeof(int), 1);
    VRCHOL* aktualny = NULL;
    VRCHOL* zaciatok = NULL;

    aktualny = (VRCHOL*) malloc(sizeof(VRCHOL));

    zaciatok = aktualny;

    inicializuj_policko(aktualny, 0, 0, mapa[0][0]);

    generuj_policka(mapa, aktualny, n, m);

    halda_init(&halda);

    od = *dlzka_cesty;

    najdi_draka_v_bludisku(&halda, aktualny, 'D', mapa, &cena_cesty, dlzka_cesty, &suradnice_cesty, &aktualny);

    cas += aktualny->cena_od_zaciatku;

    if (cas > t)
    {
        free(suradnice_cesty);
        return NULL;
    }

    if (mapa[0][0] != 'D')
    {
        (*dlzka_cesty)++;
        suradnice_cesty = (int*)realloc(suradnice_cesty, sizeof(int) * (*dlzka_cesty) * 2);
    }
    prevrat_cestu(&suradnice_cesty, *dlzka_cesty, od * 2);

    suradnice_cesty[0] = 0;
    suradnice_cesty[1] = 0;

    mapa[0][0] = 'X';

    halda_init(&halda);

    cisti_struktury(zaciatok, n, m);

    od = *dlzka_cesty;

    while (!najdi_princezne_v_bludisku(&halda, aktualny, 'P', mapa, &cena_cesty, dlzka_cesty, &suradnice_cesty, &aktualny))
    {
        prevrat_cestu(&suradnice_cesty, *dlzka_cesty, od * 2);
        od = *dlzka_cesty;
        cisti_struktury(zaciatok, n, m);
        halda_init(&halda);
    }

    return suradnice_cesty;
}

int generuj_mapu(char** mapa, int pocet_princezien, int n, int m)
{
    int i, j, x, y, typ;

    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
        {
            typ = retRandoms(0, 2);

            if (1 == typ)
                typ = retRandoms(0, 2);

            switch (typ) {
            case 0:
                mapa[i][j] = 'C';
                break;
            case 1:
                mapa[i][j] = 'N';
                break;
            case 2:
                mapa[i][j] = 'H';
                break;
            }
        }

    while (pocet_princezien)
    {
        x = retRandoms(0, n - 1);
        y = retRandoms(0, m - 1);

        if (mapa[x][y] != 'P')
        {
            mapa[x][y] = 'P';
            pocet_princezien--;
        }
    }

    while (1)
    {
        x = retRandoms(0, n - 1);
        y = retRandoms(0, m - 1);

        if (mapa[x][y] != 'P')
        {
            mapa[x][y] = 'D';
            break;
        }
    }

    return 0;
}

int alokuj_mapu(char*** mapa, int n, int m)
{
    int i;

    *mapa = (char**)malloc(sizeof(char*) * n);

    for (i = 0; i < n; i++)
        (*mapa)[i] = (char*)calloc(sizeof(char), m);

    return 1;
}

int uvolni_mapu(char*** mapa, int n, int m)
{
    int i;

    for (i = 0; i < n; i++)
        free((*mapa)[i]);

    free(*mapa);

    return 1;
}

int vypis_mapu(char** mapa, int n, int m)
{
    int i, j;

    printf("\n");

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
            printf("%c", mapa[i][j]);
        printf("\n");
    }
}

int test(int n, int m, int t, int pocet_princezien)
{

    char** mapa = NULL;
    int dlzka_cesty = 0;
    int i, * cesta;

    printf("-----------------------------------------------------------\n");
    alokuj_mapu(&mapa, n, m);
    generuj_mapu(mapa, pocet_princezien, n, m);
    printf("mapa: ");
    vypis_mapu(mapa, n, m);
    cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
    if (cesta == NULL)
        printf("nestihol zabit draka (mozno neexistuje taka cesta)\n");
    else
    {
        printf("\nmapa s cestou: ");

        vypis_mapu(mapa, n, m);

        printf("\ndlzka cesty: %d\n", dlzka_cesty);

        printf("\nsuradnice: \n");

        for (i = 0; i < dlzka_cesty; i++)
            printf("%d %d\n", cesta[i * 2], cesta[i * 2 + 1]);
    }

    printf("-----------------------------------------------------------\n");
    uvolni_mapu(&mapa, n, m);

    return 0;
}

int main() {

    srand(time(0));

    //test berie tieto argumenty:
    //test(int n, int m, int t, int pocet_princezien)

    test(5, 5, 20, 2);
    test(5, 5, 20, 3);

    return 0;
}