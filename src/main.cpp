#include "array"
#include <atomic>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <ostream>
#include <random>
#include <termios.h>
#include <thread>
#include <unistd.h>
using namespace std;

template <std::size_t SIZE>
void printMatrix(const std::array<std::array<string, SIZE>, SIZE> &input) {
  for (auto row : input) {
    for (auto element : row) {
      cout << element;
    }
    cout << std::endl;
  }
  cout << std::endl;
}

int generateInitialPosition(int arrLength) {
  // Random number engine
  std::random_device rd; // A seed source for the random number engine
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> disInt(0, arrLength -
                                                1); // inclusive at the min, max
  return disInt(gen);
}

std::string const DEFAULT_PLACEHOLDER = "_";
std::string const PLAYER_PLACEHOLDER = "*";
std::string const OBSTACLE_PLACEHOLDER = "|";
void populateMatrix(std::array<std::array<string, 10>, 10> &matrix,
                    const int &initialPosition) {
  for (size_t i = 0; i < matrix.size(); ++i) {
    for (size_t j = 0; j < matrix[i].size(); ++j) {
      if (matrix[i][j] == OBSTACLE_PLACEHOLDER) {
        continue;
      }
      if (i == matrix[i].max_size() - 1 && j == initialPosition) {
        matrix[i][j] = "*";
      } else {
        matrix[i][j] = "_";
      }
    }
  }
}

auto const FROM_WHICH_ROW_TO_ADD_OBSTACLES = 3;
auto const HOW_OFTEN_AFTER_FIRST_ROW_TO_ADD_OBSTACLES = 2;
auto const HOW_MANY_OBSTACLES_PER_ROW = 2;
void addObstaclesEveryRow(std::array<std::array<string, 10>, 10> &matrix) {
  for (size_t i = matrix.size() - FROM_WHICH_ROW_TO_ADD_OBSTACLES;
       i >= 0 && i < matrix.size();
       i -= HOW_OFTEN_AFTER_FIRST_ROW_TO_ADD_OBSTACLES) {
    // for (size_t indexToCheckIfObstaclesAreInitialized = 0;
    //      indexToCheckIfObstaclesAreInitialized < matrix[i].size();
    //      indexToCheckIfObstaclesAreInitialized++) {
    //   if (matrix[i][indexToCheckIfObstaclesAreInitialized] ==
    //           OBSTACLE_PLACEHOLDER ||
    //       matrix[i][indexToCheckIfObstaclesAreInitialized] ==
    //           PLAYER_PLACEHOLDER) {
    //     continue;
    //   }
    // }
    for (size_t j = 0; j < HOW_MANY_OBSTACLES_PER_ROW && j < matrix[i].size();
         ++j) {
      auto const elementIndexToBeConvertedToObstacle =
          generateInitialPosition(matrix[i].size());
      matrix[i][elementIndexToBeConvertedToObstacle] = OBSTACLE_PLACEHOLDER;
    }
  }
}

void addObstacleToLastRowIfItObeysRules(
    std::array<std::array<std::string, 10>, 10> &matrix) {
  bool addObstacles = true;
  for (size_t i = 0;
       i < std::min(matrix.size(),
                    (size_t)(HOW_OFTEN_AFTER_FIRST_ROW_TO_ADD_OBSTACLES));
       ++i) {
    for (const auto &cell : matrix[i]) {
      if (cell == OBSTACLE_PLACEHOLDER) {
        addObstacles = false;
      }
    }
  }

  if (addObstacles) {
    for (size_t j = 0; j < HOW_MANY_OBSTACLES_PER_ROW && j < matrix[0].size();
         ++j) {
      size_t elementIndexToBeConvertedToObstacle =
          generateInitialPosition(matrix[0].size());

      while (matrix[0][elementIndexToBeConvertedToObstacle] ==
             OBSTACLE_PLACEHOLDER) {
        elementIndexToBeConvertedToObstacle =
            generateInitialPosition(matrix[0].size());
      }

      matrix[0][elementIndexToBeConvertedToObstacle] = OBSTACLE_PLACEHOLDER;
    }
  }
}

void moveDownPlaceholdersInMatrix(
    std::array<std::array<std::string, 10>, 10> &matrix) {
  for (size_t i = 0; i < matrix.size() - 1; ++i) {  // Go from top to bottom
    for (size_t j = 0; j < matrix[i].size(); ++j) { // Check each column
      if (matrix[i][j] != DEFAULT_PLACEHOLDER &&
          matrix[i + 1][j] == DEFAULT_PLACEHOLDER) {
        // Swap if current is non-placeholder and the one below is a placeholder
        std::swap(matrix[i][j], matrix[i + 1][j]);
        // Since we've moved this element, skip checking it again in the next
        // iteration
        i++;
        if (i >= matrix.size() - 1)
          break; // Avoid going out of bounds
      }
    }
  }
}

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT_A 97
#define KEY_RIGHT_D 100
int newPositionOfPlayer(int input, int position) {
  switch (input) {
  case KEY_LEFT_A:
    return position - 1;
  case KEY_RIGHT_D:
    return position + 1;
  default:
    return position;
  }
}
void clearScreenToBeReDrawn() { std::cout << "\033[2J\033[1;1H"; }

int main() {
  auto matrix = std::array<std::array<string, 10>, 10>();
  const auto initialPosition = generateInitialPosition(matrix[0].max_size());
  populateMatrix(matrix, initialPosition);

  struct termios original_termios, raw;
  tcgetattr(STDIN_FILENO, &original_termios);
  raw = original_termios;
  raw.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

  addObstaclesEveryRow(matrix);

  auto position = initialPosition;
  while (true) {
    clearScreenToBeReDrawn();
    addObstacleToLastRowIfItObeysRules(matrix);
    printMatrix(matrix);
    const auto keyboardInput = std::cin.get();
    position = newPositionOfPlayer(keyboardInput, position);

    populateMatrix(matrix, position);

    this_thread::sleep_for(std::chrono::milliseconds(50));

    moveDownPlaceholdersInMatrix(matrix);
  }
}
