from scipy.optimize import linprog
import numpy as np

# Datele problemei (10 surse și 3 destinații => 10 × 3 = 30 variabile decizionale)
costs = np.array([
    [12, 8, 15],
    [10, 18, 9],
    [14, 9, 19],
    [11, 12, 10],
    [20, 13, 11],
    [17, 15, 14],
    [10, 16, 12],
    [13, 11, 15],
    [18, 14, 9],
    [14, 10, 17]
])

# Oferta și cererea neechilibrate
supply = np.array([25, 30, 20, 40, 35, 15, 50, 30, 25, 30])  # Total supply: 300
demand = np.array([70, 100, 80])  # Total demand: 250

# Transformăm matricea costurilor într-un vector
c = costs.flatten()

# Construim matricea de constrângeri pentru surse și destinații
num_sources = len(supply)
num_destinations = len(demand)

A_eq = []

# Constrângeri pentru surse
for i in range(num_sources):
    row = [0] * (num_sources * num_destinations)
    for j in range(num_destinations):
        row[i * num_destinations + j] = 1
    A_eq.append(row)

# Constrângeri pentru destinații
for j in range(num_destinations):
    row = [0] * (num_sources * num_destinations)
    for i in range(num_sources):
        row[i * num_destinations + j] = 1
    A_eq.append(row)

A_eq = np.array(A_eq)

# Ajustăm b_eq pentru a permite neechilibrul
b_eq = np.concatenate((supply, demand))

# Modificăm vectorul de constrângeri pentru a permite surplus (<= in loc de =)
A_ub = np.vstack((A_eq[:num_sources], -A_eq[num_sources:]))
b_ub = np.concatenate((supply, -demand))

# Rezolvăm problema folosind linprog
result = linprog(c, A_ub=A_ub, b_ub=b_ub, method='highs')

if result.success:
    # Obținem soluția și costul total
    solution = result.x.reshape((num_sources, num_destinations))
    total_cost = result.fun

    # Afișăm rezultatele
    print("Cost minim total:", total_cost)
    print("Matricea soluției (cantitățile transportate):")
    print(solution)
else:
    print("Solverul nu a găsit o soluție. Verificați constrângerile problemei.")
