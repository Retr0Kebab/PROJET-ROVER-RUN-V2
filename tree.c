//
// Created by User on 11/14/2024.
//
#include <stdlib.h>
#include <stdio.h>
#include "moves.h"
#include "map.h"
#include "loc.h"
#include "tree.h"

// A global variable to provide unique IDs for each node in the tree.
int num  = 0;

/**
 * ========================
 * Tree Initialization and Construction
 * ========================
 */

// Creates child nodes for a given parent node.
t_node **createChildren(t_node *parent) {
    // Allocate memory for the array of child node pointers.
    t_node **children = (t_node **)malloc(parent->info.num_children * sizeof(t_node *));

    // Loop through the number of children the parent node is supposed to have.
    for (int i = 0; i < parent->info.num_children; ++i) {
        // Allocate memory for each child node.
        children[i] = (t_node *)malloc(sizeof(t_node));

        // Initialize child node's fields.
        children[i]->info.cost = 0; // Initial cost is 0.
        children[i]->info.is_end = 0; // Not an end node yet.
        children[i]->info.found_base = 0; // Base station not found yet.
        children[i]->info.num_children = parent->info.num_children - 1; // Decrease number of children as we move down.
        children[i]->parent = parent; // Set the parent of the child node.
        children[i]->children = NULL; // Initialize children as NULL since they're not created yet.
        children[i]->info.value = num++; // Assign a unique value (ID) to this node.
    }

    // Return the array of child node pointers.
    return children;
}

// Initializes the root node of the tree with a given number of choices.
t_node *initializeTree(int num_choices) {
    // Allocate memory for the root node.
    t_node *tree = (t_node *)malloc(sizeof(t_node));

    // Set initial values for the root node fields.
    tree->info.value = -1; // Root node has a placeholder value.
    tree->info.num_children = num_choices; // Number of children it will have.
    tree->info.movement = -1; // Placeholder movement value.
    tree->parent = NULL; // Root has no parent.
    tree->info.found_base = 0; // Base station not found initially.
    tree->info.is_end = 0; // Not an end node.
    tree->info.cost = 0; // Initial cost is 0.

    return tree; // Return the initialized root node.
}

// Recursively builds the tree from the root node until the minimum choices limit is reached.
void buildTree(t_node *root, int min_choices) {
    // Stop recursion if the number of children matches the minimum limit.
    if (root->info.num_children == min_choices - 1) return;

    // Create children for the current node.
    root->children = createChildren(root);

    // Recursively call buildTree on each child node.
    for (int i = 0; i < root->info.num_children; ++i) {
        assignRandomMove(root); // Assign random movements to the children.
        buildTree(root->children[i], min_choices); // Build the subtree for each child.
    }
}

/**
 * ========================
 * Tree Navigation and Simulation
 * ========================
 */

// Assigns random movements to a node's children.
void assignRandomMove(t_node *node) {
    // Get the number of children for the current node.
    int num_children = node->info.num_children;

    // Generate random movements for the node's children.
    t_move *random_moves = getRandomMoves(5*num_children);

    // Check if the random moves were successfully generated.
    if (!random_moves) {
        printf("Error: Failed to allocate random moves.\n");
        return; // Exit function in case of error.
    }

    // Assign each generated move to a child node.
    for (int i = 0; i < num_children; i++) {
        node->children[i]->info.movement = random_moves[i];
    }

    // Free the allocated memory for the random moves.
    free(random_moves);
}

// Validates and updates a node's move based on the map and remaining moves.
int validateAndUpdateMove(t_node *node, t_map map, int *remainingMoves) {
    // Set the node's location based on its parent's location.
    node->info.location.ori = node->parent->info.location.ori;
    node->info.location.pos.x = node->parent->info.location.pos.x;
    node->info.location.pos.y = node->parent->info.location.pos.y;

    // Extract the node's current position and movement.
    int x = node->info.location.pos.x;
    int y = node->info.location.pos.y;
    t_move movement = node->info.movement;

    // Update the node's cost by adding the parent's cost and the map cost at the current position.
    node->info.cost += node->parent->info.cost + map.costs[y][x];

    // Get the terrain type at the current position.
    t_soil soil = map.soils[y][x];

    // Handle special cases for different types of terrain.
    if (soil == BASE_STATION) {
        node->info.is_end = 1; // Mark node as an endpoint.
        node->info.found_base = 1; // Base station found.
        return -1; // Terminate further movement.
    } else if (soil == CREVASSE) {
        node->info.is_end = 1; // Mark node as an endpoint.
        return -1; // Terminate further movement.
    } else if (soil == REG) {
        (*remainingMoves)--; // Decrease remaining moves in regular terrain.
    }

    // For ERG terrain, ensure the movement is valid.
    if (soil == ERG) {
        if (!(movement == F_10 || movement == B_10 || movement == T_LEFT || movement == T_RIGHT)) {
            node->info.movement--; // Adjust the movement if invalid.
            return 1; // Indicate that movement adjustment occurred.
        }
        return 0; // Valid movement, continue.
    }
    return 1; // Default case, continue traversal.
}

// Traverses the tree, updating node states and paths.
void traverseTree(t_node *root, t_map map, int remainingMoves) {
    if (root->parent != NULL) {
        // Validate and update the current node's move.
        int checkResult = validateAndUpdateMove(root, map, &remainingMoves);

        // Adjust location if the move is valid.
        if (checkResult == 1) {
            updateLocalisation(&root->info.location, root->info.movement);
        } else if (checkResult == -1) {
            return; // Terminate traversal if the node is an endpoint.
        }

        remainingMoves--; // Decrease the remaining moves.

        // Stop traversal if out of moves or no children exist.
        if (remainingMoves == 0 || root->children == NULL) {
            root->info.is_end = 1; // Mark as an endpoint.
            return;
        }
    }

    // Validate the node's position against map boundaries.
    if (!isValidLocalisation(root->info.location.pos, map.x_max, map.y_max)) {
        root->info.is_end = 1; // Mark as an endpoint.
        return;
    }

    // Recursively traverse all child nodes.
    for (int i = 0; i < root->info.num_children; i++) {
        traverseTree(root->children[i], map, remainingMoves);
    }
}

/**
 * ========================
 * Pathfinding and Optimization
 * ========================
 */

// Finds and prints all paths to the base station from the root.
void findPath(t_node *root) {
    if (root->info.found_base) {
        // Base station found, print path details.
        printf("\n  Path : \n ");
        printf("    Arrival: [%d ; %d] ORI: %s\n", root->info.location.pos.y, root->info.location.pos.x, orient[root->info.location.ori]);
        printPath(root->parent); // Print the path leading to this node.
        printf("     Cost: %d\n", root->info.cost); // Print the cost.
    }

    // Stop searching if the node is a leaf or an endpoint.
    if (root->children == NULL || root->info.is_end) return;

    // Recursively search in child nodes.
    for (int i = 0; i < root->info.num_children; i++) {
        findPath(root->children[i]);
    }
}

// Finds the best path to the base station by minimizing cost.
void findBestPath(t_node *root, t_node **best) {
    // If the root node is null, terminate the function.
    if (root == NULL) return;

    // Check if the current node has found the base station.
    if (root->info.found_base) {
        // If no "best" node is currently set, or if the current node's cost is lower, update the best node.
        if (*best == NULL || root->info.cost < (*best)->info.cost) {
            *best = root; // Update the best node pointer to the current node.
        }
    }

    // Stop searching if the node has no children or is marked as an endpoint.
    if (root->children == NULL || root->info.is_end) return;

    // Recursively search for the best path in the child nodes.
    for (int i = 0; i < root->info.num_children; i++) {
        findBestPath(root->children[i], best);
    }
}

// Recursively prints the path from a given node back to the root.
void printPath(t_node *node) {
    // If the node is null, terminate recursion.
    if (node == NULL) return;

    // If the node has a parent, recursively print its parent's path first.
    if (node->parent != NULL) {
        printPath(node->parent);
    }

    // If this is the starting node (root), print the column headers.
    if (node->parent == NULL) {
        printf(" %10s | %10s | %10s | %10s\n", "Y", "X", "ORI", "MOVE");
        printf("---------------------------------------------\n");
        printf(" %10d | %10d | %10s | %10s\n",
               node->info.location.pos.y,
               node->info.location.pos.x,
               orient[node->info.location.ori], // Orientation as a string.
               "START"); // Movement is "START" for the root node.
    } else {
        // Print the current node's position, orientation, and movement.
        printf(" %10d | %10d | %10s | %10s\n",
               node->info.location.pos.y,
               node->info.location.pos.x,
               orient[node->info.location.ori], // Orientation as a string.
               _moves[node->info.movement]); // The type of move made to reach this node.
    }
}

/**
 * ========================
 * Main Simulation Execution
 * ========================
 */

// Executes the main simulation, including tree construction, traversal, and pathfinding.
t_node *executePhase(int num_choices, int min_choices, t_map map, int mode) {
    // Step 1: Initialize the root node of the tree.
    t_node *root = initializeTree(num_choices);

    // Step 2: Retrieve the base station position from the map.
    t_position baseStation = getBaseStationPosition(map);

    // Step 3: Build the decision tree starting from the root node.
    buildTree(root, min_choices);

    // Step 4: Set the initial location of the root node.
    root->info.location = loc_init(4, 1, EAST); // Example: start at position (4, 1) facing EAST.

    // Step 5: Traverse the tree to simulate all paths.
    traverseTree(root, map, 5); // Assume 5 remaining moves to begin with.

    // Step 6: Based on the selected mode, find and print paths.
    t_node *bestNode = NULL; // Pointer to track the best path node.
    if (mode == 1) {
        findPath(root); // Print all paths to the base station.
    } else if (mode == 2) {
        findBestPath(root, &bestNode); // Find and print the best path to the base station.

        // If a best path is found, print its details.
        if (bestNode != NULL) {
            printf("\n  Best Path : \n ");
            printf("    Arrival: [%d ; %d] ORI: %s\n",
                   bestNode->info.location.pos.y,
                   bestNode->info.location.pos.x,
                   orient[bestNode->info.location.ori]); // Orientation as a string.

            printPath(bestNode->parent); // Print the path leading to the best node.
            printf("     Cost: %d\n", bestNode->info.cost); // Print the total cost of the best path.
        }
    } else {
        // Handle invalid mode input.
        printf("Invalid mode selected.\n");
    }

    // Step 7: Return the root node of the tree.
    return root;
}
