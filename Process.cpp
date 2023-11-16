#include "Process.h"
#include <iostream>

using namespace std;

// Getters
int Process::getId() const {
    return id;
}

int Process::getSize() const {
    return size;
}

int Process::getQuantum() const {
    return quantum;
}

int Process::getRemainingQuantum() const {
    return remainingQuantum;
}

// Setter
void Process::setRemainingQuantum(int remainingQuantum) {
    this->remainingQuantum = remainingQuantum;
}

// Setter
void Process::setId(int id) {
    this->id = id;
}

// Setter
void Process::setSize(int size) {
    this->size = size;
}

// Setter
void Process::setQuantum(int quantum) {
    this->quantum = quantum;
}

// Simula la ejecución del proceso
void Process::execute() {
    if (remainingQuantum > 0) {
        remainingQuantum--;
        cout << "Proceso " << id << " en ejecución. Quantum restante: " << remainingQuantum << endl;
    }
}

// Verifica si el proceso ha finalizado
bool Process::isFinished() const {
    return remainingQuantum <= 0;
}