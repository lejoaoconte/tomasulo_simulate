#include "files.h"

Operation parse_operation(char *op)
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

void read_instructions_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char op[10];
        int dest, src1, src2, imm;

        if (sscanf(line, "%s R%d, R%d, R%d", op, &dest, &src1, &src2) == 4)
        {
            Operation operation = parse_operation(op);
            issue_instruction(operation, dest, src1, src2, 0);
        }
        else if (sscanf(line, "%s R%d, %d(R%d)", op, &dest, &imm, &src1) == 4)
        {
            Operation operation = parse_operation(op);
            issue_instruction(operation, dest, src1, 0, imm);
        }
        else
        {
            printf("Unrecognized instruction: %s", line);
        }
    }

    fclose(file);
}