#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>

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

// Parámetros configurables
int quantumProceso = 5;
int tamanoMaximoProceso = 1024; // Por ejemplo, 1024 KB
int quantumMaximoSistema = 10;

// Parámetros de estadísticas
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
        bloquesLibres.push_back(raiz); // El bloque raíz está inicialmente libre
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

    void mostrarEstado(Bloque* nodo, string prefijo = "") {
        if (!nodo) return;

        cout << prefijo << "[" << nodo->tamano << ", " << (nodo->libre ? "libre" : "ocupado") << "]" << endl;

        mostrarEstado(nodo->izquierda, prefijo + "I-");
        mostrarEstado(nodo->derecha, prefijo + "D-");
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
            cout << "Proceso en ejecución: ID=" << procesoActual.getId() << endl;

            // Simular ejecución...
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

                    buddySystem.liberarBloque(bloqueAsignado);
                    memoriaDisponible += proceso.getTamano();
                    cout << "Memoria liberada del Proceso " << proceso.getId() << endl;
                } else {
                    cout << "No se pudo asignar memoria al Proceso " << proceso.getId() << std::endl;
                    procesosRechazados++;
                }
            } else {
                cout << "Memoria insuficiente para Proceso " << proceso.getId() << endl;
                procesosRechazados++;
            }

            // Control de finalización de la simulación (puedes ajustar esto según tus necesidades)
            if (procesosFinalizados >= procesosTotales - procesosRechazados) {
                detenerSimulacion();
            }
        }
    }

    // Método para terminar la simulación
    void detenerSimulacion() {
        ejecutarSimulacion = false;
    }
};

// Función principal
int main() {
    srand(time(NULL));
    Simulacion simulacion(TAMANO_MEMORIA, MIN_SPLIT, quantumMaximoSistema, quantumProceso, tamanoMaximoProceso);
    simulacion.ejecutar();
    return 0;
}
