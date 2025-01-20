from pulp import LpMaximize, LpProblem, LpVariable

# Define the problem
problem = LpProblem("Production_Linear_Maximization", LpMaximize)

# Define decision variables
x1 = LpVariable("x1", lowBound=0)  # Production level of P1
x2 = LpVariable("x2", lowBound=0)  # Production level of P2
x3 = LpVariable("x3", lowBound=0)  # Production level of P3
x4 = LpVariable("x4", lowBound=0)  # Production level of P4
x5 = LpVariable("x5", lowBound=0)  # Production level of P5

# Objective function
problem += 50 * x1 + 60 * x2 + 65 * x3 + 53 * x4 + 90 * x5, "Total Profit"

# Constraints
problem += 5 * x1 + 6 * x2 + 2 * x3 + 7 * x4 + 5 * x5 <= 400, "Labor Constraint"
problem += 5 * x1 + 5 * x2 + 5 * x3 + 4 * x4 + 7 * x5 <= 350, "Material Constraint"
problem += x1 >= 10, "Demand for P1"
problem += x2 >= 10, "Demand for P2"
problem += x3 + x4 + x5 <= 50, "Demand for P3, P4, P5"

# Solve the problem
status = problem.solve()

# Print the results
print(f"Status: {problem.status}")
print(f"Objective (Maximum Profit): {problem.objective.value()}")
print(f"x1 (P1 Production): {x1.value()}")
print(f"x2 (P2 Production): {x2.value()}")
print(f"x3 (P3 Production): {x3.value()}")
print(f"x4 (P4 Production): {x4.value()}")
print(f"x5 (P5 Production): {x5.value()}")