#ifndef FILES_H
#define FILES_H

#include "tomasulo.h"

Operation parseOperation(char *op);
void readInstructionsFromFile(const char *filename);

#endif