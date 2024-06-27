#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>

using namespace std;

struct Individuo {
    vector <int> cromosoma;
    double fitness;
    Individuo() {}
    Individuo(vector <int> cromo, double fit) : cromosoma(cromo), fitness(fit) {};
};

void imprimeSolucion(Individuo& individuo, vector <int> items, int numAnaqueles) {

    
    for (int i = -1; i < numAnaqueles; i++) {
        if (i == -1) {
            std::cout << "\nAltura de cajas no asignadas: ";
            for (int j = 0; j < items.size(); j++)
                if (individuo.cromosoma[j] == -1)
                    cout << items[j] << " ";
        }
        else {
            cout << "Anaquel " << i + 1 << ": ";
            for (int j = 0; j < items.size(); j++) {
                if (individuo.cromosoma[j] == i)
                    cout << items[j] << " ";
            }
        }
        cout << endl;
    }
    cout << "Valor de fitness: " << individuo.fitness << endl << endl;
    cout << "======================================\n";
}

//================================ Inicializar Poblacion =======================================

void calcularFitness(Individuo& individuo, vector <int> items, int numAnaqueles, int alturaMax);

vector <Individuo> inicializarPoblacion(int popsize, int numItems, vector<int> anaqueles, vector <int> items, int alturaMax) {
    vector <Individuo> poblacion;
    for (int i = 0; i < popsize; i++) {
        vector <int> cromo(numItems, 0);
        vector<int> alturasAnaqueles(anaqueles.size(), 0);       //Inicializar vector para analizar restringir la poblacion por alturas
        vector<bool> usado(numItems, false);
        int itemsRestantes = numItems;
        while (itemsRestantes > 0) {
            int j = rand() % items.size();
            if (usado[j]) continue;
            bool asignado = false;
            int intentos = 0;                       // Colocar un maximo de intentos a probar por item (para evitar bucles infinitos)

            while (!asignado && intentos < 5) {
                int indiceAnaquel = rand() % anaqueles.size();    // Seleccionar un anaquel al azar
                if (alturasAnaqueles[indiceAnaquel] + items[j] <= alturaMax) {
                    cromo[j] = indiceAnaquel;                       // Asignar item al anaquel
                    alturasAnaqueles[indiceAnaquel] += items[j];    // Actualizar la altura del anaquel
                    asignado = true;
                }
                intentos++;
            }
            if (!asignado) {
                cromo[j] = -1;
            }
            usado[j] = true;
            itemsRestantes--;
        }

        poblacion.emplace_back(cromo, 0);
        //calcularFitness(poblacion[i], items, anaqueles.size(), alturaMax);
        //imprimeSolucion(poblacion[i], items, anaqueles.size());

    }
    return poblacion;
}


//================================ Seleccion y Fitness =======================================

int buscarMejorIndividuo(vector <Individuo> poblacion) {
    double mejorFitness = 0;
    int posMejorFitness = 0;
    for (int i = 0; i < poblacion.size(); i++) {
        if (poblacion[i].fitness > mejorFitness) {
            mejorFitness = poblacion[i].fitness;
            posMejorFitness = i;
        }
    }
    return posMejorFitness;
}

Individuo seleccionTorneo(vector <Individuo>& poblacion, int tamanhoTorneo) {
    vector <Individuo> torneo;
    for (int i = 0; i < tamanhoTorneo; i++) {
        int indiceAleatorio = rand() % poblacion.size();
        torneo.push_back(poblacion[indiceAleatorio]);
    }
    int posGanador = buscarMejorIndividuo(torneo);
    Individuo ganador(torneo[posGanador].cromosoma, torneo[posGanador].fitness);
    return ganador;
}

vector <Individuo> seleccionarSupervivientes(vector <Individuo>& poblacion, vector <Individuo>& poblacionDescendencia, int tamanhoPoblacion) {
    poblacion.insert(poblacion.end(), poblacionDescendencia.begin(), poblacionDescendencia.end());
    sort(poblacion.begin(), poblacion.end(), [](Individuo& a, Individuo& b) {
        return a.fitness > b.fitness;
        });
    vector <int> ruleta;
    while (poblacion.size() != tamanhoPoblacion) {
        for (int j = 0; j < poblacion.size(); j++) {
            for (int k = 0; k <= j; k++) ruleta.push_back(j);
        }
        int indiceRand = rand() % ruleta.size();
        poblacion.erase(poblacion.begin() + ruleta[indiceRand]);
        ruleta.clear();
    }
    return poblacion;
}

void calcularFitness(Individuo& individuo, vector <int> items, int numAnaqueles, int alturaMax) {
    int sumaAltura, sumaAlturaCajasAsignadas = 0;
    int sumaAlturaCajasSueltas = 0;

    for (int i = 0; i < items.size(); i++) {
        if (individuo.cromosoma[i] == -1)
            sumaAlturaCajasSueltas += items[i];
    }

    for (int i = 0; i <= numAnaqueles; i++) {
        sumaAltura = 0;
        for (int j = 0; j < items.size(); j++) {
            if (individuo.cromosoma[j] == i)
                sumaAltura += items[j];
        }
        if (sumaAltura > alturaMax) {
            individuo.fitness = 0;
            return;
        }
        sumaAlturaCajasAsignadas += sumaAltura;
    }

    int espacioLibre = (alturaMax * numAnaqueles) - sumaAlturaCajasAsignadas;
    double fitness = sumaAlturaCajasAsignadas - espacioLibre - sumaAlturaCajasSueltas;
    individuo.fitness = fitness;
}

void evaluarPoblacion(vector <Individuo>& poblacion, vector <int> items, int numAnaqueles, int alturaMax) {
    for (int i = 0; i < poblacion.size(); i++)
        calcularFitness(poblacion[i], items, numAnaqueles, alturaMax);
}

//================================ Cruces =======================================

pair <Individuo, Individuo> cruceUniforme(Individuo& padre, Individuo& madre) {
    int tamanho = padre.cromosoma.size();
    vector <int> hijoCromo(tamanho);
    vector <int> hijaCromo(tamanho);
    for (int i = 0; i < tamanho; i++) {
        if (rand() % 2) {
            hijoCromo[i] = padre.cromosoma[i];
            hijaCromo[i] = madre.cromosoma[i];
        }
        else {
            hijoCromo[i] = madre.cromosoma[i];
            hijaCromo[i] = padre.cromosoma[i];
        }
    }
    return make_pair(Individuo(hijoCromo, 0), Individuo(hijaCromo, 0));
}

pair <Individuo, Individuo> cruceMultipunto(Individuo& padre, Individuo& madre, int cantPuntosCorte) {
    int tamanho = padre.cromosoma.size();

    vector <int> puntosCorte(cantPuntosCorte);
    for (int i = 0; i < puntosCorte.size(); i++) {
        puntosCorte[i] = (rand() % (tamanho - 4)) + 2;
    }
    sort(puntosCorte.begin(), puntosCorte.end());
    bool flag = true;
    vector <int> hijoCromo = padre.cromosoma;
    vector <int> hijaCromo = madre.cromosoma;

    int indx = 0;
    for (int i = 0; i < puntosCorte.size(); i++) {
        if (flag) {
            for (; indx < puntosCorte[i]; indx++) {
                hijoCromo[indx] = padre.cromosoma[indx];
                hijaCromo[indx] = madre.cromosoma[indx];
            }
        }
        else {
            for (; indx < puntosCorte[i]; indx++) {
                hijoCromo[indx] = madre.cromosoma[indx];
                hijaCromo[indx] = padre.cromosoma[indx];
            }
        }
        flag = !flag;
    }

    return make_pair(Individuo(hijoCromo, 0), Individuo(hijaCromo, 0));
}


//================================ Mutacion =======================================


void mutacion_single_gene(Individuo& individuo, int numAnaqueles) {

    int posicion = rand() % individuo.cromosoma.size();
    individuo.cromosoma[posicion] = rand() % numAnaqueles;
}

void mutacion_permutation_inversion(Individuo& individuo) {

    int position1 = rand() % individuo.cromosoma.size();
    int position2 = rand() % individuo.cromosoma.size();
    if (position1 > position2)
        swap(position1, position2);

    reverse(individuo.cromosoma.begin() + position1, individuo.cromosoma.begin() + position2);
}

int algoritmoGenetico(vector <Individuo>& poblacion, vector <int> items, int numAnaqueles, int ALTURA_MAX,
    int generaciones, double tasaMutacion, int tamanhoTorneo, string tipoCruce, string tipoMutacion, int puntosDeCorte) {
    int popsize = poblacion.size();
    evaluarPoblacion(poblacion, items, numAnaqueles, ALTURA_MAX);
    vector <int> mejorFitness;
    int posMejorFitness = buscarMejorIndividuo(poblacion);
    Individuo mejorIndividuo(poblacion[posMejorFitness].cromosoma, poblacion[posMejorFitness].fitness);
    mejorFitness.push_back(mejorIndividuo.fitness);

    //cout << "Poblacion inicial, mejor fitness = " << mejorIndividuo.fitness << endl;

    for (int i = 0; i < generaciones; i++) {
        vector <pair <Individuo, Individuo>> poolCruces;
        for (int j = 0; j < (popsize / 2); j++) {
            poolCruces.push_back(make_pair(seleccionTorneo(poblacion, tamanhoTorneo), seleccionTorneo(poblacion, tamanhoTorneo)));
        }
        vector <Individuo> poblacionDescendencia;
        for (int k = 0; k < poolCruces.size(); k++) {

            pair <Individuo, Individuo> hijos;
            if(tipoCruce == "uniforme")
                hijos = cruceUniforme(poolCruces[k].first, poolCruces[k].second);
            else if(tipoCruce == "multipunto")
                hijos = cruceMultipunto(poolCruces[k].first, poolCruces[k].second, puntosDeCorte);

            if ((double)rand() / RAND_MAX < tasaMutacion) {
                if(tipoMutacion == "singleGene")
                    mutacion_single_gene(hijos.first, numAnaqueles);
                else if(tipoMutacion == "inversion")
                    mutacion_permutation_inversion(hijos.first);
            }
            if ((double)rand() / RAND_MAX < tasaMutacion) {
                if (tipoMutacion == "singleGene")
                    mutacion_single_gene(hijos.second, numAnaqueles);
                else if (tipoMutacion == "inversion")
                    mutacion_permutation_inversion(hijos.second);
            }


            poblacionDescendencia.push_back(hijos.first);
            poblacionDescendencia.push_back(hijos.second);
        }

        evaluarPoblacion(poblacionDescendencia, items, numAnaqueles, ALTURA_MAX);
        poblacion = seleccionarSupervivientes(poblacion, poblacionDescendencia, popsize);

        posMejorFitness = buscarMejorIndividuo(poblacion);
        mejorIndividuo.cromosoma.clear();
        mejorIndividuo.cromosoma.insert(mejorIndividuo.cromosoma.begin(), poblacion[posMejorFitness].cromosoma.begin(), poblacion[posMejorFitness].cromosoma.end());
        mejorIndividuo.fitness = poblacion[posMejorFitness].fitness;
        mejorFitness.push_back(mejorIndividuo.fitness);
        
        /*
        if (i % 2 == 0) {
            cout << "Generacion " << setw(2) << i << ", Mejor Fitness: " << mejorIndividuo.fitness << endl;
        }*/
    }

    //imprimeSolucion(mejorIndividuo, items, numAnaqueles);
    //cout << "Despues del algoritmo, mejor fitness = " << mejorIndividuo.fitness << endl;
    return mejorIndividuo.fitness;
}

//================================ Generar Items =======================================

//Funcion para generar items aleatorios que no necesariamente entraran todos en los anaqueles
vector<int> generarItemsAleatorios(int ALTURA_MAX, int cantItems) {

    vector<int> items(cantItems);
    for (int i = 0; i < items.size(); i++) {
        items[i] = rand() % ALTURA_MAX + 1;
    }

    return items;
}

// Función para generar items que llenen perfectamente los anaqueles
vector<int> generarItemsPerfectos(int ALTURA_MAX, int NUM_ANAQUELES, int NUM_ITEMS) {

    std::vector<int> items;

    // Calcula cuántas cajas caben en cada anaquel
    int cajas_por_anaquel = round((double)NUM_ITEMS / NUM_ANAQUELES);
    int resto_cajas = NUM_ITEMS - (cajas_por_anaquel * (NUM_ANAQUELES - 1));

    //Poner las cajas sobrantes en 1 anaquel
    int alturaRestante = ALTURA_MAX;
    for (int i = 1; i <= resto_cajas; i++) {
        if (i == resto_cajas) {
            items.push_back(alturaRestante);
            break;
        }
        int alturaCaja = rand() % alturaRestante;
        alturaRestante -= alturaCaja;
        items.push_back(alturaCaja);
    }

    NUM_ANAQUELES--;
    //Distribuye equitativamente el resto de cajas
    for (int i = 0; i < NUM_ANAQUELES; i++) {
        alturaRestante = ALTURA_MAX;
        for (int j = 1; j <= cajas_por_anaquel; j++) {
            if (j == cajas_por_anaquel) {
                items.push_back(alturaRestante);
                break;
            }
            int alturaCaja = (rand() % (alturaRestante - 1)) + 1;
            alturaRestante -= alturaCaja;
            items.push_back(alturaCaja);
        }

    }

    return items;
}


void probarAlgoritmoGenetico(const int NUM_ANAQUELES, const int ALTURA_MAX, const int NUM_ITEMS, const int NUM_ITERACIONES,
                            const int POP_SIZE, const int GENERACIONES, const double TASA_MUTACION, const int TAM_TORNEO,
                            string generacionItems, string tipoCruce, string tipoMutacion, int puntosCorte = 2) {
        
    if(tipoCruce != "uniforme" && tipoCruce != "multipunto"){
        cout << "Elija un tipo de cruce adecuado";
        return;
    }
    if(tipoMutacion != "singleGene" && tipoMutacion != "inversion"){
        cout << "Elija un tipo de mutacion adecuado";
        return;
    }
    if(generacionItems != "exacta" && generacionItems != "aleatoria"){
        std::cout << "Elija un tipo de generacion adecuada";
        return;
    }

    vector <int> items = (generacionItems == "exacta") 
    ? generarItemsPerfectos(ALTURA_MAX, NUM_ANAQUELES, NUM_ITEMS) 
    : generarItemsAleatorios(ALTURA_MAX, NUM_ITEMS);

    vector<int> anaqueles(NUM_ANAQUELES, ALTURA_MAX);
    double promedio = 0;
    int mejorFitness = NUM_ANAQUELES * ALTURA_MAX;
    int contador = 0;
    for (int i = 0; i < NUM_ITERACIONES; i++) {
        vector <Individuo> poblacion = inicializarPoblacion(POP_SIZE, NUM_ITEMS, anaqueles, items, ALTURA_MAX);
        double fitActual = algoritmoGenetico(poblacion, items, NUM_ANAQUELES, ALTURA_MAX, GENERACIONES, TASA_MUTACION, TAM_TORNEO, tipoCruce, tipoMutacion, puntosCorte);
        promedio += fitActual;
        if(generacionItems == "exacta" && mejorFitness == fitActual) 
            contador++;
    }
    promedio /= NUM_ITERACIONES;
    cout << "Promedio del fitness en " << NUM_ITERACIONES << " iteraciones: " << promedio << endl;
    if (generacionItems == "exacta") {
        double porcentaje = ((double)contador / NUM_ITERACIONES) * 100;
        cout << "Cantidad de soluciones optimas encontradas en " << NUM_ITERACIONES << " ejecuciones: " << contador << endl;
        cout << "Porcentaje de solucion optima encontrada entre las soluciones: " << porcentaje << "%\n";
        cout << "Mejor fitness para generacion de cajas exactas: " << mejorFitness;
    }
}

int main(int argc, char** argv) {

    srand(time(0));

    const int NUM_ANAQUELES = 5;
    const int ALTURA_MAX = 150;
    const int NUM_ITEMS = 11;

    const int POPSIZE = 10;
    const int GENERACIONES = 100;
    const double TASA_MUTACION_A = 0.0;
    const double TASA_MUTACION_B = 0.4;
    const double TASA_MUTACION_C = 0.8;
    const int TAMANHO_TORNEO = 3;
    const int PUNTOS_CORTE = 2;

    const int NUM_ITERACIONES = 100;


    //Para una generacion de items exacta colocar "exacta", de lo contrario colocar "aleatoria"
    //Para un cruce elegir entre tipo "uniforme" o "multipunto"
    //En caso escoger un cruce multipunto ingresar como ultimo parametro la cantidad de puntos a usar en el cruce
    //Para la mutacion elegir entre "singleGene" o "inversion"
    probarAlgoritmoGenetico(NUM_ANAQUELES, ALTURA_MAX, NUM_ITEMS, NUM_ITERACIONES, POPSIZE, GENERACIONES, TASA_MUTACION_C,
        TAMANHO_TORNEO, "exacta", "uniforme", "singleGene");

    return 0;
}