#include "RoundRobin.h"
#include <iostream>

using namespace std;

// Constructor
RoundRobin::RoundRobin(int quantum) : quantum(quantum) {} // Constructor s

// Añadir un proceso a la cola
void RoundRobin::addProcess(const Process& process) {
    processQueue.push(process); // Añadir el proceso a la cola
}

// Ejecutar el algoritmo de Round Robin
void RoundRobin::execute() {
    while (!processQueue.empty()) { // Mientras la cola no esté vacía ...
        Process& currentProcess = processQueue.front(); // Obtener el proceso actual

        int processQuantum = min(currentProcess.getRemainingQuantum(), quantum); // Obtener el quantum del proceso
        currentProcess.setRemainingQuantum(currentProcess.getRemainingQuantum() - processQuantum); // Actualizar el quantum restante

        cout << "Ejecutando proceso " << currentProcess.getId() << " por " << processQuantum << " unidades de tiempo." << endl; // Mostrar mensaje de ejecución (esto es de prueba)

        if (currentProcess.isFinished()) { // Si el proceso ha finalizado, sacarlo de la cola
            cout << "Proceso " << currentProcess.getId() << " completado." << endl; // Mostrar mensaje de finalización (esto es de prueba)
            processQueue.pop(); // Sacar el proceso de la cola
        } else {    // Si el proceso no ha finalizado, ponerlo al final de la cola
            processQueue.push(currentProcess); // Poner el proceso al final de la cola
            processQueue.pop(); // Sacar el proceso de la cola
        }
    }
}

// Verificar si la cola está vacía
bool RoundRobin::isEmpty() const { // Verificar si la cola está vacía
    return processQueue.empty();
}
