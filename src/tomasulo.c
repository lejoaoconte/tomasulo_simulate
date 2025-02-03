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
        reservation_stations[i].Vj = 0;
        reservation_stations[i].Vk = 0;
        reservation_stations[i].Qj = -1;
        reservation_stations[i].Qk = -1;
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
        if (!reservation_stations[i].busy)
        {
            // Prepara a estação de reserva para a nova instrução.
            reservation_stations[i].op = op;
            reservation_stations[i].dest = dest;
            reservation_stations[i].imm = imm;
            reservation_stations[i].busy = 1;

            // RESET dos novos campos.
            reservation_stations[i].Vj = 0;
            reservation_stations[i].Vk = 0;
            reservation_stations[i].Qj = -1;
            reservation_stations[i].Qk = -1;

            // Define o ciclo de execução com base na operação.
            switch (op)
            {
            case ADD:
            case SUB:
                reservation_stations[i].exec_cycle = clock_cycle + ((op == ADD) ? ADD_LATENCY : SUB_LATENCY);
                break;
            case LW:
                reservation_stations[i].exec_cycle = clock_cycle + LW_LATENCY;
                break;
            case SW:
                reservation_stations[i].exec_cycle = clock_cycle + SW_LATENCY;
                break;
            default:
                reservation_stations[i].exec_cycle = clock_cycle + 1;
                break;
            }

            // TRATAMENTO DOS OPERANDOS:
            // Para ADD e SUB, usamos src1 e src2.
            if (op == ADD || op == SUB)
            {
                // Operando 1 (src1):
                if (registers[src1].tag == -1)
                {
                    reservation_stations[i].Vj = registers[src1].value;
                    reservation_stations[i].Qj = -1;
                }
                else
                {
                    reservation_stations[i].Qj = registers[src1].tag;
                }
                // Operando 2 (src2):
                if (registers[src2].tag == -1)
                {
                    reservation_stations[i].Vk = registers[src2].value;
                    reservation_stations[i].Qk = -1;
                }
                else
                {
                    reservation_stations[i].Qk = registers[src2].tag;
                }
                // Marca o registrador destino para aguardar o resultado.
                registers[dest].tag = i;
            }
            // Para LW, usamos src1 como base.
            else if (op == LW)
            {
                if (registers[src1].tag == -1)
                {
                    reservation_stations[i].Vj = registers[src1].value;
                    reservation_stations[i].Qj = -1;
                }
                else
                {
                    reservation_stations[i].Qj = registers[src1].tag;
                }
                // Atualiza o registrador destino para aguardar o resultado da carga.
                registers[dest].tag = i;
            }
            // Para SW, temos dois operandos:
            // - O primeiro (dest) é o registrador com o dado a ser armazenado.
            // - O segundo (src1) é o registrador base para o cálculo do endereço.
            else if (op == SW)
            {
                // Operando 1: valor a ser armazenado.
                if (registers[dest].tag == -1)
                {
                    reservation_stations[i].Vj = registers[dest].value;
                    reservation_stations[i].Qj = -1;
                }
                else
                {
                    reservation_stations[i].Qj = registers[dest].tag;
                }
                // Operando 2: base para o endereço.
                if (registers[src1].tag == -1)
                {
                    reservation_stations[i].Vk = registers[src1].value;
                    reservation_stations[i].Qk = -1;
                }
                else
                {
                    reservation_stations[i].Qk = registers[src1].tag;
                }
                // Não atualizamos nenhum registrador, pois SW não escreve em registrador.
            }

            return i; // Retorna o índice da estação utilizada.
        }
    }
    return -1; // Não há estação de reserva disponível.
}

void executeInstructions()
{
    for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
    {
        // Verifica se a estação está ocupada e se chegou o ciclo de execução.
        if (reservation_stations[i].busy && reservation_stations[i].exec_cycle == clock_cycle)
        {
            int result = 0;
            int effective_address = 0;
            switch (reservation_stations[i].op)
            {
            case ADD:
                result = reservation_stations[i].Vj + reservation_stations[i].Vk;
                break;
            case SUB:
                result = reservation_stations[i].Vj - reservation_stations[i].Vk;
                break;
            case LW:
                // Calcula o endereço efetivo: base (Vj) + imediato.
                effective_address = reservation_stations[i].Vj + reservation_stations[i].imm;
                if (effective_address < 0 || effective_address >= MEMORY_SIZE)
                {
                    printf("Erro de acesso à memória na RS%d para LW\n", i);
                    result = 0;
                }
                else
                {
                    result = memory[effective_address].value;
                }
                break;
            case SW:
                // Calcula o endereço efetivo: base (Vk) + imediato.
                effective_address = reservation_stations[i].Vk + reservation_stations[i].imm;
                if (effective_address < 0 || effective_address >= MEMORY_SIZE)
                {
                    printf("Erro de acesso à memória na RS%d para SW\n", i);
                }
                else
                {
                    // Armazena o valor (Vj) na memória.
                    memory[effective_address].value = reservation_stations[i].Vj;
                }
                break;
            default:
                break;
            }

            // WRITE-BACK: Apenas para operações que escrevem em registrador (ADD, SUB, LW).
            if (reservation_stations[i].op != SW)
            {
                // Atualiza o registrador destino, se ele ainda estiver aguardando o resultado desta RS.
                if (registers[reservation_stations[i].dest].tag == i)
                {
                    registers[reservation_stations[i].dest].value = result;
                    registers[reservation_stations[i].dest].tag = -1;
                }

                // Propaga o resultado para outras estações que aguardam esse valor.
                for (int j = 0; j < NUM_RESERVATION_STATIONS; j++)
                {
                    if (reservation_stations[j].busy)
                    {
                        if (reservation_stations[j].Qj == i)
                        {
                            reservation_stations[j].Vj = result;
                            reservation_stations[j].Qj = -1;
                        }
                        if (reservation_stations[j].Qk == i)
                        {
                            reservation_stations[j].Vk = result;
                            reservation_stations[j].Qk = -1;
                        }
                    }
                }
            }

            // Libera a estação de reserva.
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
        printf("RS%d: op=%d, dest=%d, imm=%d, busy=%d, exec_cycle=%d, Vj=%d, Qj=%d, Vk=%d, Qk=%d\n",
               i, reservation_stations[i].op, reservation_stations[i].dest,
               reservation_stations[i].imm, reservation_stations[i].busy, reservation_stations[i].exec_cycle,
               reservation_stations[i].Vj, reservation_stations[i].Qj,
               reservation_stations[i].Vk, reservation_stations[i].Qk);
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