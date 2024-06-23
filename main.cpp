/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: user
 *
 * Created on 22 de junio de 2024, 06:41 PM
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Individuo {
    vector <int> cromosoma;
    double fitness;

    Individuo(vector <int> cromo, double fit) : cromosoma(cromo), fitness(fit) {};
};

vector <Individuo> inicializarPoblacion(int popsize, int numItems,vector<int> anaqueles, vector <int> items, int alturaMax, int numAnaqueles) {
    vector <Individuo> poblacion;
    for (int i = 0; i < popsize; i++) {
        vector <int> cromo(numItems, 0);
        vector<int> alturasAnaqueles(numAnaqueles,0);       //Inicializar vector para analizar restringir la poblacion por alturas
        
        for (int j = 0; j < numItems; j++) {
            bool asignado = false;
            int intentos=0;                       // Colocar un maximo de intentos a probar por item (para evitar bucles infinitos)
            while (!asignado && intentos<20) {
                int indiceAnaquel = rand() % (anaqueles.size());    // Seleccionar un anaquel al azar
                cout<<indiceAnaquel<<" - "<<alturasAnaqueles[indiceAnaquel]<<" - "<<items[j]<<endl;
                if (alturasAnaqueles[indiceAnaquel] + items[j] <= alturaMax) {
                    cromo[j] = indiceAnaquel;                       // Asignar item al anaquel
                    alturasAnaqueles[indiceAnaquel] += items[j];    // Actualizar la altura del anaquel
                    asignado = true;
                }
                intentos++;
            }
            
        }
        cout<<"------------------------"<<endl;
        poblacion.emplace_back(cromo, 0);
    }
    return poblacion;
//    vector <Individuo> poblacion;
//    for (int i = 0; i < popsize; i++) {
//        vector <int> cromo(numItems);
//        for (int j = 0; j < numItems; j++) {
//            /*
//            int indxAnaquel = rand() % (anaqueles.size());
//            if (anaqueles[indxAnaquel] + items[j] <= alturaMax) {
//                cromo[j] = indxAnaquel;
//            }
//            */
//            cromo[j] = rand() % (anaqueles.size()+1);
//        }
//        poblacion.emplace_back(cromo, 0);
//    }
//    return poblacion;
}

int buscarMejorIndividuo(vector <Individuo> poblacion) {
    double mejorFitness = 0;
    int posMejorFitness = 0;
    for (int i = 0; i < poblacion.size(); i++) {
        if (poblacion[i].fitness > posMejorFitness) {
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

void calcularFitness(Individuo& individuo, vector <int> items, int numAnaqueles, int alturaMax) {
    int sumaAltura, sumaAlturaTotal = 0;
    for (int i = 1; i <= numAnaqueles; i++) {
        sumaAltura = 0;
        for (int j = 0; j < items.size(); j++) {
            if (individuo.cromosoma[j] == i)
                sumaAltura += items[j];
        }
        if (sumaAltura > alturaMax) {
            individuo.fitness = 0;
            return;
        }
        sumaAlturaTotal += sumaAltura;
    }

    int espacioLibre = (alturaMax * numAnaqueles) - sumaAlturaTotal;
    double fitness = (espacioLibre != 0) ? ((double)sumaAlturaTotal / espacioLibre) : 2147483647;
    individuo.fitness = fitness;
}

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

pair <Individuo, Individuo> cruceMultipunto(Individuo& padre, Individuo& madre) {
    int tamanho = padre.cromosoma.size();
    vector <int> hijoCromo(tamanho);
    vector <int> hijaCromo(tamanho);
    
    return make_pair(Individuo(hijoCromo, 0), Individuo(hijaCromo, 0));
}


void mutacion_single_gene(Individuo& individuo, int numAnaqueles) {
    int posicion = rand() % individuo.cromosoma.size();
    individuo.cromosoma[posicion] = rand() % (numAnaqueles + 1);
}


void evaluarPoblacion(vector <Individuo>& poblacion, vector <int> items, int numAnaqueles, int alturaMax) {
    for (int i = 0; i < poblacion.size(); i++)
        calcularFitness(poblacion[i], items, numAnaqueles, alturaMax);
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

void imprimeSolucion(Individuo& individuo, vector <int> items, int numAnaqueles) {
    for (int i = 1; i <= numAnaqueles; i++) {
        cout << "Anaquel " << i << ": ";
        for (int j = 0; j < items.size(); j++) {
            if (individuo.cromosoma[j] == i)
                cout << items[j] << " ";
        }
        cout << endl;
    }
    cout << "Valor de fitness: " << individuo.fitness << endl;
}

void algoritmoGenetico(vector <Individuo>& poblacion, vector <int> items, int numAnaqueles, int alturaMax, int generaciones, double tasaMutacion, int tamanhoTorneo) {
    int popsize = poblacion.size();
    evaluarPoblacion(poblacion, items, numAnaqueles, alturaMax);
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

            pair <Individuo, Individuo> hijos = cruceUniforme(poolCruces[k].first, poolCruces[k].second);
            //pair <Individuo, Individuo> hijos = cruceMultipunto(poolCruces[k].first, poolCruces[k].second);
            
            if ((double)rand() / RAND_MAX < tasaMutacion) {
                mutacion_single_gene(hijos.first, numAnaqueles);

            }
            if ((double)rand() / RAND_MAX < tasaMutacion) {
                mutacion_single_gene(hijos.second, numAnaqueles);

            }


            poblacionDescendencia.push_back(hijos.first);
            poblacionDescendencia.push_back(hijos.second);
        }

        evaluarPoblacion(poblacionDescendencia, items, numAnaqueles, alturaMax);
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
//Mutacion permutacion por inversion - diapo 21
//-Implementar el cruzamiento multipunto
//-Generar items aleatorios

//Mateo
//poblacion inical con restriccion

//Jairo
//Generar arreglo de cajas que calcen perfectamente
//Hacer las 10 ejecuciones

//Nicolas
//Seleccion de sobrevivientes no tan heuristico



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

    //vector <int> items = { 30,90,120,85,30,45,70,60,70 };
    vector <int> items = { 30,90,120,85,30,45,70,60,70,20,10,120 };
    //vector <int> items = { 150, 150, 150, 150, 150};
    const int NUM_ITEMS = items.size();
    cout<<"Tamaño de items: "<<NUM_ITEMS<<endl;
    vector<int> anaqueles(NUM_ANAQUELES, ALTURA_MAX);
    vector <Individuo> poblacion = inicializarPoblacion(POPSIZE, NUM_ITEMS, anaqueles, items, ALTURA_MAX,NUM_ANAQUELES);

    algoritmoGenetico(poblacion, items, NUM_ANAQUELES, ALTURA_MAX, GENERACIONES, TASA_MUTACION_C, TAMANHO_TORNEO);

    return 0;
}