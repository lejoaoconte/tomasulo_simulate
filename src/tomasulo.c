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
        // Verifica se a estação de reserva está livre
        if (!reservation_stations[i].busy)
        {
            reservation_stations[i].op = op;
            reservation_stations[i].dest = dest;
            reservation_stations[i].imm = imm;
            reservation_stations[i].busy = 1;

            // Dependência de registrador de origem (aguardar se necessário)
            if (registers[src1].tag == -1)
            {
                reservation_stations[i].src1 = src1;
            }
            else
            {
                reservation_stations[i].src1 = -1; // Ainda aguardando valor
            }

            if (registers[src2].tag == -1)
            {
                reservation_stations[i].src2 = src2;
            }
            else
            {
                reservation_stations[i].src2 = -1;
            }

            // Define o tempo de execução com base na operação
            switch (op)
            {
            case ADD:
            case SUB:
                reservation_stations[i].exec_cycle = clock_cycle + ADD_LATENCY;
                break;
            case LW:
            case SW:
                reservation_stations[i].exec_cycle = clock_cycle + LW_LATENCY;
                break;
            default:
                reservation_stations[i].exec_cycle = clock_cycle + 1;
                break;
            }

            // Marcar que o registrador de destino está esperando um resultado
            registers[dest].tag = i;

            return i; // Retorna o índice da estação de reserva usada
        }
    }
    return -1; // Não há estação de reserva disponível
}

void executeInstructions()
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        // Verifica se a estação de reserva está ocupada e se é o ciclo correto para executar
        if (reservation_stations[i].busy && reservation_stations[i].exec_cycle == clock_cycle)
        {
            int result = 0;
            int src1_value = (reservation_stations[i].src1 >= 0) ? registers[reservation_stations[i].src1].value : 0;
            int src2_value = (reservation_stations[i].src2 >= 0) ? registers[reservation_stations[i].src2].value : 0;

            // Executar a operação
            switch (reservation_stations[i].op)
            {
            case ADD:
                result = src1_value + src2_value;
                break;
            case SUB:
                result = src1_value - src2_value;
                break;
            case LW:
                result = memory[reservation_stations[i].imm].value;
                break;
            case SW:
                memory[reservation_stations[i].imm].value = src1_value;
                break;
            default:
                break;
            }

            // Write-Back: Se for uma operação SW, não precisa atualizar registrador
            if (reservation_stations[i].op != SW)
            {
                registers[reservation_stations[i].dest].value = result;
                registers[reservation_stations[i].dest].tag = -1; // Registrador agora está pronto

                // Propaga o resultado para outras estações que estavam esperando esse valor
                for (int j = 0; j < NUM_RESERVATION_STATIONS; j++)
                {
                    if (reservation_stations[j].busy)
                    {
                        if (reservation_stations[j].src1 == i)
                        {
                            reservation_stations[j].src1 = -1;                      // Indica que o valor está pronto
                            registers[reservation_stations[j].dest].value = result; // Propaga o valor
                        }
                        if (reservation_stations[j].src2 == i)
                        {
                            reservation_stations[j].src2 = -1;
                            registers[reservation_stations[j].dest].value = result;
                        }
                    }
                }
            }

            // Libera a estação de reserva após execução
            reservation_stations[i].busy = 0;
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