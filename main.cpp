#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <climits>

using namespace std;

struct Individuo {
    vector <int> cromosoma;
    double fitness;

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
    cout << "Valor de fitness: " << individuo.fitness << endl;
}

//================================ Inicializar Poblacion =======================================

void calcularFitness(Individuo& individuo, vector <int> items, int numAnaqueles, int alturaMax);

vector <Individuo> inicializarPoblacion(int popsize, int numItems, vector<int> anaqueles, vector <int> items, int alturaMax) {
    vector <Individuo> poblacion;
    for (int i = 0; i < popsize; i++) {
        vector <int> cromo(numItems, 0);
        vector<int> alturasAnaqueles(anaqueles.size(), 0);       //Inicializar vector para analizar restringir la poblacion por alturas
        for (int j = 0; j < numItems; j++) {
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
        }

        poblacion.emplace_back(cromo, 0);
        calcularFitness(poblacion[i], items, anaqueles.size(), alturaMax);
        imprimeSolucion(poblacion[i], items, anaqueles.size());
        
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
    vector <Individuo> siguientePoblacion;
    poblacion.insert(poblacion.end(), poblacionDescendencia.begin(), poblacionDescendencia.end());
    sort(poblacion.begin(), poblacion.end(), [](Individuo& a, Individuo& b) {
        return a.fitness > b.fitness;
        });
    for (int i = 0; i < tamanhoPoblacion; i++) {
        siguientePoblacion.push_back(poblacion[i]);
    }
    return siguientePoblacion;
}

//Cambie un poco el fitness ya que si sobraban cajas pero igual se llenaban los anaqueles igual daba el maximo
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

void algoritmoGenetico(vector <Individuo>& poblacion, vector <int> items, int numAnaqueles, int ALTURA_MAX,
    int generaciones, double tasaMutacion, int tamanhoTorneo, int puntosDeCorte) {
    int popsize = poblacion.size();
    evaluarPoblacion(poblacion, items, numAnaqueles, ALTURA_MAX);
    vector <int> mejorFitness;
    int posMejorFitness = buscarMejorIndividuo(poblacion);
    Individuo mejorIndividuo(poblacion[posMejorFitness].cromosoma, poblacion[posMejorFitness].fitness);
    mejorFitness.push_back(mejorIndividuo.fitness);

    cout << "Poblacion inicial, mejor fitness = " << mejorIndividuo.fitness << endl;

    for (int i = 0; i < generaciones; i++) {
        vector <pair <Individuo, Individuo>> poolCruces;
        for (int j = 0; j < (popsize / 2); j++) {
            poolCruces.push_back(make_pair(seleccionTorneo(poblacion, tamanhoTorneo), seleccionTorneo(poblacion, tamanhoTorneo)));
        }
        vector <Individuo> poblacionDescendencia;
        for (int k = 0; k < poolCruces.size(); k++) {

            //pair <Individuo, Individuo> hijos = cruceUniforme(poolCruces[k].first, poolCruces[k].second);
            pair <Individuo, Individuo> hijos = cruceMultipunto(poolCruces[k].first, poolCruces[k].second, puntosDeCorte);

            if ((double)rand() / RAND_MAX < tasaMutacion) {
                mutacion_single_gene(hijos.first, numAnaqueles);
                //mutacion_permutation_inversion(hijos.first);
            }
            if ((double)rand() / RAND_MAX < tasaMutacion) {
                mutacion_single_gene(hijos.second, numAnaqueles);
                //mutacion_permutation_inversion(hijos.second);
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

        if (i % 2 == 0) {
            cout << "Generacion " << setw(2) << i << ", Mejor Fitness: " << mejorIndividuo.fitness << endl;
        }
    }
    
    imprimeSolucion(mejorIndividuo, items, numAnaqueles);
}


//Tener 2 cruzamientos --
//Tener 2 mutaciones   --
//10 individuos        --
//Hacer 10 ejecuciones para sacar conclusiones
//Ver configuracion mas adecuada
//Poner conjuntos de cajas que entren exacto para ver efectividad



//Puede ser

//Krlos
//---Mutacion permutacion por inversion - diapo 21
//---Implementar el cruzamiento multipunto
//---Generar items aleatorios

//Mateo
//---poblacion inical con restriccion

//Jairo
//Generar arreglo de cajas que calcen perfectamente
//Hacer las 10 ejecuciones

//Nicolas
//Seleccion de sobrevivientes no tan heuristico

vector<int> generarItemsAleatorios(int ALTURA_MAX, int cantItems) {

    vector<int> items(cantItems);
    for (int i = 0; i < items.size(); i++) {
        items[i] = rand() % ALTURA_MAX + 1;
    }

    return items;
}

// Función para generar items que llenen perfectamente los anaqueles
vector<int> generarItemsPerfectos(int ALTURA_MAX, int NUM_ANAQUELES, int NUM_ITEMS) {
    srand(time(NULL));
    vector<int> items;

    int suma_anaqueles = ALTURA_MAX * NUM_ANAQUELES;

    // Genera un item grande para llenar un anaquel completo
    items.push_back(ALTURA_MAX);
    
    // Genera pares de items para llenar los restantes anaqueles
    for (int i = 1; i < NUM_ANAQUELES; ++i) {
        int item1 = (rand() % (ALTURA_MAX / 20) + 1) * 10;
        int item2 = ALTURA_MAX - item1;
        items.push_back(item1);
        items.push_back(item2);
    }

    // Ordena los items en orden decreciente
    sort(items.begin(), items.end(), greater<int>());

    return items;
}

int main(int argc, char** argv) {
    srand(time(0));

    const int NUM_ANAQUELES = 5;
    const int ALTURA_MAX = 150;

    const int POPSIZE = 10;
    const int GENERACIONES = 100;
    const double TASA_MUTACION_A = 0.0;
    const double TASA_MUTACION_B = 0.4;
    const double TASA_MUTACION_C = 0.8;
    const int TAMANHO_TORNEO = 3;
    const double TASA_CASAMIENTO = 0.5;
    const int PUNTOS_CORTE = 2;

    //vector <int> items = { 30,90,120,85,30,45,70,60,70 };
    //vector <int> items = { 30,90,120,85,30,45,70,60,70,20,10,120 };
    //vector <int> items = { 150, 150, 100, 150, 120, 20, 30, 50, 10, 10, 10};
    //vector <int> items = { 150, 150, 150, 150, 150};
    //const int NUM_ITEMS = items.size();

    //const int NUM_ITEMS = 9;
    //vector <int> items = generarItemsAleatorios(ALTURA_MAX, NUM_ITEMS);
    
    // Genera 10 items aleatorios que encaja en los anaqueles 
    const int NUM_ITEMS = 10;
    vector <int> items = generarItemsAleatorios(ALTURA_MAX, NUM_ITEMS);
    
    vector<int> anaqueles(NUM_ANAQUELES, ALTURA_MAX);
    vector <Individuo> poblacion = inicializarPoblacion(POPSIZE, NUM_ITEMS, anaqueles, items, ALTURA_MAX);

    algoritmoGenetico(poblacion, items, NUM_ANAQUELES, ALTURA_MAX, GENERACIONES, TASA_MUTACION_A, TAMANHO_TORNEO, PUNTOS_CORTE);

    return 0;
}
