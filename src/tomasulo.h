#ifndef TOMASULO_H
#define TOMASULO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_REGISTERS 32
#define NUM_RESERVATION_STATIONS 6
#define NUM_FUNCTIONAL_UNITS 3
#define MEMORY_SIZE 100
#define MAX_INSTRUCTIONS 100

typedef enum
{
    ADD,
    SUB,
    LW,
    SW,
    NOP
} Operation;

typedef struct
{
    Operation op;
    int dest;
    int src1;
    int src2;
    int imm;
    int busy;
    int exec_cycle;
} ReservationStation;

typedef struct
{
    int value;
    int tag;
} Register;

typedef struct
{
    int value;
} Memory;

extern ReservationStation reservation_stations[NUM_RESERVATION_STATIONS];
extern Register registers[NUM_REGISTERS];
extern Memory memory[MEMORY_SIZE];
extern int clock_cycle;

void initialize();
int issue_instruction(Operation op, int dest, int src1, int src2, int imm);
void execute_instructions();
void print_state();

#endif