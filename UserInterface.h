#ifndef PROYECTOFINALDEOS_USERINTERFACE_H
#define PROYECTOFINALDEOS_USERINTERFACE_H

#include "Scheduler.h"
#include "MemoryManager.h"
#include "Statistics.h"

class UserInterface {
private:
    Scheduler& scheduler;
    MemoryManager& memoryManager;
    Statistics& statistics;
    // Atributos para interfaz de usuario
public:
    void display(); // Muestra la interfaz de usuario
    // Métodos relevantes aquí
    void getUserInput(); // Obtiene la entrada del usuario para configuraciones y controles
    UserInterface(MemoryManager& memoryManager, Scheduler& scheduler, Statistics& statistics)
            : memoryManager(memoryManager), scheduler(scheduler), statistics(statistics) {}
};

#endif
