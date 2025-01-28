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

void populateMatrix(std::array<std::array<string, 10>, 10> &matrix,
                    const int &initialPosition) {
  for (size_t i = 0; i < matrix.size(); ++i) {
    for (size_t j = 0; j < matrix[i].size(); ++j) {
      if (i == matrix[i].max_size() - 1 && j == initialPosition) {
        matrix[i][j] = "*";
      } else {
        matrix[i][j] = "_";
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

  auto position = initialPosition;
  while (true) {
    clearScreenToBeReDrawn();
    printMatrix(matrix);
    const auto keyboardInput = std::cin.get();
    position = newPositionOfPlayer(keyboardInput, position);

    populateMatrix(matrix, position);

    this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
