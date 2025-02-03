#include "tomasulo.h"

ReservationStation reservation_stations[NUM_RESERVATION_STATIONS]; // Define as estações de reserva.
Register registers[NUM_REGISTERS];                                 // Define os registradores.
Memory memory[MEMORY_SIZE];                                        // Define a memória.
int clock_cycle = 0;                                               // Inicializa o ciclo de clock.

void initializeStationsRegistersAndMemory()
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        // Inicializa todas as estações de reserva como livres.
        reservation_stations[i].busy = 0;
    }
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        // Inicializa os valores dos registradores como 0.
        registers[i].value = 0;
        // Inicializa as tags dos registradores como -1 (indicando que não estão esperando por resultados).
        registers[i].tag = -1;
    }
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        // Inicializa os valores da memória como 0.
        memory[i].value = 0;
    }
}

int defineIssueInstruction(Operation op, int dest, int src1, int src2, int imm)
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        // Verifica se a estação de reserva está livre.
        if (!reservation_stations[i].busy)
        {
            // Define a operação.
            reservation_stations[i].op = op;
            // Define o registrador de destino.
            reservation_stations[i].dest = dest;
            // Define o primeiro registrador de origem.
            reservation_stations[i].src1 = src1;
            // Define o segundo registrador de origem.
            reservation_stations[i].src2 = src2;
            // Define o valor imediato.
            reservation_stations[i].imm = imm;
            // Marca a estação de reserva como ocupada.
            reservation_stations[i].busy = 1;
            // Define o ciclo de execução.
            reservation_stations[i].exec_cycle = clock_cycle + 1;
            // Retorna o índice da estação de reserva.
            return i;
        }
    }
    // Retorna -1 se não houver estações de reserva livres.
    return -1;
}

void executeInstructions()
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        // Verifica se a estação de reserva está ocupada e pronta para executar.
        if (reservation_stations[i].busy && reservation_stations[i].exec_cycle <= clock_cycle)
        {
            // Obtém o valor do primeiro registrador de origem.
            int src1_value = (registers[reservation_stations[i].src1].tag == -1) ? registers[reservation_stations[i].src1].value : 0;
            // Obtém o valor do segundo registrador de origem.
            int src2_value = (registers[reservation_stations[i].src2].tag == -1) ? registers[reservation_stations[i].src2].value : 0;

            switch (reservation_stations[i].op)
            {
            case ADD:
                // Executa a operação de adição.
                registers[reservation_stations[i].dest].value = src1_value + src2_value;
                break;
            case SUB:
                // Executa a operação de subtração.
                registers[reservation_stations[i].dest].value = src1_value - src2_value;
                break;
            case LW:
                // Executa a operação de leitura da memória.
                registers[reservation_stations[i].dest].value = memory[reservation_stations[i].imm].value;
                break;
            case SW:
                // Executa a operação de escrita na memória.
                memory[reservation_stations[i].imm].value = src1_value;
                break;
            default:
                break;
            }
            // Libera a estação de reserva.
            reservation_stations[i].busy = 0;
            // Limpa a tag do registrador de destino.
            registers[reservation_stations[i].dest].tag = -1;
        }
    }
}

void printState()
{
    // Imprime o ciclo de clock atual.
    printf("Clock Cycle: %d\n", clock_cycle);
    printf("Reservation Stations:\n");
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        // Imprime o estado de cada estação de reserva.
        printf("RS%d: op=%d, dest=%d, src1=%d, src2=%d, imm=%d, busy=%d\n",
               i, reservation_stations[i].op, reservation_stations[i].dest,
               reservation_stations[i].src1, reservation_stations[i].src2,
               reservation_stations[i].imm, reservation_stations[i].busy);
    }
    printf("Registers:\n");
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        // Imprime o estado de cada registrador.
        printf("R%d: value=%d, tag=%d\n", i, registers[i].value, registers[i].tag);
    }
    printf("Memory:\n");
    for (int i = 0; i < 10; i++)
    {
        // Imprime os primeiros 10 valores da memória.
        printf("M%d: value=%d\n", i, memory[i].value);
    }
    printf("\n");
}