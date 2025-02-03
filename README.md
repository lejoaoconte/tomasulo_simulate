## run

gcc -Wall -Wextra -std=c11 -I./src main.c src/tomasulo.c src/files.c -o main

add R1, R2, R3
sub R4, R1, R2
lw R5, 0(R1)
sw R5, 4(R1)