#ifndef FILES_H
#define FILES_H

#include "tomasulo.h"

Operation parse_operation(char *op);
void read_instructions_from_file(const char *filename);

#endif