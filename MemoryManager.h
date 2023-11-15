#ifndef PROYECTOFINALDEOS_MEMORYMANAGER_H
#define PROYECTOFINALDEOS_MEMORYMANAGER_H

#include "Process.h"

class MemoryManager {
    // Implementación del Buddy System
private:
    static const int MIN_SPLIT_SIZE = 32 * 1024; // 32 KB
    int memorySize; // Tamaño de la memoria en bytes
public:
    MemoryManager(int sizeInMB); // Constructor
    // Métodos relevantes aquí
    void allocate(int size); // Asigna memoria
    void deallocate(int size); // Libera memoria
    void displayMemory(); // Muestra el estado de la memoria
    void showMemoryState(); // Muestra el estado de la memoria
};

#endif
