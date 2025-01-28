import numpy as np
from scipy.optimize import linprog

# Modified payoff matrix
M = np.array([
    [ 4.2, -5,  6.7],
    [ 1.13, 5.1, -1.6],
    [-2.86, 4.1, 3.13],
    [ 2.85, -4.7, 7.5],
    [ 8.55, 0.9, -4.2]
])


# Solve the game
num_strategies_A, num_strategies_B = M.shape

# Add a constant to make the matrix positive
c = np.max(M)
M_positive = M + c

# Problem for player A (maximization)
c_A = [-1] + [0] * num_strategies_A  # Maximize v
A_eq_A = np.hstack([np.array([[0]]), np.ones((1, num_strategies_A))])  # x1 + x2 + ... + xn = 1
b_eq_A = [1]

A_ub = np.hstack([np.ones((num_strategies_B, 1)), -M_positive.T])
b_ub = np.zeros(num_strategies_B)
bounds_A = [(0, None)] + [(0, 1)] * num_strategies_A

result_A = linprog(c_A, A_ub=A_ub, b_ub=b_ub, A_eq=A_eq_A, b_eq=b_eq_A, bounds=bounds_A, method='highs')

# Solution for Player A
v_A = result_A.fun * -1
strategy_A = result_A.x[1:]

# Problem for player B (minimization)
c_B = [1] + [0] * num_strategies_B  # Minimize v
A_eq_B = np.hstack([np.array([[0]]), np.ones((1, num_strategies_B))])  # y1 + y2 + ... + ym = 1
b_eq_B = [1]

A_ub_B = np.hstack([-np.ones((num_strategies_A, 1)), M_positive])
b_ub_B = np.zeros(num_strategies_A)
bounds_B = [(0, None)] + [(0, 1)] * num_strategies_B

result_B = linprog(c_B, A_ub=A_ub_B, b_ub=b_ub_B, A_eq=A_eq_B, b_eq=b_eq_B, bounds=bounds_B, method='highs')

# Solution for Player B
v_B = result_B.fun
strategy_B = result_B.x[1:]

# Display results
print("\nRezultatele jocului:")
print(f"Strategia mixtă a jucătorului A: {strategy_A}")
print(f"Strategia mixtă a jucătorului B: {strategy_B}")
