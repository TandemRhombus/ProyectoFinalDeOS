#ifndef PROYECTOFINALDEOS_SCHEDULER_H
#define PROYECTOFINALDEOS_SCHEDULER_H

#include <queue>
#include "Process.h"

using namespace std;

class Scheduler{
    // Implementación de Round Robin
    queue<Process> readyQueue;
    int quantum;
public:
    Scheduler(int quantum) : quantum(quantum) {}
    // Métodos relevantes aquí
    void addProcess(Process& process); // Agrega un proceso a la cola de listos
    void schedule(); // Planifica el proceso que está en la cabeza de la cola de listos
    void removeProcess(); // Elimina el proceso que está en la cabeza de la cola de listos
    bool isEmpty(); // Retorna true si la cola de listos está vacía
    Process getNextProcess(); // Retorna el proceso que está en la cabeza de la cola de listos
    int getQuantum(); // Retorna el quantum
    void setQuantum(int quantum); // Cambia el quantum
};

#endif
