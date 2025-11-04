#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <string>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */

// Forward declarations for globals used by helpers
extern int rows;
extern int columns;
extern int total_mines;
extern int game_state;

// Internal game storage and helpers
namespace {
  constexpr int MAXN = 35; // safe upper bound (n, m <= 30 per constraints)
  bool mine[MAXN][MAXN];      // true if there is a mine
  bool visited[MAXN][MAXN];   // true if the cell has been visited (non-mine or mine)
  bool marked[MAXN][MAXN];    // true if the cell is marked as a mine

  const int dr[8] = {-1,-1,-1,0,0,1,1,1};
  const int dc[8] = {-1,0,1,-1,1,-1,0,1};

  inline bool in_bounds(int r, int c) {
    return r >= 0 && r < rows && c >= 0 && c < columns;
  }

  inline int adj_mines(int r, int c) {
    int cnt = 0;
    for (int k = 0; k < 8; ++k) {
      int nr = r + dr[k], nc = c + dc[k];
      if (in_bounds(nr, nc) && mine[nr][nc]) cnt++;
    }
    return cnt;
  }

  void reveal_zero_dfs(int r, int c) {
    if (!in_bounds(r, c)) return;
    if (visited[r][c]) return;
    if (marked[r][c]) return; // do not change marked cells
    if (mine[r][c]) return;   // never visit mines in expansion
    visited[r][c] = true;
    int cnt = adj_mines(r, c);
    if (cnt == 0) {
      for (int k = 0; k < 8; ++k) {
        int nr = r + dr[k], nc = c + dc[k];
        if (in_bounds(nr, nc) && !visited[nr][nc] && !mine[nr][nc]) {
          reveal_zero_dfs(nr, nc);
        }
      }
    }
  }

  inline int visited_non_mine_count() {
    int vc = 0;
    for (int i = 0; i < rows; ++i) for (int j = 0; j < columns; ++j)
      if (visited[i][j] && !mine[i][j]) vc++;
    return vc;
  }

  inline int marked_mine_count_fn() {
    int mc = 0;
    for (int i = 0; i < rows; ++i) for (int j = 0; j < columns; ++j)
      if (marked[i][j] && mine[i][j]) mc++;
    return mc;
  }

  inline void update_victory_if_needed() {
    if (game_state != 0) return; // do not override loss
    int total_non_mine = rows * columns - total_mines;
    if (visited_non_mine_count() == total_non_mine) {
      game_state = 1;
    }
  }
}

int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which
 * mines are located at (0, 1) and (1, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
  std::cin >> rows >> columns;
  // Initialize game state and internal data structures
  game_state = 0;
  total_mines = 0;

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch;
      std::cin >> ch;
      mine[i][j] = (ch == 'X');
      visited[i][j] = false;
      marked[i][j] = false;
      if (mine[i][j]) total_mines++;
    }
  }
}

/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), the return value would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, the return value would be -1 (game ends and the players loses) , and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, the return value of the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void VisitBlock(int r, int c) {
  if (game_state != 0) return; // game already ended
  if (!in_bounds(r, c)) return; // invalid
  if (visited[r][c] || marked[r][c]) { // no effect
    game_state = (game_state == 0 ? 0 : game_state);
    return;
  }
  if (mine[r][c]) {
    visited[r][c] = true; // reveal mine
    game_state = -1;
    return;
  }
  // Visit non-mine and auto-expand zeros
  reveal_zero_dfs(r, c);
  update_victory_if_needed();
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game) And you don't need to
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1?1
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {
  if (game_state != 0) return;
  if (!in_bounds(r, c)) return;
  if (visited[r][c] || marked[r][c]) return; // no effect
  if (mine[r][c]) {
    marked[r][c] = true;
    update_victory_if_needed(); // marking doesn't directly cause win, but visiting all safe cells might already be true
  } else {
    // Marking a non-mine causes immediate failure and shows 'X' at that cell per PrintMap rules
    marked[r][c] = true; // so PrintMap can show 'X' for wrongly marked
    game_state = -1;
  }
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */
void AutoExplore(int r, int c) {
  if (game_state != 0) return;
  if (!in_bounds(r, c)) return;
  if (!visited[r][c]) return; // must be a visited non-mine
  if (mine[r][c]) return;     // cannot auto-explore on a mine
  // Count marked neighbors
  int k = adj_mines(r, c);
  int marked_neighbors = 0;
  for (int t = 0; t < 8; ++t) {
    int nr = r + dr[t], nc = c + dc[t];
    if (in_bounds(nr, nc) && marked[nr][nc]) marked_neighbors++;
  }
  if (marked_neighbors != k) return;
  // Visit all non-mine neighbors that are not visited and not marked
  for (int t = 0; t < 8; ++t) {
    int nr = r + dr[t], nc = c + dc[t];
    if (!in_bounds(nr, nc)) continue;
    if (visited[nr][nc]) continue;
    if (marked[nr][nc]) continue;
    if (mine[nr][nc]) {
      // If auto-explore would visit a mine, it's not allowed per rules. Since we only visit non-mine grids here, skip.
      continue;
    }
    reveal_zero_dfs(nr, nc);
  }
  update_victory_if_needed();
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
  // Output result line
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
    std::cout << (rows * columns - total_mines) << " " << total_mines << std::endl;
  } else if (game_state == -1) {
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visited_non_mine_count() << " " << marked_mine_count_fn() << std::endl;
  } else {
    // Should not happen in normal flow, but keep consistent
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visited_non_mine_count() << " " << marked_mine_count_fn() << std::endl;
  }
  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
  // When victory: show '@' for all mines, digits for visited non-mines, '?' otherwise
  // When ongoing or loss: show according to rules
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char out = '?';
      if (game_state == 1) {
        if (mine[i][j]) out = '@';
        else if (visited[i][j]) out = char('0' + adj_mines(i, j));
        else out = '?';
      } else { // game_state == 0 or -1
        if (marked[i][j]) {
          if (mine[i][j]) out = '@';
          else out = 'X';
        } else if (visited[i][j]) {
          if (mine[i][j]) out = 'X';
          else out = char('0' + adj_mines(i, j));
        } else {
          out = '?';
        }
      }
      std::cout << out;
    }
    std::cout << std::endl;
  }
}

#endif
