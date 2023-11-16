#include "BuddySystem.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

// Constructor
BuddySystem::BuddySystem(int memorySize, int splitSize)
        : totalMemorySize(memorySize), minSplitSize(splitSize) {
    //convercion de tamaño de memoria y split a bytes
    this->totalMemorySize = memorySize * 1024 * 1024;
    this->minSplitSize = splitSize * 1024;
    int largestBlockSize = pow(2, ceil(log2(memorySize)));
    memoryBlocks[largestBlockSize].emplace_back(largestBlockSize, 0);
}

// Asignar memoria
int BuddySystem::allocate(int size) {
    int blockSize = findSuitableBlockSize(size);
    if (blockSize == 0) {
        cout << "No hay suficiente memoria disponible." << endl;
        return -1;
    }

    if (memoryBlocks[blockSize].empty()) {
        splitBlock(blockSize);
    }

    for (auto& block : memoryBlocks[blockSize]) {
        if (block.free) {
            block.free = false;
            return block.startAddress;
        }
    }

    return -1; // No debería llegar aquí
}

// Asignar memoria con un tamaño máximo
int BuddySystem::allocateWithMaxSize(int size, int maxSize) {
    // Verificar si el tamaño solicitado excede el tamaño máximo permitido
    if (size > maxSize) {
        cout << "Solicitud de memoria excede el tamaño máximo permitido para el proceso." << endl;
        return -1; // Denegar la solicitud
    }

    return allocate(size); // Llamar al método allocate existente
}

// Liberar memoria
void BuddySystem::deallocate(int startAddress) {
    for (auto& pair : memoryBlocks) {
        for (auto& block : pair.second) {
            if (block.startAddress == startAddress && !block.free) {
                block.free = true;
                mergeBlocks();
                return;
            }
        }
    }
    cout << "Dirección de inicio no válida." << endl;
}

// Mostrar el estado de la memoria
void BuddySystem::displayMemoryState() const {
    cout << "Estado de la memoria:" << endl;
    for (const auto& pair : memoryBlocks) {
        for (const auto& block : pair.second) {
            cout << "Bloque de " << block.size << " bytes en dirección " << block.startAddress
                      << ". Estado: " << (block.free ? "Libre" : "Ocupado") << endl;
        }
    }
}

// Métodos auxiliares
void BuddySystem::splitBlock(int size) {
    // Lógica modificada para considerar el SplitSize
    int currentSize = size * 2;
    while (currentSize <= totalMemorySize && memoryBlocks[currentSize].empty()) {
        currentSize *= 2;
    }

    if (currentSize > totalMemorySize) {
        cout << "No se pudo encontrar un bloque para dividir." << endl;
        return;
    }

    while (currentSize != size && currentSize / 2 >= minSplitSize) {
        currentSize /= 2;
        int startAddress = memoryBlocks[currentSize * 2].front().startAddress;
        memoryBlocks[currentSize * 2].erase(memoryBlocks[currentSize * 2].begin());
        memoryBlocks[currentSize].emplace_back(currentSize, startAddress);
        memoryBlocks[currentSize].emplace_back(currentSize, startAddress + currentSize);
    }
}

void BuddySystem::mergeBlocks() {
    bool merged;
    do {
        merged = false;
        for (auto& pair : memoryBlocks) {
            if (pair.second.size() >= 2) {
                sort(pair.second.begin(), pair.second.end(),
                          [](const Block& a, const Block& b) { return a.startAddress < b.startAddress; });

                for (size_t i = 0; i < pair.second.size() - 1; ++i) {
                    Block& block1 = pair.second[i];
                    Block& block2 = pair.second[i + 1];

                    if (block1.free && block2.free && block1.startAddress + block1.size == block2.startAddress) {
                        block1.size *= 2;
                        pair.second.erase(pair.second.begin() + i + 1);
                        memoryBlocks[block1.size].push_back(block1);
                        pair.second.erase(pair.second.begin() + i);
                        merged = true;
                        break;
                    }
                }
            }
            if (merged) break;
        }
    } while (merged);
}

int BuddySystem::findSuitableBlockSize(int size) const {
    int blockSize = pow(2, ceil(log2(size)));
    while (blockSize <= totalMemorySize) {
        if (!memoryBlocks.at(blockSize).empty() && memoryBlocks.at(blockSize).front().free) {
            return blockSize;
        }
        blockSize *= 2;
    }
    return 0;
}

// Getters y Setters
int BuddySystem::getTotalMemorySize() const {
    return totalMemorySize;
}

void BuddySystem::setTotalMemorySize(int size) {
    totalMemorySize = size;
}