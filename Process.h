#ifndef PROYECTOFINALDEOS_PROCESO_H
#define PROYECTOFINALDEOS_PROCESO_H

class Process {
    int id;
    int size;
    int quantum;
public:
    Process(int id, int size, int quantum) : id(id), size(size), quantum(quantum) {}
    // Métodos relevantes aquí
    int getId(); // Retorna el id del proceso
    int getSize(); // Retorna el tamaño del proceso
    int getQuantum(); // Retorna el quantum
    void setQuantum(int quantum); // Cambia el quantum
    void setSize(int size); // Cambia el tamaño del proceso
    void setId(int id); // Cambia el id del proceso
    void showProcessInfo(); // Muestra la información del proceso
};

#endif
