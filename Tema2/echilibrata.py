from scipy.optimize import linprog
import numpy as np

# New Cost matrix
costs = np.array([
    [2, 3, 1, 4, 5, 6],  # D1
    [4, 1, 3, 2, 6, 5],  # D2
    [3, 5, 2, 6, 4, 1],  # D3
    [5, 6, 4, 3, 2, 1],  # D4
    [1, 2, 3, 5, 4, 6]   # D5
]).flatten()

# Adjusted Capacities (supply from Depozite)
capacities = [50, 60, 40, 70, 80]

# Adjusted Demands (requirements for Distributie)
demands = [40, 50, 60, 70, 50, 30]

# Number of depots and distribution points
m, n = len(capacities), len(demands)

# Constraint matrices
A_eq = np.zeros((m + n, m * n))

# Supply constraints (rows for each depot)
for i in range(m):
    A_eq[i, i * n:(i + 1) * n] = 1

# Demand constraints (columns for each distribution point)
for j in range(n):
    A_eq[m + j, j::n] = 1

# Right-hand side of constraints
b_eq = capacities + demands

# Solve the problem
result = linprog(costs, A_eq=A_eq, b_eq=b_eq, bounds=(0, None), method='highs')

# Display the results
if result.success:
    solution = result.x.reshape((m, n))
    print("Cost minim total:", result.fun)
    print("Matricea soluției (cantitățile transportate):")
    print(solution)
else:
    print("Problema nu a putut fi rezolvată.")