#include "src/tomasulo.h"
#include "src/files.h"

int main()
{
    initialize();

    // Read instructions from file
    read_instructions_from_file("instructions.txt");

    // Simulate execution
    while (1)
    {
        int all_done = 1;
        for (int i = 0; i < NUM_RESERVATION_STATIONS; i++)
        {
            if (reservation_stations[i].busy)
            {
                all_done = 0;
                break;
            }
        }
        if (all_done)
            break;

        execute_instructions();
        print_state();
        clock_cycle++;
    }

    printf("Simulation completed.\n");
    return 0;
}