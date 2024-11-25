//
// Created by User on 11/14/2024.
//


#ifndef MARC_TREE_H
#define MARC_TREE_H

#include "loc.h"
#include "map.h"
#include "moves.h"
#include "stdbool.h"

/**
 * @brief Structure to hold the information of a node in the tree.
 */
typedef struct s_info {
    t_move movement;            // The movement associated with the node
    int is_end;                 // Flag to indicate if this node represents an end point
    int value;                  // A unique value for the node
    int found_base;             // Flag to indicate if the base station was found
    int cost;                   // The total cost to reach this node
    t_localisation location;    // The current location (position and orientation)
    int num_children;           // The number of child nodes
} t_info;

/**
 * @brief Structure representing a node in the tree.
 */
typedef struct s_node {
    struct s_node **children;   // Pointer to an array of child nodes
    struct s_node *parent;      // Pointer to the parent node
    t_info info;                // Information stored in the node
} t_node;
/**
 * ========================
 * Tree Initialization and Construction
 * ========================
 */

/**
 * @brief Creates child nodes for a given parent node.
 * @param parent Pointer to the parent node.
 * @return Array of pointers to the created child nodes.
 */
t_node **createChildren(t_node *parent);

/**
 * @brief Initializes a tree with a specified number of children per node.
 * @param num_choices Number of children for the root node.
 * @return Pointer to the root node.
 */
t_node *initializeTree(int num_choices);

/**
 * @brief Builds the tree recursively by creating children for each node.
 * @param root Pointer to the root node.
 * @param min_choices Minimum number of choices for children.
 */
void buildTree(t_node *root, int min_choices);

/**
 * ========================
 * Tree Navigation and Simulation
 * ========================
 */

/**
 * @brief Assigns a random movement to each child of a node.
 * @param node Pointer to the parent node.
 */
void assignRandomMove(t_node *node);

/**
 * @brief Validates and updates the movement of the rover based on the soil type.
 * @param node Pointer to the current node.
 * @param map The map being traversed.
 * @param remainingMoves Pointer to the remaining moves count.
 * @return -1 if the movement ends, 1 for valid movement, 0 for invalid movement.
 */
int validateAndUpdateMove(t_node *node, t_map map, int *remainingMoves);

/**
 * @brief Traverses the tree and simulates rover movements.
 * @param root Pointer to the root node.
 * @param map The map being used.
 * @param remainingMoves Number of remaining moves available.
 */
void traverseTree(t_node *root, t_map map, int remainingMoves);

/**
 * ========================
 * Pathfinding and Optimization
 * ========================
 */

/**
 * @brief Searches the tree to find a path that reaches the base station.
 * @param root Pointer to the current node.
 */
void findPath(t_node *root);

/**
 * @brief Finds the optimal path to the base station by evaluating costs.
 * @param root Pointer to the current node.
 * @param best Pointer to the best node found so far.
 */
void findBestPath(t_node *root, t_node **best);

/**
 * ========================
 * Utilities
 * ========================
 */

/**
 * @brief Prints the path recursively from the current node to the root.
 * @param node Pointer to the current node.
 */
void printPath(t_node *node);


/**
 * ========================
 * Main Simulation Execution
 * ========================
 */

/**
 * @brief Executes the entire simulation phase and determines the best path.
 * @param num_choices Number of choices for the root node.
 * @param min_choices Minimum number of choices for children nodes.
 * @param map The map being used.
 * @return Pointer to the root of the generated tree.
 */
t_node *executePhase(int num_choices, int min_choices, t_map map, int mode);
#endif //MARC_TREE_H