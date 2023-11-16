#include "Config.h"

int Config::getProcessQuantum() {
    return processQuantum;
}

int Config::getMaxProcessMemorySize() {
    return maxProcessMemorySize;
}

int Config::getMaxSystemQuantum() {
    return maxSystemQuantum;
}

int Config::getMemorySize() {
    return memorySize;
}

int Config::getMinSplitSize() {
    return minSplitSize;
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