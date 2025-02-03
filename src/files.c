#include "files.h"

// função que converte a string da operação para o tipo Operation. A mesma verifica se a operação é "add", "sub", "lw" ou "sw". Retorna NOP (No Operation) se a operação não for reconhecida.
Operation parseOperation(char *op)
{
    if (strcmp(op, "add") == 0)
        return ADD;
    if (strcmp(op, "sub") == 0)
        return SUB;
    if (strcmp(op, "lw") == 0)
        return LW;
    if (strcmp(op, "sw") == 0)
        return SW;
    return NOP;
}

void readInstructionsFromFile(const char *filename)
{
    // Abre o arquivo para leitura.
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto.
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    // Lê cada linha do arquivo.
    while (fgets(line, sizeof(line), file))
    {
        char op[10];
        int dest, src1, src2, imm;

        // Verifica se a linha contém uma operação com três registradores.
        if (sscanf(line, "%s R%d, R%d, R%d", op, &dest, &src1, &src2) == 4)
        {
            // Converte a string da operação para o tipo Operation.
            Operation operation = parseOperation(op);
            // Emite a instrução para a estação de reserva.
            defineIssueInstruction(operation, dest, src1, src2, 0);
        }
        // Verifica se a linha contém uma operação com um registrador e um valor imediato.
        else if (sscanf(line, "%s R%d, %d(R%d)", op, &dest, &imm, &src1) == 4)
        {
            // Converte a string da operação para o tipo Operation.
            Operation operation = parseOperation(op);
            // Emite a instrução para a estação de reserva.
            defineIssueInstruction(operation, dest, src1, 0, imm);
        }
        else
        {
            // Exibe uma mensagem de erro se a instrução não for reconhecida.
            printf("Unrecognized instruction: %s", line);
        }
    }

    fclose(file);
}