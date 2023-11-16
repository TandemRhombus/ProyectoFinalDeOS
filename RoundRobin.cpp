#include "RoundRobin.h"
#include <iostream>

using namespace std;

// Constructor
RoundRobin::RoundRobin(int quantum) : quantum(quantum) {}

// Añadir un proceso a la cola
void RoundRobin::addProcess(const Process& process) {
    processQueue.push(process);
}

// Ejecutar el algoritmo de Round Robin
void RoundRobin::execute() {
    while (!processQueue.empty()) {
        Process& currentProcess = processQueue.front();

        int processQuantum = min(currentProcess.getRemainingQuantum(), quantum);
        currentProcess.setRemainingQuantum(currentProcess.getRemainingQuantum() - processQuantum);

        cout << "Ejecutando proceso " << currentProcess.getId() << " por " << processQuantum << " unidades de tiempo." << endl;

        if (currentProcess.isFinished()) {
            cout << "Proceso " << currentProcess.getId() << " completado." << endl;
            processQueue.pop();
        } else {
            processQueue.push(currentProcess);
            processQueue.pop();
        }
    }
}

// Verificar si la cola está vacía
bool RoundRobin::isEmpty() const {
    return processQueue.empty();
}
