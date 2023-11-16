#ifndef PROYECTOFINALDEOS_ROUNDROBIN_H
#define PROYECTOFINALDEOS_ROUNDROBIN_H

#include "Process.h"
#include <queue>

using namespace std;

class RoundRobin {
private:
    queue<Process> processQueue;
    int quantum; // Cuánto de tiempo para cada proceso

public:
    // Constructor
    RoundRobin(int quantum);
    RoundRobin() : quantum(0) {}

    // Añadir un proceso a la cola
    void addProcess(const Process& process);

    // Ejecutar el algoritmo de Round Robin
    void execute();

    // Verificar si la cola está vacía
    bool isEmpty() const;

    // Getters y Setters
    int getQuantum() const;
    void setQuantum(int quantum);
};


#endif
