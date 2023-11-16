#ifndef PROYECTOFINALDEOS_CONFIG_H
#define PROYECTOFINALDEOS_CONFIG_H

class Config {
public:
    int processQuantum; // Cuánto de procesamiento asignado a cada proceso
    int maxProcessMemorySize; // Tamaño máximo de memoria de proceso
    int maxSystemQuantum; // Tamaño máximo del cuanto del sistema
    int memorySize; // Tamaño de la memoria (en MB)
    int minSplitSize; // Tamaño mínimo de split (en KB)

    Config() : processQuantum(0), maxProcessMemorySize(0), maxSystemQuantum(0), memorySize(1), minSplitSize(32) {}

    // Métodos para establecer y obtener configuraciones
    // ...
    int getProcessQuantum();
    int getMaxProcessMemorySize();
    int getMaxSystemQuantum();
    int getMemorySize();
    int getMinSplitSize();
    void setProcessQuantum(int quantum);
    void setMaxProcessMemorySize(int size);
    void setMaxSystemQuantum(int quantum);
    void setMemorySize(int size);
    void setMinSplitSize(int size);
};

#endif
