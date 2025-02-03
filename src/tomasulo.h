#ifndef TOMASULO_H
#define TOMASULO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de constantes. Número de registradores, estações de reserva, unidades funcionais, tamanho da memória e número máximo de instruções.
#define NUM_REGISTERS 32
#define NUM_RESERVATION_STATIONS 6
#define NUM_FUNCTIONAL_UNITS 3
#define MEMORY_SIZE 100
#define MAX_INSTRUCTIONS 100

// Define o tipo de operação.
typedef enum
{
    ADD,
    SUB,
    LW,
    SW,
    NOP
} Operation;

// Define a estrutura da estação de reserva.
typedef struct
{
    Operation op;
    int dest;
    int src1;
    int src2;
    int imm;
    int busy;
    int exec_cycle;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
} ReservationStation;

// Define a estrutura do registrador.
typedef struct
{
    int value;
    int tag;
} Register;

// Define a estrutura da memória.
typedef struct
{
    int value;
} Memory;

// Declarações de variáveis globais.
extern ReservationStation reservation_stations[NUM_RESERVATION_STATIONS];
extern Register registers[NUM_REGISTERS];
extern Memory memory[MEMORY_SIZE];
extern int clock_cycle;

// Define as latências das operações.
#define ADD_LATENCY 2
#define SUB_LATENCY 2
#define LW_LATENCY 3
#define SW_LATENCY 3

void initializeStationsRegistersAndMemory();
int defineIssueInstruction(Operation op, int dest, int src1, int src2, int imm);
void executeInstructions();
void printState();

#endif