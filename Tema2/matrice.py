import numpy as np


# Read input data from file
def read_tsp_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    coords = []
    start_reading = False
    for line in lines:
        if line.startswith("NODE_COORD_SECTION"):
            start_reading = True
            continue
        if line.startswith("EOF"):
            break
        if start_reading:
            parts = line.split()
            coords.append((int(parts[0]) - 1, float(parts[1]), float(parts[2])))  # Convert to zero-based index

    return coords


# Define the number of cities
filename = "st70.tsp"  # Replace with the actual file path
cities = read_tsp_file(filename)
num_cities = len(cities)

# Compute the distance matrix with 0s where no direct road exists
distance_matrix = np.zeros((num_cities, num_cities))
for i in range(num_cities):
    for j in range(num_cities):
        if i != j:
            x1, y1 = cities[i][1], cities[i][2]
            x2, y2 = cities[j][1], cities[j][2]
            distance = np.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)
            distance_matrix[i, j] = distance
        else:
            distance_matrix[i, j] = 0  # No self-loop

# Print distance matrix in a format suitable for Excel
print("Distance Matrix:")
for row in distance_matrix:
    print("\t".join(map(lambda x: f"{x:.2f}", row)))