#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "tree.h"


// Main function: Entry point of the program.
int main() {
    // Step 1: Create a map object from a file.
    // The file "../maps/training.map" is expected to contain the map's details.
    t_map map = createMapFromFile("../maps/example1.map");

    // Step 2: Check if the map was initialized correctly.
    // Both soils and costs should be non-NULL for a valid map.
    if (map.soils == NULL || map.costs == NULL) {
        fprintf(stderr, "Error: Failed to initialize the map.\n"); // Print error to standard error.
        return EXIT_FAILURE; // Exit the program with failure status.
    }

    // Step 3: Print basic information about the map dimensions.
    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);

    // Step 4: Display the raw soil types map.
    // This shows the integer values representing terrain types for each cell.
    printf("\nSoil Types Map:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]); // Print each soil type.
        }
        printf("\n"); // Move to the next row after printing a row of the map.
    }

    // Step 5: Display the raw cost map.
    // This shows the cost of moving through each cell in the map.
    printf("\nInitial Cost Map:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%-5d ", map.costs[i][j]); // Print each cost, aligned for readability.
        }
        printf("\n"); // Move to the next row after printing a row of costs.
    }

    // Step 6: Display a visual representation of the map.
    // Uses the `displayMap` function, which might render a user-friendly view.
    printf("\nVisual Map Representation:\n");
    displayMap(map);

    // Step 7: Initialize the simulation mode variable.
    int mode = 0;

    // Step 8: Display a menu for the user to select the mode of operation.
    printf("===================================\n");
    printf("          PATHFINDER MENU          \n");
    printf("===================================\n");
    printf("1. Print every path\n");  // Mode 1: Display all possible paths.
    printf("2. Print the best path\n"); // Mode 2: Display only the optimal path.
    printf("-----------------------------------\n");
    printf("Please select a mode (1 or 2): ");

    // Step 9: Validate user input.
    // Loop until the user enters a valid mode (1 or 2).
    while (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) {
        printf("Invalid selection. Please enter 1 or 2: ");
        // Clear any invalid input from the input buffer.
        while (getchar() != '\n');
    }

    // Step 10: Confirm the selected mode to the user.
    printf("\nYou selected Mode %d.\n", mode);
    printf("===================================\n");

    // Step 11: Execute the simulation with the chosen mode.
    // Parameters:
    // - 9: Initial number of choices at the root node.
    // - 5: Minimum number of choices for stopping tree growth.
    // - map: The initialized map object.
    // - mode: The user's selected mode of operation (1 or 2).
    executePhase(9, 5, map, mode);

    // Step 12: End the program with a successful status.
    return 0;
}

