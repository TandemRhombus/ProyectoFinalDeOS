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
#include <numeric>  // Para std::accumulate

using namespace std;

struct Bloque {
    int tamano;
    bool libre;
    Bloque *izquierda, *derecha, *padre;

    Bloque(int t, Bloque* p = nullptr) : tamano(t), libre(true), izquierda(nullptr), derecha(nullptr), padre(p) {}

    ~Bloque() {
        delete izquierda;
        delete derecha;
    }
};

const int MIN_SPLIT = 32; // 32 KB
vector<int> tiemposDeProcesos;
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
    Bloque* bloqueAsignado;
    int tiempoInicio;       // Tiempo cuando el proceso es creado
    int tiempoFinalizacion; // Tiempo cuando el proceso finaliza
public:

    // Constructor modificado y métodos para manejar bloqueAsignado
    Proceso(int tam, int q) : id(contadorID++), tamano(tam), quantum(q), quantumRestante(q), bloqueAsignado(nullptr), tiempoInicio(clock()) {}

    void finalizarProceso() {
        tiempoFinalizacion = clock();
    }

    int getTiempoTotal() const {
        return tiempoFinalizacion - tiempoInicio;
    }

    void setBloqueAsignado(Bloque* bloque) {
        bloqueAsignado = bloque;
    }

    Bloque* getBloqueAsignado() const {
        return bloqueAsignado;
    }

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
    Bloque* raiz = nullptr;
    const int tamanoMinimoSplit;
    vector<Bloque*> bloquesLibres; // Lista de bloques libres

    Bloque* asignarBloque(Bloque* nodo, int tamano) {
        if (!nodo || tamano > nodo->tamano) {
            return nullptr;
        }

        // Si el bloque ya está dividido, intenta asignar en los hijos
        if (nodo->izquierda && nodo->derecha) {
            Bloque* bloqueAsignado = asignarBloque(nodo->izquierda, tamano);
            if (bloqueAsignado) {
                return bloqueAsignado;
            }
            return asignarBloque(nodo->derecha, tamano);
        }

        // Si el bloque está libre y es del tamaño adecuado, asignarlo
        if (nodo->libre && (nodo->tamano == tamano || nodo->tamano / 2 < tamano)) {
            nodo->libre = false;
            return nodo;
        }

        // Si el bloque es demasiado grande y está libre, dividirlo
        if (nodo->libre && nodo->tamano / 2 >= tamano && nodo->tamano / 2 >= MIN_SPLIT) {
            nodo->izquierda = new Bloque(nodo->tamano / 2, nodo); // Establecer el padre del nuevo bloque
            nodo->derecha = new Bloque(nodo->tamano / 2, nodo);   // Establecer el padre del nuevo bloque
            return asignarBloque(nodo->izquierda, tamano);
        }

        return nullptr; // No se pudo asignar el bloque
    }

    bool sonHermanos(Bloque* nodo1, Bloque* nodo2) {
        return nodo1 && nodo2 && nodo1->padre == nodo2->padre;
    }

    void fusionarBloques(Bloque* nodo) {
        if (!nodo || !nodo->izquierda || !nodo->derecha) return;

        // Condensa los hijos primero
        fusionarBloques(nodo->izquierda);
        fusionarBloques(nodo->derecha);

        // Verificar si ambos hijos están libres y no tienen hijos
        if (nodo->izquierda->libre && nodo->derecha->libre &&
            !nodo->izquierda->izquierda && !nodo->izquierda->derecha &&
            !nodo->derecha->izquierda && !nodo->derecha->derecha) {

            // Eliminar y fusionar los hijos
            delete nodo->izquierda;
            delete nodo->derecha;
            nodo->izquierda = nullptr;
            nodo->derecha = nullptr;
            nodo->libre = true;
        }
    }


    // Funcion que condensa la memoria y muestra como lo va haciendo en consola
    void condensar(Bloque* nodo) {
        fusionarBloques(nodo);
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

    bool puedeAsignar(Bloque* nodo, int tamano) {
        if (!nodo) {
            return false;
        }

        // Si el bloque actual está libre y es del tamaño adecuado, devuelve true
        if (nodo->libre && nodo->tamano >= tamano) {
            return true;
        }

        // Si no, verifica en los bloques hijos
        return puedeAsignar(nodo->izquierda, tamano) || puedeAsignar(nodo->derecha, tamano);
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
        return asignarBloque(raiz, tamano);
    }

    void liberarBloque(Bloque* bloque) {
        if (!bloque) return;
        bloque->libre = true;
        condensar(raiz); // Comienza la condensación desde la raíz
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

    // Getters y setters
    Bloque* getRaiz() const { return raiz; }
    int getTamanoMinimoSplit() const { return tamanoMinimoSplit; }

    void setRaiz(Bloque* r) { raiz = r; }

};

// Clase Round Robin
class RoundRobin {
private:
    queue<Proceso> cola;
    int quantum;

public:
    // Constructor modificado
    RoundRobin(int q) : quantum(q) {}

    // Metodo para imprimir la cola de procesos con el formato [ID, tamano, quantum]
    void imprimirCola(){
        queue<Proceso> colaTemp;
        cout<<endl<<"Cola de procesos: ";
        while(!cola.empty()){
            Proceso proceso = cola.front();
            cola.pop();
            cout<<"["<<proceso.getId()<<", "<<proceso.getTamano()<<", "<<proceso.getQuantumRestante()<<"] ";
            colaTemp.push(proceso);
        }
        swap(cola, colaTemp);
    }

    bool colaVacia() const {
        return cola.empty();
    }

    void agregarProceso(Proceso proceso) {
        cola.push(proceso);
    }

    void agregarProcesoAlFrente(Proceso proceso) {
        //nuevos procesos se agregan al frente de la cola
        queue<Proceso> colaTemp;
        colaTemp.push(proceso);
        while(!cola.empty()){
            Proceso proceso = cola.front();
            cola.pop();
            colaTemp.push(proceso);
        }
        swap(cola, colaTemp);
    }

    // Método para eliminar procesos finalizados (si es necesario)
    void eliminarProcesosFinalizados() {
        queue<Proceso> colaTemp;
        while (!cola.empty()) {
            Proceso proceso = cola.front();
            cola.pop();
            if (proceso.getQuantumRestante() > 0) {
                colaTemp.push(proceso);
            } else {
                procesosFinalizados++;
            }
        }
        swap(cola, colaTemp);
    }

    void procesarQuantum(BuddySystem& buddySystem, int& memoriaDisponible) {
        if (cola.empty()) return;

        Proceso& procesoActual = cola.front();
        cout << endl << "Proceso en ejecucion: [" << procesoActual.getId() << ", " << procesoActual.getTamano() << ", " << procesoActual.getQuantumRestante() << "]" << endl;

        // Restar el quantum del sistema al quantum restante del proceso
        procesoActual.setQuantumRestante(procesoActual.getQuantumRestante() - quantum);
        if(procesoActual.getQuantumRestante() > 0){
            cout << "Quantum restante del proceso: " << procesoActual.getQuantumRestante() << endl;
        } else {
            cout << "Quantum restante del proceso: 0" << endl;
            cout << "Descarga de proceso..." << endl;
        }

        // Verificar si el proceso ha terminado
        if (procesoActual.getQuantumRestante() <= 0) {
            procesoActual.finalizarProceso();
            //anadir el tiempo de ejecucion del proceso a la lista de tiempos
            tiemposDeProcesos.push_back(procesoActual.getTiempoTotal());
            cout << endl << "Proceso finalizado: ID=" << procesoActual.getId() << endl;
            memoriaDisponible += procesoActual.getTamano(); // Liberar memoria
            buddySystem.liberarBloque(procesoActual.getBloqueAsignado());
            cola.pop(); // Quitar el proceso de la cola
            procesosFinalizados++;
        } else {
            // Si el proceso no ha terminado, moverlo al final de la cola
            cola.push(procesoActual);
            cola.pop();
        }
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
    queue<Proceso> colaEspera; // Cola de espera para procesos que no se pudieron asignar
    bool esperandoEspacio; // Indica si hay un proceso en espera por falta de espacio

public:
    Simulacion(int memoria, int split, int qSistema, int qProceso, int tamMaxProceso)
            : buddySystem(memoria, split), roundRobin(qSistema),
              memoriaDisponible(memoria), quantumSistema(qSistema),
              quantumProceso(qProceso), tamanoMaximoProceso(tamMaxProceso),
              ejecutarSimulacion(true), esperandoEspacio(false) {}

    Proceso generarProcesoAleatorio() {
        int tamano1 = rand() % tamanoMaximoProceso + 1;
        int quantum = rand() % quantumProceso + 1;
        Proceso proceso(tamano1, quantum);
        procesosTotales++;
        return proceso;
    }

    void ejecutar() {
        char velocidadTecla;
        int velocidad = 1;

        while (ejecutarSimulacion) {
            cout << endl << endl;
            // Control para finalizar la simulación
            if (kbhit()) {
                char tecla = getch();
                if (tecla == 'q') {
                    detenerSimulacion();
                    cout << "Simulación detenida. Mostrando estadísticas..." << endl;
                    // Mostrar estadísticas aquí
                    break;
                }
            }

            // Ejecutar el proceso en el frente de la cola con Round Robin
            roundRobin.procesarQuantum(buddySystem, memoriaDisponible);

            // Verificar si hay suficiente memoria libre para el nuevo proceso
            if (memoriaDisponible >= tamanoMaximoProceso) {
                // Intentar asignar un nuevo proceso
                Proceso proceso = generarProcesoAleatorio();
                cout << endl << "Proceso generado: [" << proceso.getId() << ", " << proceso.getTamano() << ", " << proceso.getQuantum() << "]" << endl;
                //verificar si hay algun bloque donde se pueda asignar el proceso
                if (buddySystem.puedeAsignar(buddySystem.getRaiz(), proceso.getTamano())) {
                    Bloque* bloqueAsignado = buddySystem.asignar(proceso.getTamano());
                    if (bloqueAsignado) {
                        cout <<"Proceso asignado en bloque de memoria: "<< bloqueAsignado->tamano<<" KB"<<endl;
                        proceso.setBloqueAsignado(bloqueAsignado);
                        memoriaDisponible -= proceso.getTamano();
                        roundRobin.agregarProcesoAlFrente(proceso);
                    }
                } else {
                    // Si no hay suficiente memoria libre, verificar si algún proceso en espera puede liberar espacio
                    while (!colaEspera.empty()) {
                        Proceso procesoEspera = colaEspera.front();
                        if (memoriaDisponible >= procesoEspera.getTamano()) {
                            Bloque* bloqueAsignado = buddySystem.asignar(procesoEspera.getTamano());
                            if (bloqueAsignado) {
                                colaEspera.pop();
                                procesoEspera.setBloqueAsignado(bloqueAsignado);
                                memoriaDisponible -= procesoEspera.getTamano();
                                roundRobin.agregarProcesoAlFrente(procesoEspera);
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }
                }
            } else {
                // Si no hay suficiente memoria libre, verificar si algún proceso en espera puede liberar espacio
                while (!colaEspera.empty()) {
                    Proceso procesoEspera = colaEspera.front();
                    if (memoriaDisponible >= procesoEspera.getTamano()) {
                        Bloque* bloqueAsignado = buddySystem.asignar(procesoEspera.getTamano());
                        if (bloqueAsignado) {
                            colaEspera.pop();
                            procesoEspera.setBloqueAsignado(bloqueAsignado);
                            memoriaDisponible -= procesoEspera.getTamano();
                            roundRobin.agregarProceso(procesoEspera);
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            } // Fin de la verificación de memoria libre

            // Mostrar el estado de la memoria
            buddySystem.mostrarEstadoCompleto();
            // Mostrar la cola de procesos
            roundRobin.imprimirCola();

            // Control de velocidad de la simulación
            if (kbhit()) {
                velocidadTecla = getch();
                if (velocidadTecla == '1') {
                    velocidad = 1;
                } else if (velocidadTecla == '2') {
                    velocidad = 2;
                }
            }

            if (velocidad == 1) {
                Sleep(2000); // Pausa de 2 segundos para visualizar la simulación
            } else if (velocidad == 2) {
                Sleep(1); // Pausa mínima para una ejecución más rápida
            }
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

    // Mostrar estadísticas
    cout << endl << endl << "Estadisticas:" << endl;
    cout << "Procesos totales: " << procesosTotales << endl;
    cout << "Procesos finalizados: " << procesosFinalizados << endl;
    cout << "Procesos no finalizados: " << procesosNoFinalizados << endl;
    cout << "Procesos rechazados: " << procesosRechazados << endl;
    int sumaTiempos = accumulate(tiemposDeProcesos.begin(), tiemposDeProcesos.end(), 0);
    double tiempoMedio = static_cast<double>(sumaTiempos) / tiemposDeProcesos.size();
    cout << "Tiempo medio de atencion: " << tiempoMedio << " ms." << endl;
    system("pause");
    return 0;
}