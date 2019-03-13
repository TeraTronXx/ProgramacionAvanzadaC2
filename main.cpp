#include <iostream>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

void ejercicio2(int** matrizNumeros, int m, int n);
void ejercicio1(int** matriz1, int** matriz2, int** matrizResult, int m, int n);
void ejercicio3(int** matrizDesordenada, int** matrizDesordenadaParalelo, int m, int n);


void ejercicio1(int** matriz1, int** matriz2, int** matrizResult, int m, int n){

    unsigned t0, t1;
    double tiempoTotal=0.0f;
    double tiempoMedio=0.0f;
    double time;

///paralelizacion 1.1 2,8s
for (int t=0; t<10; ++t){
    t0 = clock();
            #pragma omp parallel for collapse(3)
            for(int i=0; i<m; ++i)
                for(int j=0; j<n; ++j)
                    for(int z=0; z<n; ++z)
                        matrizResult[i][j] += matriz1[i][z] * matriz2[z][j];
    t1 = clock();
        time = (double(t1-t0)/CLOCKS_PER_SEC);
        tiempoTotal += time;
}
tiempoMedio = tiempoTotal /10;
    cout << "Paralelo con collapse: " << tiempoMedio << endl;
tiempoTotal = 0.0f;
tiempoMedio = 0.0f;

///paralelizacion 1.2. 2,8s
for (int t=0; t<10; ++t){
    t0 = clock();
        #pragma omp parallel
        {
            #pragma omp for
            for(int i=0; i<m; ++i)
                for(int j=0; j<n; ++j)
                    for(int z=0; z<n; ++z)
                        matrizResult[i][j] += matriz1[i][z] * matriz2[z][j];
        }
    t1 = clock();

        time = (double(t1-t0)/CLOCKS_PER_SEC);
        tiempoTotal += time;
}
tiempoMedio = tiempoTotal /10;
cout << "Paralelo normal: " << tiempoMedio << endl;
tiempoTotal = 0.0f;
tiempoMedio = 0.0f;


///serial. 9,4s
for (int t=0; t<10; ++t){
    t0 = clock();
        for(int i=0; i<m; ++i)
                for(int j=0; j<n; ++j)
                    for(int z=0; z<n; ++z)
                        matrizResult[i][j] += matriz1[i][z] * matriz2[z][j];

        t1 = clock();

        time = (double(t1-t0)/CLOCKS_PER_SEC);
        tiempoTotal += time;
}
tiempoMedio = tiempoTotal /10;
cout << "Serial: " << time << endl;
tiempoTotal = 0.0f;
tiempoMedio = 0.0f;


///paralelizacion 2. data race, usando critical. 45,4s
for (int t=0; t<3; ++t){
    t0 = clock();

    #pragma omp parallel
        {
            #pragma omp for
            for(int i=0; i<m; ++i){
                for(int j=0; j<n; ++j){
                    for(int z=0; z<n; ++z){
                        #pragma omp critical
                        matrizResult[i][j] += matriz1[i][z] * matriz2[z][j];
                    }
                }
            }
        }
        t1 = clock();

        time = (double(t1-t0)/CLOCKS_PER_SEC);
        tiempoTotal += time;
}
tiempoMedio = tiempoTotal /3;
cout << "Data Race (critical): " << time << endl;
tiempoTotal = 0.0f;
tiempoMedio = 0.0f;

///paralelizacion 3. data race, usando atomic. 2,8s
for (int t=0; t<10; ++t){
    t0 = clock();
    #pragma omp parallel
        {
            #pragma omp for
            for(int i=0; i<m; ++i){
                for(int j=0; j<n; ++j){
                    for(int z=0; z<n; ++z){
                        #pragma omp atomic
                        matrizResult[i][j] += matriz1[i][z] * matriz2[z][j];
                    }
                }
            }
        }

        t1 = clock();

        time = (double(t1-t0)/CLOCKS_PER_SEC);
        tiempoTotal += time;
}
tiempoMedio = tiempoTotal /10;
cout << "Data Race (atomic): " << time << endl;
tiempoTotal = 0.0f;
tiempoMedio = 0.0f;


///paralelizacion 4. data race, usando reduction de la matrizResult. 1,8s
for (int t=0; t<10; ++t){
    t0 = clock();
    float aux = 0.0;
    #pragma omp parallel
        {
            #pragma omp for reduction (+:aux)
            for(int i=0; i<m; ++i){
                for(int j=0; j<n; ++j){
                    for(int z=0; z<n; ++z){
                       aux += matriz1[i][z] * matriz2[z][j];
                    }
                    matrizResult[i][j] = aux;
                }
            }
        }
        t1 = clock();

        time = (double(t1-t0)/CLOCKS_PER_SEC);
        tiempoTotal += time;
}
tiempoMedio = tiempoTotal /10;
cout << "Data Race (Reduction): " << time << endl;
tiempoTotal = 0.0f;
tiempoMedio = 0.0f;

}


void ejercicio2(int** matrizNumeros, int m, int n){
 unsigned t0, t1;
 srand (time(NULL));
    int numMayor = 0;
    int numMayorParalelo = 0;
//    while (numMayor == numMayorParalelo){  //DESCOMENTAR PARA COMPROBAR EN BUCLE QUE SIEMPRE SE OBTIENE EL MISMO RESULTADO
    for(int i=0; i<m; ++i)
            for(int j=0; j<n; ++j)
                matrizNumeros[i][j] = rand() % 30000 + 1;

t0 = clock();
     for(int i=0; i<m; ++i){
            for(int j=0; j<n; ++j){
                if(matrizNumeros[i][j] > numMayor)
                    numMayor = numMayor > matrizNumeros[i][j] ? numMayor : matrizNumeros[i][j];

                    //numMayor = matrizNumeros[i][j];
            }
     }
t1 = clock();
    double time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Numero mayor (serial) en: "  << time << endl;

t0 = clock();
     #pragma omp parallel
    {
        #pragma omp for reduction(max:numMayorParalelo) collapse(2)
         for(int i=0; i<m; ++i){
                for(int j=0; j<n; ++j){
                    if(matrizNumeros[i][j] > numMayorParalelo){ //decir que si situamos el critical dentro del if, falla de igual manera.
                            numMayorParalelo = numMayorParalelo > matrizNumeros[i][j] ? numMayorParalelo : matrizNumeros[i][j];
                       // numMayorParalelo = matrizNumeros[i][j];
                    }

                }
         }
    }
t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Numero mayor (paralelo) en: " << time << endl;


    cout << "serie: " << numMayor << endl;
    cout << "paralelo: " << numMayorParalelo << endl;

    if(numMayor != numMayorParalelo){
        cout << "Fail!" << endl;
     //   break;  //DESCOMENTAR PARA EL WHILE TAMBIEN
    }
    else{
        cout << "Success!" << endl;
    }

    for(int i=0; i<m; ++i)
            for(int j=0; j<n; ++j)
                matrizNumeros[i][j] = 0;

    numMayor = 0;
    numMayorParalelo = 0;
  //  }  //FIN DEL BUCLE WHILE
}


void ejercicio3(int** matrizDesordenada, int** matrizDesordenadaParalelo, int m, int n){
 unsigned t0, t1;
 srand (time(NULL));
  double time;
    int mayor = 0;
    for(int i=0; i<m; ++i)
            for(int j=0; j<n; ++j)
                matrizDesordenada[i][j] = rand() % 89 + 10;

    matrizDesordenadaParalelo = matrizDesordenada;

t0 = clock();
     for(int i=0; i<m; i++)
     {
        for(int j=0; j<n; j++)
        {
            for(int x=0; x<m;x++)
            {
                for(int y=0; y<n; y++)
                {
                    if(matrizDesordenada[i][j]>matrizDesordenada[x][y])
                    {
                        mayor=matrizDesordenada[i][j];
                        matrizDesordenada[i][j]=matrizDesordenada[x][y];
                        matrizDesordenada[x][y]=mayor;
                    }
                }
            }
         }
    }

    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Ordenado(serial) en: " << time << endl;

    t0 = clock();
    #pragma omp parallel
    {
        #pragma omp for collapse(4)
         for(int i=0; i<m; i++)
         {
            for(int j=0; j<n; j++)
            {
                for(int x=0; x<m;x++)
                {
                    for(int y=0; y<n; y++)
                    {
                        if(matrizDesordenadaParalelo[i][j]>matrizDesordenadaParalelo[x][y])
                        {
                            mayor=matrizDesordenadaParalelo[i][j];
                            matrizDesordenadaParalelo[i][j]=matrizDesordenadaParalelo[x][y];
                            matrizDesordenadaParalelo[x][y]=mayor;
                        }
                    }
                }
             }
        }
    }
    t1 = clock();
    time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Ordenado(paralelo) en: " << time << endl;



   /* for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            cout << matrizDesordenadaParalelo[i][j] << " ";
        }
        cout << endl;
    } */ //DESCOMENTAR PARA VER IMPRESO EL RESULTADO


}

int main()
{
    cout << "Ejercicio de Programacion Avanzada. Luis Miguel Pinto." << endl << endl;
bool stop = false;
    while(!stop){
        int m1 = 1000; //para el ejercicio1
        int n1 = 1000;

        int m2 = 10000; //para el ejercicio2
        int n2 = 10000;

        int m3 = 200;
        int n3 = 200;
        //Introducción por parametro de los valores de la matriz
        int **matriz1 = new int*[m1];
        int **matriz2 = new int*[m1];
        int **matrizNumeros = new int*[m2];
        int **matrizResult = new int*[m1];
        int **matrizDesordenada = new int*[m3];
        int **matrizDesordenadaParalelo = new int*[m3];

        for (int i = 0; i < m1; i++) {
            matriz1[i] = new int[n1];
            matriz2[i] = new int[n1];
            matrizResult[i] = new int[n1];
        }
        for (int i = 0; i < m2; i++) {
            matrizNumeros[i] = new int[n2];
        }
        for (int i = 0; i < m3; i++) {
            matrizDesordenada[i] = new int[n3];
            matrizDesordenadaParalelo[i] = new int[n3];
        }

        for (int i=0; i<m1; ++i){
            for (int j = 0; j<n1; ++j){
                matriz1[i][j] = 2;
                matriz2[i][j] = 3;
            }
        }
        int respuesta;



        cout << "1- Para ejecutar el ejercicio 1, introduzca el caracter 1. Multiplicar matrices en paralelo." << endl;
        cout << "2- Para ejecutar el ejercicio 2, introduzca el caracter 2. Obtener el mayor valor dentro de una matriz rellenada aleatoriamente." << endl;
        cout << "3- Para ejecutar el ejercicio 3, introduzca el caracter 3. Ordenar una matriz rellenada aleatoriamente de mayor a menor." << endl;
        cout << "0- Cierre de la aplicacion." << endl;
        cout << "Texto: ";

        cin >> respuesta;

        system("CLS");
        switch(respuesta){
            case 1:
                cout << "Ejercicio de Programacion Avanzada. Luis Miguel Pinto." << endl << endl;
                ejercicio1(matriz1, matriz2, matrizResult, m1, n1);
                cout << endl << endl;
                break;
            case 2:
                cout << "Ejercicio de Programacion Avanzada. Luis Miguel Pinto." << endl << endl;
                ejercicio2(matrizNumeros, m2, n2);
                cout << endl << endl;
                break;
            case 3:
                cout << "Ejercicio de Programacion Avanzada. Luis Miguel Pinto." << endl << endl;
                ejercicio3(matrizDesordenada, matrizDesordenadaParalelo, m3, n3);
                cout << endl << endl;
                break;
            case 0:
                stop = true;
                break;
            default:
                cout << "Ejercicio de Programacion Avanzada. Luis Miguel Pinto." << endl << endl;
                cout << "La seleccion no corresponde a ninguna opcion" << endl << endl << endl;
                break;
        }

        delete[] matriz1;
        delete[] matriz2;
        delete[] matrizResult;
        delete[] matrizNumeros;
        delete[] matrizDesordenada;
        delete[] matrizDesordenadaParalelo;
    }
}
