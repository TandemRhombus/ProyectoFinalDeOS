#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <windows.h>
#include <conio.h>

using namespace std;

struct Bloque {
    int tamano;
    bool libre;
    Bloque *izquierda, *derecha;

    Bloque(int t) : tamano(t), libre(true), izquierda(nullptr), derecha(nullptr) {}

    ~Bloque() {
        delete izquierda;
        delete derecha;
    }
};

const int MIN_SPLIT = 32; // 32 KB

// Parámetros configurables
int TAMANO_MEMORIA;
int quantumProceso;
int tamanoMaximoProceso; // Por ejemplo, 1024 KB
int quantumMaximoSistema;

// Parámetros de estadi­sticas
int procesosTotales = 0;
int procesosFinalizados = 0;
int procesosNoFinalizados = 0;
int procesosRechazados = 0;

void gotoxy(int x,int y){
    HANDLE hcon;
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y= y;
    SetConsoleCursorPosition(hcon,dwPos);
}

// Clase Proceso
class Proceso {
private:
    int id;
    int tamano;
    int quantum;
    int quantumRestante;
    static int contadorID;
public:
    Proceso(int tam, int q) : id(contadorID++), tamano(tam), quantum(q), quantumRestante(q) {}

    int getId() const { return id; }
    int getTamano() const { return tamano; }
    int getQuantum() const { return quantum; }
    int getQuantumRestante() const { return quantumRestante; }
    void setQuantumRestante(int qRestante) { quantumRestante = qRestante; }
    void setTamano(int tam) { tamano = tam; }
    void setQuantum(int q) { quantum = q; }
};

int Proceso::contadorID = 1;

// Clase Buddy System
class BuddySystem {
private:
    Bloque* raiz;
    const int tamanoMinimoSplit;
    vector<Bloque*> bloquesLibres; // Lista de bloques libres

    Bloque* asignarBloque(Bloque* nodo, int tamano) {
        if (!nodo || tamano > nodo->tamano) {
            return nullptr;
        }

        if (nodo->libre && nodo->tamano >= tamano && (nodo->tamano / 2 < tamano || nodo->tamano / 2 < MIN_SPLIT)) {
            nodo->libre = false;
            return nodo;
        }

        if (!nodo->izquierda && nodo->tamano / 2 >= tamano) {
            nodo->izquierda = new Bloque(nodo->tamano / 2);
            nodo->derecha = new Bloque(nodo->tamano / 2);
        }

        Bloque* bloqueAsignado = asignarBloque(nodo->izquierda, tamano);
        if (bloqueAsignado) {
            return bloqueAsignado;
        }

        return asignarBloque(nodo->derecha, tamano);
    }

    void fusionarBloques(Bloque* nodo) {
        if (!nodo || !nodo->izquierda || !nodo->derecha) return;

        if (nodo->izquierda->libre && nodo->derecha->libre) {
            delete nodo->izquierda;
            delete nodo->derecha;
            nodo->izquierda = nodo->derecha = nullptr;
            nodo->libre = true;
            // Actualiza la lista de bloques libres
            actualizarListaBloquesLibres();
            fusionarBloques(raiz); // Intentar fusionar en niveles superiores
        }
    }

    // Funcion que condensa la memoria y muestra como lo va haciendo en consola
    void condensarMemoria(Bloque* nodo){
        if(!nodo) return;
        if(nodo->libre && nodo->tamano/2 < MIN_SPLIT){
            cout<<"Condensando memoria..."<<endl;
            cout<<"Memoria condensada"<<endl;
            return;
        }
        if(nodo->izquierda && nodo->derecha){
            if(nodo->izquierda->libre && nodo->derecha->libre){
                cout<<"Condensando memoria..."<<endl;
                cout<<"Memoria condensada"<<endl;
                delete nodo->izquierda;
                delete nodo->derecha;
                nodo->izquierda = nodo->derecha = nullptr;
                nodo->libre = true;
                actualizarListaBloquesLibres();
                condensarMemoria(raiz);
            }
        }
    }

    void actualizarListaBloquesLibres() {
        bloquesLibres.clear();
        agregarBloquesLibres(raiz);
    }

    void agregarBloquesLibres(Bloque* nodo) {
        if (!nodo) return;

        if (nodo->libre) {
            bloquesLibres.push_back(nodo);
        }

        agregarBloquesLibres(nodo->izquierda);
        agregarBloquesLibres(nodo->derecha);
    }

public:
    BuddySystem(int tamanoTotal, int splitMinimo) : tamanoMinimoSplit(splitMinimo) {
        raiz = new Bloque(tamanoTotal);
        bloquesLibres.push_back(raiz); // El bloque raí­z esta inicialmente libre
    }

    ~BuddySystem() {
        delete raiz; // Asegura la liberación adecuada de la memoria
    }

    Bloque* dividirBloque(Bloque* nodo, int tamano) {
        if (!nodo || tamano > nodo->tamano) {
            return nullptr;
        }

        // Detener si el bloque actual es suficientemente pequeño
        if (nodo->tamano / 2 < tamano || nodo->tamano / 2 < MIN_SPLIT) {
            return nodo;
        }

        // Crear hijos si no existen
        if (!nodo->izquierda) {
            nodo->izquierda = new Bloque(nodo->tamano / 2);
            nodo->derecha = new Bloque(nodo->tamano / 2);
        }

        // Solo dividir el lado necesario
        if (tamano <= nodo->izquierda->tamano) {
            return dividirBloque(nodo->izquierda, tamano);
        } else {
            return dividirBloque(nodo->derecha, tamano);
        }
    }

    Bloque* asignar(int tamano) {
        // Dividir primero el bloque más grande posible
        dividirBloque(raiz, tamano);

        // Luego, intentar asignar
        return asignarBloque(raiz, tamano);
    }

    void liberarBloque(Bloque* bloque) {
        if (!bloque) return;
        bloque->libre = true;
        fusionarBloques(bloque);
    }

    void mostrarEstado(Bloque* nodo) {
        if (!nodo) return;

        if (nodo->libre && nodo->tamano != TAMANO_MEMORIA) {
            cout << "[0," << nodo->tamano << ",0]";
        } else if (!nodo->libre) {
            cout << "[Asignado,"  << nodo->tamano << "]";
        }

        mostrarEstado(nodo->izquierda);
        mostrarEstado(nodo->derecha);
    }


    void mostrarEstadoCompleto() {
        mostrarEstado(raiz);
    }


    void condensarMemoriaCompleta(){
        condensarMemoria(raiz);
    }

    // Getters y setters
    Bloque* getRaiz() const { return raiz; }
    int getTamanoMinimoSplit() const { return tamanoMinimoSplit; }

    void setRaiz(Bloque* r) { raiz = r; }

};

// Clase Round Robin
class RoundRobin {
private:
    std::queue<Proceso> cola;
    int quantum;

public:
    RoundRobin(int q) : quantum(q) {}

    void agregarProceso(Proceso proceso) {
        cola.push(proceso);
    }

    void ejecutar() {
        while (!cola.empty()) {
            Proceso& procesoActual = cola.front();
            cout << "Proceso en ejecucion: ID=" << procesoActual.getId();
            cout<< endl <<"Quantum restante=" << procesoActual.getQuantumRestante() << endl;

            // Simular ejecución...
            procesoActual.setQuantumRestante(procesoActual.getQuantumRestante() - quantum);

            if (procesoActual.getQuantumRestante() <= 0) {
                // Proceso finalizado
                procesosFinalizados++;
                cout << "Quantum agotado para proceso ID=" << procesoActual.getId();
                cout << endl << "Proceso finalizado" << endl;
                cola.pop();
            } else {
                // Mover al final de la cola
                cola.push(procesoActual);
                cola.pop();
            }
        }
    }

    // Método para eliminar procesos finalizados (si es necesario)
    void eliminarProcesosFinalizados() {
        std::queue<Proceso> colaTemp;
        while (!cola.empty()) {
            Proceso proceso = cola.front();
            cola.pop();
            if (proceso.getQuantumRestante() > 0) {
                colaTemp.push(proceso);
            } else {
                procesosFinalizados++;
            }
        }
        std::swap(cola, colaTemp);
    }
};

// Clase Simulación
class Simulacion {
private:
    BuddySystem buddySystem;
    RoundRobin roundRobin;
    int memoriaDisponible;
    int quantumSistema;
    int quantumProceso;
    int tamanoMaximoProceso;
    bool ejecutarSimulacion;

public:
    Simulacion(int memoria, int split, int qSistema, int qProceso, int tamMaxProceso)
            : buddySystem(memoria, split), roundRobin(qSistema),
              memoriaDisponible(memoria), quantumSistema(qSistema),
              quantumProceso(qProceso), tamanoMaximoProceso(tamMaxProceso),
              ejecutarSimulacion(true) {}

    Proceso generarProcesoAleatorio() {
        int tamano1 = rand() % tamanoMaximoProceso + 1;
        int quantum = rand() % quantumProceso + 1;
        Proceso proceso(tamano1, quantum);
        procesosTotales++;
        return proceso;
    }

    void ejecutar() {
        char velocidadtecla;
        int velocidad = 1;
        while (ejecutarSimulacion) {
            // Control de finalizacion de la simulacion precionando una tecla
            if (kbhit()) {
                char tecla = getch();
                if (tecla == 'q') {
                    detenerSimulacion();
                    //Mostrar Estadiscicas
                    cout << endl << endl << "ESTADISTICAS" << endl;
                    cout << "Procesos Totales: " << procesosTotales << endl;
                    cout << "Procesos Finalizados: " << procesosFinalizados << endl;
                }
            }

            Proceso proceso = generarProcesoAleatorio();
            cout << setw(15) << left << "Proceso generado: [" << proceso.getId() << ", " << proceso.getTamano() << ", " << proceso.getQuantum() << "]" << endl;

            // Control de velocidad de la simulacion
            if (kbhit()) {
                velocidadtecla = getch();
                if (velocidadtecla == '1') {
                    velocidad = 1;
                } else if (velocidadtecla == '2') {
                    velocidad = 2;
                }
            }
            if (velocidad == 1) {
                Sleep(2000);
            } else if (velocidad == 2) {
                Sleep(1);
            }

            if (memoriaDisponible >= proceso.getTamano()) {
                // Asignar memoria al proceso
                Bloque *bloqueAsignado = buddySystem.asignar(proceso.getTamano());
                buddySystem.mostrarEstadoCompleto();
                if (bloqueAsignado != nullptr) {
                    memoriaDisponible -= proceso.getTamano();
                    cout << endl << endl <<"Memoria asignada al Proceso " << proceso.getId() << " en un bloque de "
                         << bloqueAsignado->tamano << " KB" << endl;

                    roundRobin.agregarProceso(proceso);
                    roundRobin.ejecutar();

                    memoriaDisponible += proceso.getTamano();
                    cout << "Memoria liberada del Proceso " << proceso.getId() << endl;
                    // Condensar memoria
                    buddySystem.condensarMemoriaCompleta();
                    // Eliminar procesos finalizados
                    roundRobin.eliminarProcesosFinalizados();
                    cout << endl << endl;
                } else {
                    cout << "No se pudo asignar memoria al Proceso " << proceso.getId() << std::endl;
                    procesosRechazados++;
                }
            } else {
                cout << "Memoria insuficiente para Proceso " << proceso.getId() << endl;
                procesosRechazados++;
            }
            cout << endl << endl;
            if(velocidad==1){
                cout << "Velocidad de simulacion: NORMAL" << endl;
            }else if(velocidad==2){
                cout << "Velocidad de simulacion: RAPIDA" << endl;
            }
            cout << "Presiona 'q' para terminar la simulacion" << endl;
            cout << endl << endl;
        }
    }

    // Método para terminar la simulación
    void detenerSimulacion() {
        ejecutarSimulacion = false;
    }
};

void portada(){
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    //Tamaño del marco
    const int ancho=100;
    const int alto=25;

    //Texto de la pantalla
    string carrera="<< I N G E N I E R I A  E N  S I S T E M A S  C O M P U T A C I O N A L E S >>";
    string materia="<< S I S T E M A S  O P E R A T I V O S >>";
    string proyecto="<< SYSTEM BUDDY - ROUND ROBIN >>";
    string integrantes="I N T E G R A N T E S";
    string nom1="Ivan Favela Ruvalcaba";
    string nom2="Sergio Emiliano Hernandez Villalpando";
    string nom3="Cinthia Edith Garcia de Luna";
    string nom4="Dario Miguel Moreno Gonzalez";
    string intro="ENTER PARA COMENZAR";

    //Obtener el ancho y alto de la consola
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    int consoleWidth=consoleInfo.srWindow.Right-consoleInfo.srWindow.Left+1;
    int consoleHeight=consoleInfo.srWindow.Bottom-consoleInfo.srWindow.Top+1;

    //Calcular la posición de inicio para centrar la pantalla
    int startX=(consoleWidth-ancho)/2;
    int startY=(consoleHeight-alto)/6;

    //Imprimir marco centrado
    for(int i=0; i<startY; i++){
        cout<<endl;
    }

    for(int i=0; i<alto; i++){
        for(int j=0; j<startX; j++){
            cout<<" ";
        }
        for(int j=0; j<ancho; j++){
            if(i==0 || i==alto-1 || j==0 || j==ancho-1){
                // Cambiar el color del marco en cada iteración
                SetConsoleTextAttribute(hConsole, 15);
                cout<<char(178);
            } else {
                cout<<" ";
            }
        }
        cout<<endl;
    }

    SetConsoleTextAttribute(hConsole,10); //color
    gotoxy(startX+11, startY+3); //posición
    cout<<carrera; //nombre
    SetConsoleTextAttribute(hConsole,12);
    gotoxy(startX+29, startY+6);
    cout<<materia;
    SetConsoleTextAttribute(hConsole,11);
    gotoxy(startX+34, startY +9);
    cout<<proyecto;
    SetConsoleTextAttribute(hConsole,6);
    gotoxy(startX+39, startY +12);
    cout<<integrantes;
    gotoxy(startX+39, startY+14);
    cout<<nom1;
    gotoxy(startX+31, startY +15);
    cout<<nom2;
    gotoxy(startX+36, startY +16);
    cout<<nom3;
    gotoxy(startX+36, startY +17);
    cout<<nom4;
    SetConsoleTextAttribute(hConsole,112);
    gotoxy(startX+39, startY +20);
    cout<<intro;
    SetConsoleTextAttribute(hConsole,0);
    cout<<endl;
    cout<<endl;
    gotoxy(0,25);
    getch();
    system("cls");
    SetConsoleTextAttribute(hConsole, 15);
}

void config(){
    int x=40, y=0, opc;
    gotoxy(x,y);
    cout<<"MODULO DE CONFIGURACION DE PARAMETROS";

    cout<<endl<<endl<<"Tamanos de memoria";
    cout<<endl<<"1. 1MB";
    cout<<endl<<"2. 4MB";
    cout<<endl<<"3. 8MB";

    do{
        cout<<endl<<"Elige un tamano: ";
        cin>>opc;
        switch(opc){
            case 1: TAMANO_MEMORIA=1024;
                break;
            case 2: TAMANO_MEMORIA=4096;
                break;
            case 3: TAMANO_MEMORIA=8192;
                break;
            default: cout<<endl<<"Opcion no disponible"<<endl;
                break;
        }
    }while(opc<1 || opc>3);

    cout<<endl<<"Cuanto de procesamiento asignado a cada proceso: ";
    cin>>quantumProceso;
    cout<<endl<<"Tamano maximo de memoria de proceso: ";
    cin>>tamanoMaximoProceso;
    cout<<endl<<"Tamano maximo del cuanto de procesamiento por proceso (cuanto del sistema): ";
    cin>>quantumMaximoSistema;
    cout<<endl<<"Configuracion completada, ENTER para continuar";
    getch();
    fflush(stdin);
    system("cls");
}

// Función principal
int main() {
    srand(time(NULL));
    portada();
    config();
    Simulacion simulacion(TAMANO_MEMORIA, MIN_SPLIT, quantumMaximoSistema, quantumProceso, tamanoMaximoProceso);
    simulacion.ejecutar();
    system("pause");
    return 0;
}