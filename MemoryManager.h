#ifndef PROYECTOFINALDEOS_MEMORYMANAGER_H
#define PROYECTOFINALDEOS_MEMORYMANAGER_H

#include "Process.h"

class MemoryManager {
    // Implementación del Buddy System
public:
    MemoryManager(int size); // Constructor
    // Métodos relevantes aquí
    void allocate(int size); // Asigna memoria
    void deallocate(int size); // Libera memoria
    void displayMemory(); // Muestra el estado de la memoria
    void showMemoryState(); // Muestra el estado de la memoria
};

#endif
