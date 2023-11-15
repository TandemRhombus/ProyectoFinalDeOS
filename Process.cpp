#include "Process.h"
#include <iostream>

using namespace std;

// Metodos de la clase Process
int Process::getId() {
    return id;
}

int Process::getSize() {
    return size;
}

int Process::getQuantum() {
    return quantum;
}

void Process::setQuantum(int quantum) {
    this->quantum = quantum;
}

void Process::setSize(int size) {
    this->size = size;
}

void Process::setId(int id) {
    this->id = id;
}

void Process::showProcessInfo() {
    cout << "ID: " << id << endl;
    cout << "Size: " << size << endl;
    cout << "Quantum: " << quantum << endl;
}

