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

const int TAMANO_MEMORIA = 4096; // 4 MB
const int MIN_SPLIT = 32; // 32 KB

// ParÃ¡metros configurables
int quantumProceso;
int tamanoMaximoProceso; // Por ejemplo, 1024 KB
int quantumMaximoSistema;

/*int quantumProceso = 5;
int tamanoMaximoProceso = 1024; // Por ejemplo, 1024 KB
int quantumMaximoSistema = 10;
*/

// ParÃ¡metros de estadÃ­sticas
int procesosTotales = 0;
int procesosFinalizados = 0;
int procesosNoFinalizados = 0;
int procesosRechazados = 0;

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
        bloquesLibres.push_back(raiz); // El bloque raÃ­z estÃ¡ inicialmente libre
    }

    ~BuddySystem() {
        delete raiz; // Asegura la liberaciÃ³n adecuada de la memoria
    }

    Bloque* dividirBloque(Bloque* nodo, int tamano) {
        if (!nodo || tamano > nodo->tamano) {
            return nullptr;
        }

        // Detener si el bloque actual es suficientemente pequeÃ±o
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
        // Dividir primero el bloque mÃ¡s grande posible
        dividirBloque(raiz, tamano);

        // Luego, intentar asignar
        return asignarBloque(raiz, tamano);
    }

    void liberarBloque(Bloque* bloque) {
        if (!bloque) return;
        bloque->libre = true;
        fusionarBloques(bloque);
    }

    void mostrarEstado(Bloque* nodo, string prefijo = "") {
        if (!nodo) return;

        cout << prefijo << "[" << nodo->tamano << ", " << (nodo->libre ? "libre" : "ocupado") << "]" << endl;

        mostrarEstado(nodo->izquierda, prefijo + "I-");
        mostrarEstado(nodo->derecha, prefijo + "D-");
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
        if (!cola.empty()) {
            Proceso& procesoActual = cola.front();
            cout << "Proceso en ejecucion: ID=" << procesoActual.getId() << endl;

            // Simular ejecuciÃ³n...
            procesoActual.setQuantumRestante(procesoActual.getQuantumRestante() - quantum);

            if (procesoActual.getQuantumRestante() <= 0) {
                // Proceso finalizado
                procesosFinalizados++;
                cola.pop();
            } else {
                // Mover al final de la cola
                cola.push(procesoActual);
                cola.pop();
            }
        }
    }

    // MÃ©todo para eliminar procesos finalizados (si es necesario)
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

// Clase SimulaciÃ³n
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
        int quantum = rand() % quantumSistema + 1;
        Proceso proceso(tamano1, quantum);
        procesosTotales++;
        return proceso;
    }

    void ejecutar() {
        while (ejecutarSimulacion) {
            Proceso proceso = generarProcesoAleatorio();
            cout << "Proceso generado: [" << proceso.getId() << ", " << proceso.getTamano() << ", " << proceso.getQuantum() << "]" << endl;

            if (memoriaDisponible >= proceso.getTamano()) {
                // Asignar memoria al proceso
                Bloque* bloqueAsignado = buddySystem.asignar(proceso.getTamano());
                buddySystem.mostrarEstadoCompleto();
                if (bloqueAsignado != nullptr) {
                    memoriaDisponible -= proceso.getTamano();
                    cout << "Memoria asignada al Proceso " << proceso.getId() << " en un bloque de " << bloqueAsignado->tamano << " KB" << endl;

                    roundRobin.agregarProceso(proceso);
                    roundRobin.ejecutar();
                    cout << "Ejecutando Round Robin... (pendiente de impresion)" << endl;

                    memoriaDisponible += proceso.getTamano();
                    cout << "Memoria liberada del Proceso " << proceso.getId() << endl;
                    // Condensar memoria
                    buddySystem.condensarMemoriaCompleta();
                    cout << "Pendiente la impreson de la condensacion de memoria (solo falta imprimir)" << endl;
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
        }
    }

    // MÃ©todo para terminar la simulaciÃ³n
    void detenerSimulacion() {
        ejecutarSimulacion = false;
    }
};

void gotoxy(int x,int y){
    HANDLE hcon;
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y= y;
    SetConsoleCursorPosition(hcon,dwPos);
}

void portada(){
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    //TamaÃ±o del marco
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
    string intro="*ENTER PARA COMENZAR*";

    //Obtener el ancho y alto de la consola
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    int consoleWidth=consoleInfo.srWindow.Right-consoleInfo.srWindow.Left+1;
    int consoleHeight=consoleInfo.srWindow.Bottom-consoleInfo.srWindow.Top+1;

    //Calcular la posiciÃ³n de inicio para centrar la pantalla
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
                // Cambiar el color del marco en cada iteraciÃ³n
                SetConsoleTextAttribute(hConsole, 15);
                cout<<char(178);
            } else {
                cout<<" ";
            }
        }
        cout<<endl;
    }

    SetConsoleTextAttribute(hConsole,10); //color
    gotoxy(startX+11, startY+3); //posiciÃ³n
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
    cout<<"MODULO DE CONFIGURACION DE PARAMETROS";
    cout<<endl<<endl<<"Cuanto de procesamiento asignado a cada proceso: ";
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

// FunciÃ³n principal
int main() {
    srand(time(NULL));
    portada();
    config();
    Simulacion simulacion(TAMANO_MEMORIA, MIN_SPLIT, quantumMaximoSistema, quantumProceso, tamanoMaximoProceso);
    simulacion.ejecutar();
    system("pause");
    return 0;
}