from scipy.optimize import linprog

# Coefficients of the objective function (cost per gram of each food)
cost = [0.5, 0.7, 0.75, 0.5, 0.4]  # Example costs (in $ per gram)

# Constraints coefficients
# Each row corresponds to a constraint: [calories, protein, fat, carbs, fiber]
nutritional_content = [
    [30, 30, 20, 10, 7],  # Food 1
    [30, 50, 15, 20, 10],  # Food 2
    [30, 30, 30, 40, 25],  # Food 3
    [40, 20, 10, 40, 15],  # Food 4
    [25, 15, 20, 15, 20],  # Food 5
]

# Required minimum nutritional values
nutritional_min = [180, 120, 70, 180, 79]  # Min: calories, protein, fat, carbs, fiber

# Constraints setup
A = [[-nutritional_content[j][i] for j in range(5)] for i in range(5)]  # Negative for >=
b = [-x for x in nutritional_min]  # Negative for >=

# Bounds for each decision variable (non-negative quantities)
bounds = [(0, None) for _ in range(5)]

# Solve the linear programming problem
result = linprog(c=cost, A_ub=A, b_ub=b, bounds=bounds, method="highs")


# Display results
if result.success:
    print("Optimal solution found:")
    for i, x in enumerate(result.x, 1):
        print(f"Food {i}: {x:.4f} grams")
    print(f"Minimum total cost: ${result.fun:.4f}")
else:
    print("No optimal solution found.")