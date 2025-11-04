#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <cstdlib>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

namespace {
  constexpr int MAXN = 35;
  char vis[MAXN][MAXN];
  const int dr[8] = {-1,-1,-1,0,0,1,1,1};
  const int dc[8] = {-1,0,1,-1,1,-1,0,1};
  inline bool in_bounds(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }
}

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  for (int i = 0; i < rows; ++i) for (int j = 0; j < columns; ++j) vis[i][j] = '?';
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  std::string line;
  for (int i = 0; i < rows; ++i) {
    std::cin >> line;
    for (int j = 0; j < columns; ++j) vis[i][j] = line[j];
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // 1) Auto-explore when marked_count == number
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch = vis[r][c];
      if (ch >= '0' && ch <= '8') {
        int k = ch - '0';
        int m = 0, u = 0;
        for (int t = 0; t < 8; ++t) {
          int nr = r + dr[t], nc = c + dc[t];
          if (!in_bounds(nr, nc)) continue;
          if (vis[nr][nc] == '@') m++;
          else if (vis[nr][nc] == '?') u++;
        }
        if (m == k && u > 0) { Execute(r, c, 2); return; }
      }
    }
  }
  // 2) Mark when m + u == number
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch = vis[r][c];
      if (ch >= '0' && ch <= '8') {
        int k = ch - '0';
        int m = 0; std::pair<int,int> any_unknown = {-1,-1}; int u = 0;
        for (int t = 0; t < 8; ++t) {
          int nr = r + dr[t], nc = c + dc[t];
          if (!in_bounds(nr, nc)) continue;
          if (vis[nr][nc] == '@') m++;
          else if (vis[nr][nc] == '?') { u++; if (any_unknown.first == -1) any_unknown = {nr, nc}; }
        }
        if (u > 0 && m + u == k) { Execute(any_unknown.first, any_unknown.second, 1); return; }
      }
    }
  }
  // 3) Fallback visit first unknown
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (vis[r][c] == '?') { Execute(r, c, 0); return; }
    }
  }
  // 4) No-op: try auto-explore somewhere safe
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (vis[r][c] >= '0' && vis[r][c] <= '8') { Execute(r, c, 2); return; }
    }
  }
}

#endif