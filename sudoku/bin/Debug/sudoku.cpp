
/*---------Sudoku---------*/
#include <ga/GASimpleGA.h> //  Algoritmo Genetico simple
#include <ga/GA1DArrayGenome.h> // Genoma --> array de enteros (dim. 1) alelos
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

struct plantilla
{
    int tam;
    int *fijo;
};

void leerSudoku(struct plantilla *S,char *nombreF)
{
    ifstream f(nombreF);


    f>>S->tam;


    S->fijo = new int[S->tam*S->tam];

    for(int i=0; i<S->tam*S->tam; i++)
        f>>S->fijo[i];

    f.close();
}

void InicioSudoku(GAGenome& g)
{

    GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int> &) g;
    //TODO

    struct plantilla * plantilla1;
    plantilla1 = (struct plantilla *) genome.userData();

    int aux[plantilla1->tam];

    for(int f=0; f<plantilla1->tam; f++)
    {

        for(int j=0; j<plantilla1->tam; j++)
            aux[j]=0;

        for(int j=1; j<=plantilla1->tam; j++)
        {
            int v=GARandomInt(0,plantilla1->tam-1);
            while (aux[v]!=0)
                v=(v+1)%plantilla1->tam;
            aux[v]=j;
        }

        int i=0;

        while(i<plantilla1->tam)
        {

            while((plantilla1->fijo[(f*plantilla1->tam)+i]==0) && (i<plantilla1->tam))
                i++;

            if (i<plantilla1->tam)
            {

                bool encontrado=false;
                for(int j=0; (j<plantilla1->tam) && (!encontrado); j++)
                    if (aux[j]==plantilla1->fijo[(f*plantilla1->tam)+i])
                    {
                        encontrado=true;
                        aux[j]=aux[i];
                    }

                aux[i]=plantilla1->fijo[(f*plantilla1->tam)+i];
            }
            i++;

        }

        for(int c=0; c<plantilla1->tam; c++)
            genome.gene((f*plantilla1->tam)+c,aux[c]);
    }
}

int CruceSudoku(const GAGenome& p1,const GAGenome & p2,GAGenome* c1,GAGenome* c2)
{

    GA1DArrayGenome<int> &m = (GA1DArrayGenome<int> &) p1;
    GA1DArrayGenome<int> &p = (GA1DArrayGenome<int> &) p2;
    struct plantilla * plantilla1 = (struct plantilla *) m.userData();
    int n=0;

    int punto1=GARandomInt(0,m.length());
    while ((punto1%plantilla1->tam)!=0)
        punto1++;
    int punto2=m.length()-punto1;

    if (c1)
    {

        GA1DArrayGenome<int>& h1 = (GA1DArrayGenome<int> &)  *c1;
        h1.copy(m,0,0,punto1); // el metodo copy esta definido en la clase GA1DArrayGenome
        h1.copy(p,punto1,punto1,punto2);
        n++;
    }

    if (c2)
    {

        GA1DArrayGenome<int>& h2 = (GA1DArrayGenome<int> &) *c2;
        h2.copy(p,0,0,punto1);
        h2.copy(m,punto1,punto1,punto2);
        n++;
    }

    return n;

}

bool checkColumna(int col[], int * check, int tam)
{
    bool repe=false;

    for(int i=0; i<tam; i++)
        check[i]=0;

    for(int i=0; i<tam; i++)
        check[col[i]-1]++;
    for(int i=0; i<tam; i++)
        if (check[i]>1)
            repe=true;

    return repe;
}


int MutacionSudoku(GAGenome& g,float pmut)
{

    struct plantilla * plantilla1;
    GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int> &) g;
    plantilla1 = (struct plantilla *) genome.userData();
    int nmut=0;
    int aux,aux1;
    int fil;
    bool fila;

    int caux[plantilla1->tam];
    int *checkC=new int[plantilla1->tam];

    if (pmut<=0.0)
        return 0;

    for(int f=0; f<plantilla1->tam; f++)
        for(int c=0; c<plantilla1->tam; c++)
            if (plantilla1->fijo[(f*plantilla1->tam)+c]==0)
            {
                if (GAFlipCoin(pmut) )
                {
                    if (GAFlipCoin(0.5))
                        fila = true;
                    else
                        fila = false;

                    if (!fila)
                    {

                        for(int j=0; j<plantilla1->tam; j++)
                            caux[j]=genome.gene((j*plantilla1->tam)+c);
                        if (checkColumna(caux,checkC,plantilla1->tam))
                        {
                            int v1 = GARandomInt(0,plantilla1->tam-1);
                            while (checkC[v1]<=1)
                                v1=(v1+1)%plantilla1->tam;
                            v1++;
                            int v2 = GARandomInt(0,plantilla1->tam-1);
                            while (checkC[v2]!=0)
                                v2=(v2+1)%plantilla1->tam;
                            v2++;

                            bool encontrado = false;
                            for(int j=0; j<plantilla1->tam && !encontrado; j++)
                                if ((plantilla1->fijo[j*(plantilla1->tam)+c]==0)&&(genome.gene(j*(plantilla1->tam)+c)==v1))
                                {
                                    encontrado = true;
                                    genome.gene((j*plantilla1->tam)+c,v2);
                                    fil = j;
                                }

                            int col=(c+1)%plantilla1->tam;
                            while(genome.gene((fil*plantilla1->tam)+col)!=v2)
                                col=(col+1)%plantilla1->tam;
                            if (plantilla1->fijo[(fil*plantilla1->tam)+col]==0)
                            {
                                nmut++;
                                genome.gene((fil*plantilla1->tam)+col,v1);
                            }
                            else
                            {
                                genome.gene((fil*plantilla1->tam)+c,v1);
                            }

                        }

                    }
                    else
                    {
                        int v1 = (c + 1) %plantilla1->tam;
                        while ((plantilla1->fijo[(f*plantilla1->tam)+v1]!=0))
                            v1=(v1+1)%plantilla1->tam;
                        aux = genome.gene((f*plantilla1->tam)+c);
                        genome.gene((f*plantilla1->tam)+c,genome.gene((f*plantilla1->tam)+v1));
                        genome.gene((f*plantilla1->tam)+v1,aux);
                        nmut++;
                    }
                }
            }

    return nmut;
}


GABoolean Termina(GAGeneticAlgorithm & ga)
{
    if ( (ga.statistics().minEver()==0) ||
            (ga.statistics().generation()==ga.nGenerations()) )
        return gaTrue;
    else
        return gaFalse;
}

// Funcion objetivo.

float Objective(GAGenome& g)
{

    GA1DArrayAlleleGenome<int> & genome = (GA1DArrayAlleleGenome<int> &)g;
    float nerr = 0;
    int tam  = (int) sqrt(genome.length());
    int r_tam = sqrt(tam);


    //ERRORES POR LINEA , NUMEROS REPETIDOS

    for(int nl = 0; nl < tam ; nl=+tam)
    {
        for(int i  = nl; i < tam ; i++)
        {
            int j = i+1 ;
            for (; j < tam; j++)
            {

                if(genome.gene(i)==genome.gene(j))
                {


                    nerr++;
                }

            }
        }


    }

    //ERRORES POR COLUMNA
    int fin = tam * (tam - 1 ) ;
    for(int nc = 0; nc < tam; nc++)
    {
        //final de la columna


        for(int i = nc ; i <= fin ; i+= tam)
        {

            int j = i + tam;

            for(; j <= fin ; j+=tam)
            {



                if( genome.gene(i) == genome.gene(j))
                {


                    nerr++;
                }
            }

        }

    }

    //ERRORES POR SUBCUADRICULA
    int conts = 0;
    int conj=0;
    int finc = tam*tam - ((tam)*(r_tam -1 ) + r_tam);
    for(int ns = 0; ns <= finc  ;)
    {

        int conts_in = 0;
        int finc_in = ns+tam * (r_tam -1 )+(r_tam-1);


        for(int i = ns; i<finc_in;  )
        {
            int j = 0;
            if(conts_in == (r_tam -1)){
                j = i+tam - (r_tam -1);
                conj=0;
            }else {
                j= i+1;
                conj++;
            }

            for(;j<= finc_in; ){
              //  cout << i << " " << j << endl;

                if(genome.gene(i)==genome.gene(j)){
                    nerr++;
                }
              //  cout << " conj " << conj << endl;
                if (conj == (r_tam-1)){
                    j= j + (tam - (r_tam-1));
                    conj = 0;
                }else{
                    conj++;
                    j++;
                }

            }


            if(conts_in == (r_tam-1))
            {
                conts_in=0;
                i+=tam-(r_tam -1);

            }
            else
            {
                i++;
                conts_in++;
                conj++;

            }

        }

        conts++;
        if(conts == r_tam )
        {
            conts = 0;
            // primer sector de la linea inferior
            ns += ((tam * r_tam) - (tam - r_tam));
            conj=0;

        }
        else
        {
            //sector horizontal
            ns+= r_tam ;
             conj=0;

        }

    }



    return nerr;



}
int main(int argc,char **argv)
{
    struct plantilla plantilla0 ;
    leerSudoku(&plantilla0, argv[1]);
    int popsize = atoi(argv[2]); //100 ,150
    //realizar comprobaciones del operador de seleccion

    char * seoper = argv[3]; //operador de selecion, ruleta o torneo
    float pcross = atof(argv[4]); //prob cruce
    float pmut = atof(argv[5]); //prob mutacion

    cout << "Sudoku : " << endl;
    cout << endl;
    for(int i =0; i< (plantilla0.tam * plantilla0.tam)  ; i++)
    {
        cout<< plantilla0.fijo[i] << " " ;
        if ((i+1) % plantilla0.tam == 0)
        {
            cout << endl;
        }
    }


    cout << endl;
    cout << "Parametros:    - Tamano poblacion: " << popsize << endl;
    cout << "               - Operador de Seleccion: " << seoper << endl;
    cout << "               - Probabilidad cruce: " << pcross << endl;
    cout << "               - Probabilidad mutacion: " << pmut << endl << endl;


    // Conjunto enumerado de alelos --> valores posibles de cada gen del genoma

    GAAlleleSet<int> alelos;
    for(int i=0; i<plantilla0.tam; i++)
        alelos.add(i);

    // Creamos el genoma y definimos operadores de inicio, cruce y mutaci�n

    GA1DArrayAlleleGenome<int> genome(plantilla0.tam * plantilla0.tam,alelos,Objective,&plantilla0);
    genome.crossover(CruceSudoku);
    genome.mutator(MutacionSudoku);

    // Creamos el algoritmo genetico
    genome.initializer(InicioSudoku);

    GASimpleGA ga(genome);



    // Inicializamos - minimizar funcion objetivo, tama�o poblacion, n� generaciones,
    // pr. cruce y pr. mutacion, selecci�n y le indicamos que evolucione.


    ga.minimaxi(-1);
    ga.populationSize(popsize);
    ga.nGenerations(12000);
    ga.pCrossover(pcross);
    ga.pMutation(pmut);
    if(strcmp(seoper,"GARouletteWheelSelector"))
    {
        GARouletteWheelSelector selector;
        ga.selector(selector);

    }
    else if(strcmp(seoper,"GATournamentSelector"))
    {
        GATournamentSelector selector;
        ga.selector(selector);
    }
    else
    {
        perror("Selector Error");
        exit(EXIT_FAILURE);
    }
    ga.terminator(Termina);
    ga.evolve(1);
    // Imprimimos el mejor individuo que encuentra el GA y su valor fitness
    cout << "El GA encuentra la solucion  " <<  endl;

    GA1DArrayAlleleGenome<int> &  solu = (GA1DArrayAlleleGenome<int> &) ga.statistics().bestIndividual();
    for(int i = 0; i < plantilla0.tam*plantilla0.tam; i++)
    {
        cout<<solu.gene(i) << " ";
        if( (i+1) % plantilla0.tam == 0)
        {
            cout << endl;
        }
    }
    cout  << endl;

    cout << "con valor fitness " << ga.statistics().minEver() << endl;

}


