#ifndef PROYECTOFINALDEOS_PROCESS_H
#define PROYECTOFINALDEOS_PROCESS_H

class Process {
private:
    int id;                 // Identificador único del proceso
    int size;               // Tamaño del proceso en memoria
    int quantum;            // Cuánto asignado al proceso
    int remainingQuantum;   // Cuánto restante en la ejecución actual

public:
    // Constructor
    Process(int id, int size, int quantum) : id(id), size(size), quantum(quantum), remainingQuantum(quantum) {}
    Process() : id(0), size(0), quantum(0), remainingQuantum(0) {}

    // Getters y Setters
    int getId() const;
    int getSize() const;
    int getQuantum() const;
    int getRemainingQuantum() const;

    void setId(int id);
    void setSize(int size);
    void setQuantum(int quantum);
    void setRemainingQuantum(int remainingQuantum);

    // Método para simular la ejecución del proceso
    void execute();

    // Método para verificar si el proceso ha finalizado
    bool isFinished() const;
};


#endif
