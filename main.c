#include "src/tomasulo.h"
#include "src/files.h"

// Código principal de execução, onde todo programa roda e é passado por essas linhas abaixo.
int main(int argc, char *argv[])
{

    // Inicializa as estações de reserva, registradores e memória.
    initializeStationsRegistersAndMemory();

    // Lê as instruções do arquivo "instructions.txt", caso não tenha um nome *.txt retorna a mensagem
    if (argc != 2)
    {
        fprintf(stderr, "Please enter name: %s <arquivo_de_instrucoes>.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Processa o arquivo de instruções passado como argumento.
    readInstructionsFromFile(argv[1]);

    while (1)
    {
        int all_done = 1;
        for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
        {
            // Verifica se há alguma estação de reserva ocupada.
            if (reservation_stations[i].busy)
            {
                all_done = 0;
                break;
            }
        }
        // Se todas as estações de reserva estiverem livres, o loop termina.
        if (all_done)
            break;

        // Executa as instruções nas estações de reserva.
        executeInstructions();
        // Imprime o estado atual do sistema.
        printState();
        // Incrementa o ciclo de clock.
        clock_cycle++;
    }

    // Indica que a simulação foi concluída.
    printf("Simulation completed.\n");
    return 0;
}