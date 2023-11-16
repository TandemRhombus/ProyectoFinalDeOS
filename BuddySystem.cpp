#include "BuddySystem.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

// Constructor
BuddySystem::BuddySystem(int memorySize, int splitSize)
        : totalMemorySize(memorySize), minSplitSize(splitSize) { // Inicializar el tamaño total de la memoria (en bytes) y el tamaño mínimo del bloque después de dividirlo (en bytes)
    //convercion de tamaño de memoria y split a bytes
    this->totalMemorySize = memorySize * 1024 * 1024;
    int largestBlockSize = pow(2, ceil(log2(memorySize))); // Tamaño del bloque más grande (en bytes)
    memoryBlocks[largestBlockSize].emplace_back(largestBlockSize, 0); // Inicializar el bloque más grande (en bytes)
}

// Asignar memoria
int BuddySystem::allocate(int size) { // Este método se llama cuando se solicita memoria para un proceso (sin tamaño máximo)
    int blockSize = findSuitableBlockSize(size); // Buscar un bloque disponible del tamaño solicitado o más grande
    if (blockSize == 0) {
        cout << "No hay suficiente memoria disponible." << endl;
        return -1;
    }

    if (memoryBlocks[blockSize].empty()) { // Si no hay bloques disponibles del tamaño solicitado, dividir un bloque más grande
        splitBlock(blockSize);
    }

    for (auto& block : memoryBlocks[blockSize]) { // Buscar un bloque disponible del tamaño solicitado
        if (block.free) { // Si se encuentra un bloque disponible, marcarlo como ocupado y devolver su dirección de inicio
            block.free = false; // Marcar el bloque como ocupado
            return block.startAddress; // Devolver la dirección de inicio del bloque
        }
    }

    return -1; // No debería llegar aquí
}

// Asignar memoria con un tamaño máximo
int BuddySystem::allocateWithMaxSize(int size, int maxSize) {
    // Verificar si el tamaño solicitado excede el tamaño máximo permitido
    if (size > maxSize) {
        cout << "Solicitud de memoria excede el tamano maximo permitido para el proceso." << endl; // Denegar la solicitud (el tamaño solicitado excede el tamaño máximo permitido)
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
void BuddySystem::displayMemoryState() const { // Este método se llama cuando se muestra el estado de la memoria (para fines de prueba)
    cout << "Estado de la memoria:" << endl;
    for (const auto& pair : memoryBlocks) {
        for (const auto& block : pair.second) {
            cout << "Bloque de " << block.size << " bytes en direccion " << block.startAddress
                      << ". Estado: " << (block.free ? "Libre" : "Ocupado") << endl; // Mostrar el tamaño del bloque, la dirección de inicio y el estado (libre u ocupado)
        }
    }
}

// Métodos auxiliares
void BuddySystem::splitBlock(int size) { // Este método se llama cuando no hay bloques disponibles del tamaño solicitado
    int splitSize;
    this->minSplitSize = splitSize * 1024; //convercion de split a bytes
    // Lógica modificada para considerar el SplitSize
    int currentSize = size * 2;
    while (currentSize <= totalMemorySize && memoryBlocks[currentSize].empty()) {
        currentSize *= 2;
    }

    if (currentSize > totalMemorySize) {
        cout << "No se pudo encontrar un bloque para dividir." << endl; // No se pudo encontrar un bloque para dividir (no debería llegar aquí)
        return;
    }

    while (currentSize != size && currentSize / 2 >= splitSize) { // Lógica modificada para considerar el SplitSize (currentSize / 2 >= splitSize)
        currentSize /= 2;
        int startAddress = memoryBlocks[currentSize * 2].front().startAddress;
        memoryBlocks[currentSize * 2].erase(memoryBlocks[currentSize * 2].begin());
        memoryBlocks[currentSize].emplace_back(currentSize, startAddress);
        memoryBlocks[currentSize].emplace_back(currentSize, startAddress + currentSize);
    }
}

void BuddySystem::mergeBlocks() { // Este método se llama cuando se libera un bloque de memoria
    bool merged;
    do {
        merged = false;
        for (auto& pair : memoryBlocks) {
            if (pair.second.size() >= 2) {
                sort(pair.second.begin(), pair.second.end(),
                          [](const Block& a, const Block& b) { return a.startAddress < b.startAddress; }); // Ordenar por dirección de inicio ascendente (para facilitar la fusión)

                for (size_t i = 0; i < pair.second.size() - 1; ++i) { // Combinar bloques adyacentes del mismo tamaño si están libres y tienen direcciones de inicio consecutivas (para facilitar la fusión)
                    Block& block1 = pair.second[i]; // Referencia al bloque actual
                    Block& block2 = pair.second[i + 1]; // Referencia al siguiente bloque

                    if (block1.free && block2.free && block1.startAddress + block1.size == block2.startAddress) { // Combinar bloques adyacentes del mismo tamaño si están libres y tienen direcciones de inicio consecutivas (para facilitar la fusión)
                        block1.size *= 2;
                        pair.second.erase(pair.second.begin() + i + 1); // Eliminar el siguiente bloque
                        memoryBlocks[block1.size].push_back(block1); // Agregar el bloque combinado al mapa de bloques
                        pair.second.erase(pair.second.begin() + i); // Eliminar el bloque actual (el siguiente bloque ahora ocupa su lugar)
                        merged = true;
                        break;
                    }
                }
            }
            if (merged) break; // Si se fusionaron bloques, volver a intentar fusionar desde el principio (para evitar fusiones innecesarias)
        }
    } while (merged); // Repetir mientras se fusionen bloques (para evitar fusiones innecesarias)
}

int BuddySystem::findSuitableBlockSize(int size) const { // Este método se llama cuando se solicita memoria para un proceso
    int blockSize = pow(2, ceil(log2(size))); // Tamaño del bloque más pequeño que puede contener el proceso (en bytes)
    while (blockSize <= totalMemorySize) { // Buscar un bloque disponible del tamaño solicitado o más grande
        if (!memoryBlocks.at(blockSize).empty() && memoryBlocks.at(blockSize).front().free) { // Si se encuentra un bloque disponible, devolver su tamaño
            return blockSize;
        }
        blockSize *= 2;
    }
    return 0;
}

// Getters y Setters
int BuddySystem::getTotalMemorySize() const { // Obtener el tamaño total de la memoria (en bytes)
    return totalMemorySize;
}

void BuddySystem::setTotalMemorySize(int size) { // Actualizar el tamaño total de la memoria (en bytes)
    totalMemorySize = size;
}