#include "asteroids.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int depthFirst(int field[][FIELD_WIDTH], int row, int column, int depth, int middle_bias){
    // Base case 1 - Out of bounds or asteroid value - need to backtrack and find another route
    int collision = 0;  // boolean
    if (row < 0 || row >= FIELD_HEIGHT || column < 0 || column >= FIELD_WIDTH)
        collision = 1;
    else {
        for (int i = 0; i < SHIP_WIDTH; i++) { // Check for collision for any part of the width of the ship
            if (column + i >= FIELD_WIDTH || field[row][column + i] == ASTEROID_VAL || field[row][column] == -1) {
                collision = 1;
                break;
            }
        }
    }
    if (collision)
        return INT_MAX; // Set to max value - most costly so won't be used

    // Base case 2 - Reached the right-hand side of the matrix - success
    if (column == FIELD_WIDTH - SHIP_WIDTH)
        return depth + abs(row - middle_bias); // need abs to ensure the value is positive

    field[row][column] = -1; // Mark this cell as visited
    int current = depthFirst(field, row, column + 1, depth + 1, middle_bias);
    int up = depthFirst(field, row - 1, column + 1, depth + 1, middle_bias);
    int down = depthFirst(field, row + 1, column + 1, depth + 1, middle_bias);

    int values[] = {up, current, down};
    int minDepth = values[1];  // Want to find the least costly path

    for (int i = 0; i < 3; i++) {
        if (values[i] < minDepth)
            minDepth = values[i];
    }

    return minDepth;
}

struct ship_action move_ship(int field[][FIELD_WIDTH], void *ship_state) {
    struct shipState {
        int ship_row;
    };

    struct shipState *s;  // Pointer to ship state struct which contains the ship row
    if (ship_state == NULL) {  // Happens the first time the code is run
        s = (struct shipState *) malloc(sizeof(struct shipState));
        s->ship_row = -1;  // Not initialized yet so set to -1
    } else {
        s = (struct shipState *) ship_state; // Need to cast it because it is originally a void pointer
    }

    if (s->ship_row == -1) {  // For the first round of the game we need to find the ship row
        for (int i = 0; i < FIELD_HEIGHT; i++) {
            if (field[i][0] == SHIP_VAL) {
                s->ship_row = i;
                break;
            }
        }
    }
    int shipRow = s->ship_row;

    int myfield[FIELD_HEIGHT][FIELD_WIDTH];
    memcpy(myfield, field, FIELD_WIDTH * FIELD_HEIGHT * sizeof(int));

    int middle_bias = FIELD_HEIGHT / 2; // want to try to keep ship near the middle
    int current_depth = depthFirst(myfield, shipRow, 1, 1, middle_bias);
    int up_depth = depthFirst(myfield, shipRow - 1, 1, 1, middle_bias);
    int down_depth = depthFirst(myfield, shipRow + 1, 1, 1, middle_bias);

    int move;
    if (up_depth <= current_depth && up_depth <= down_depth) {
        move = MOVE_UP;
        s->ship_row--;
    } else if (down_depth <= current_depth && down_depth <= up_depth) {
        move = MOVE_DOWN;
        s->ship_row++;
    } else {
        // Randomly explore up or down when there is a tie in order to check other paths
        srand(time(NULL));
        int random_choice = rand() % 2;
        if (random_choice == 0) {
            if (up_depth < current_depth) {
                move = MOVE_UP;
                s->ship_row--;
            } else {
                move = MOVE_NO;
            }
        } else {
            if (down_depth < current_depth) {
                move = MOVE_DOWN;
                s->ship_row++;
            } else {
                move = MOVE_NO;
            }
        }
    }

        struct ship_action result;
        result.move = move;
        result.state = (void *) s;
        return result;

}

