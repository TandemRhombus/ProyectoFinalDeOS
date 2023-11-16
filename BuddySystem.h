#ifndef PROYECTOFINALDEOS_BUDDYSYSTEM_H
#define PROYECTOFINALDEOS_BUDDYSYSTEM_H

#include <vector>
#include <iostream>
#include "Process.h"
#include <map>
#include <utility> // Para std::pair

using namespace std;

class BuddySystem {
private:
    struct Block {
        int size;
        bool free;
        int startAddress;
        Block(int size, int startAddress) : size(size), free(true), startAddress(startAddress) {}
    };

    int totalMemorySize;
    int minSplitSize; // Tamaño mínimo del bloque después de dividirlo
    //obtener el maxSize del proceso
    int maxSize;
    map<int, vector<Block>> memoryBlocks; // Clasificados por tamaño

public:
    // Constructor
    BuddySystem(int memorySize, int splitSize);

    // Métodos para asignar y liberar memoria
    int allocate(int size);
    int allocateWithMaxSize(int size, int maxSize);
    void deallocate(int startAddress);

    // Método para mostrar el estado de la memoria
    void displayMemoryState() const;

    // Getters y Setters
    int getTotalMemorySize() const;
    void setTotalMemorySize(int size);
    int getMinSplitSize() const;
    void setMinSplitSize(int size);
    int getMaxSize() const;
    void setMaxSize(int size);

private:
    // Métodos auxiliares para la gestión de bloques
    void splitBlock(int size);
    void mergeBlocks();
    int findSuitableBlockSize(int size) const;
};

#endif