#include "Config.h"

int Config::getProcessQuantum() {
    return processQuantum; // Cuánto de procesamiento asignado a cada proceso
}

int Config::getMaxProcessMemorySize() {
    return maxProcessMemorySize; // Tamaño máximo de memoria de proceso
}

int Config::getMaxSystemQuantum() {
    return maxSystemQuantum; // Tamaño máximo del cuanto de procesamiento del sistema por proceso
}

int Config::getMemorySize() {
    return memorySize; // Tamaño de la memoria (en MB)
}

int Config::getMinSplitSize() {
    return minSplitSize; // Tamaño mínimo de split (en KB)
}

void Config::setProcessQuantum(int quantum) {
    processQuantum = quantum;
}

void Config::setMaxProcessMemorySize(int size) {
    maxProcessMemorySize = size;
}

void Config::setMaxSystemQuantum(int quantum) {
    maxSystemQuantum = quantum;
}

void Config::setMemorySize(int size) {
    memorySize = size;
}

void Config::setMinSplitSize(int size) {
    minSplitSize = size;
}