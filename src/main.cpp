#include <iostream>

#include "game.h"
#include "input.h"

int main() {
    while (true) {
        int choice = readInt(
            "\nWelcome to Puzzles and Problems!\n"
            "Please select an option:\n"
            "1. N-Queens Problem Game\n"
            "2. Knight's Tour Game\n"
            "3. Exit Program\n"
            "Enter your choice: ");

        switch (choice) {
            case 1: {
                NQueensGame queens;
                queens.play();
                break;
            }
            case 2: {
                KnightsGame knights;
                knights.play();
                break;
            }
            case 3:
                std::cout << "Thanks for playing!\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}
