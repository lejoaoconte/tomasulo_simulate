#include "tomasulo.h"

ReservationStation reservation_stations[NUM_RESERVATION_STATIONS];
Register registers[NUM_REGISTERS];
Memory memory[MEMORY_SIZE];
int clock_cycle = 0;

void initialize()
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        reservation_stations[i].busy = 0;
    }
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        registers[i].value = 0;
        registers[i].tag = -1;
    }
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i].value = 0;
    }
}

int issue_instruction(Operation op, int dest, int src1, int src2, int imm)
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        if (!reservation_stations[i].busy)
        {
            reservation_stations[i].op = op;
            reservation_stations[i].dest = dest;
            reservation_stations[i].src1 = src1;
            reservation_stations[i].src2 = src2;
            reservation_stations[i].imm = imm;
            reservation_stations[i].busy = 1;
            reservation_stations[i].exec_cycle = clock_cycle + 1;
            return i;
        }
    }
    return -1;
}

void execute_instructions()
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        if (reservation_stations[i].busy && reservation_stations[i].exec_cycle <= clock_cycle)
        {
            int src1_value = (registers[reservation_stations[i].src1].tag == -1) ? registers[reservation_stations[i].src1].value : 0;
            int src2_value = (registers[reservation_stations[i].src2].tag == -1) ? registers[reservation_stations[i].src2].value : 0;

            switch (reservation_stations[i].op)
            {
            case ADD:
                registers[reservation_stations[i].dest].value = src1_value + src2_value;
                break;
            case SUB:
                registers[reservation_stations[i].dest].value = src1_value - src2_value;
                break;
            case LW:
                registers[reservation_stations[i].dest].value = memory[reservation_stations[i].imm].value;
                break;
            case SW:
                memory[reservation_stations[i].imm].value = src1_value;
                break;
            default:
                break;
            }
            reservation_stations[i].busy = 0;
            registers[reservation_stations[i].dest].tag = -1;
        }
    }
}

void print_state()
{
    printf("Clock Cycle: %d\n", clock_cycle);
    printf("Reservation Stations:\n");
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        printf("RS%d: op=%d, dest=%d, src1=%d, src2=%d, imm=%d, busy=%d\n",
               i, reservation_stations[i].op, reservation_stations[i].dest,
               reservation_stations[i].src1, reservation_stations[i].src2,
               reservation_stations[i].imm, reservation_stations[i].busy);
    }
    printf("Registers:\n");
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        printf("R%d: value=%d, tag=%d\n", i, registers[i].value, registers[i].tag);
    }
    printf("Memory:\n");
    for (int i = 0; i < 10; i++)
    { 
        printf("M%d: value=%d\n", i, memory[i].value);
    }
    printf("\n");
}