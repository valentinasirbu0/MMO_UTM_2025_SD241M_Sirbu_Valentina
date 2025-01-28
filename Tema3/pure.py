import numpy as np

# Payoff matrix for two players (5x3 matrix)
payoff_matrix = np.array([
    [3, -1, 2],
    [4, 0, -3],
    [-2, 1, 5],
    [3, 1, 6],
    [1, -2, 4]
])

# Determine the best strategies for both players
def find_pure_strategy(matrix):
    # Player 1: Maximizing row minimum
    row_min = np.min(matrix, axis=1)
    player1_best = np.argmax(row_min)

    # Player 2: Minimizing column maximum
    col_max = np.max(matrix, axis=0)
    player2_best = np.argmin(col_max)

    return player1_best, player2_best

player1_strategy, player2_strategy = find_pure_strategy(payoff_matrix)

print("Pure Strategy Solution:")
print(f"Player 1 chooses strategy: {player1_strategy + 1}")
print(f"Player 2 chooses strategy: {player2_strategy + 1}")
