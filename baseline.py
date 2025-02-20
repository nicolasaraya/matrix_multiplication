import numpy as np
import sys
import os
from scipy.sparse import csr_matrix, identity
from scipy.sparse.linalg import expm

def read_adjacency_list(file_path):
    print(f"Reading adjacency list from {file_path}...")
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
        
        num_nodes = int(lines[0].strip())
        adjacency_list = {}
        
        for line in lines[1:]:
            parts = line.strip().split(':')
            if len(parts) != 2:
                continue  # Skip malformed lines
            node = int(parts[0])
            neighbors = sorted(list(map(int, parts[1].split()))) if parts[1].strip() else []
            if neighbors:
                adjacency_list[node] = neighbors
        
        print("Finished reading adjacency list.")
        return num_nodes, adjacency_list
    except Exception as e:
        print(f"Error reading file: {e}")
        return None, None

def adjacency_list_to_sparse_matrix(num_nodes, adjacency_list):
    print("Converting adjacency list to sparse matrix...")
    row, col, data = [], [], []
    for node, neighbors in adjacency_list.items():
        for neighbor in neighbors:
            row.append(node - 1)
            col.append(neighbor - 1)
            data.append(1)
    matrix = csr_matrix((data, (row, col)), shape=(num_nodes, num_nodes))
    print("Conversion to sparse matrix completed.")
    return matrix

def sparse_matrix_to_adjacency_list(matrix):
    print("Converting sparse matrix back to adjacency list...")
    adjacency_list = {}
    coo = matrix.tocoo()
    total_entries = len(coo.row)
    for index, (i, j) in enumerate(zip(coo.row, coo.col)):
        if i + 1 not in adjacency_list:
            adjacency_list[i + 1] = []
        adjacency_list[i + 1].append(j + 1)
        if index % (total_entries // 10 + 1) == 0:
            print(f"Progress: {index / total_entries * 100:.2f}%")
    for node in adjacency_list:
        adjacency_list[node].sort()
    print("Conversion to adjacency list completed.")
    return adjacency_list

def save_adjacency_list(file_path, num_nodes, adjacency_list):
    print(f"Saving adjacency list to {file_path}...")
    try:
        with open(file_path, 'w') as file:
            file.write(f"{num_nodes}\n")
            for node in sorted(adjacency_list.keys()):
                neighbors_str = ' '.join(map(str, adjacency_list[node]))
                file.write(f"{node}: {neighbors_str}\n")
        print(f"Result successfully saved to {file_path}")
    except Exception as e:
        print(f"Error writing file: {e}")

def main(input_file, power):
    print(f"Processing file: {input_file} with power {power}...")
    num_nodes, adjacency_list = read_adjacency_list(input_file)
    if adjacency_list is None:
        return
    
    matrix = adjacency_list_to_sparse_matrix(num_nodes, adjacency_list)
    
    if power > 1:
        print(f"Raising matrix to power {power}...")
        matrix = matrix ** power  # Elevar la matriz dispersa a la potencia
        print("Matrix exponentiation completed.")
    
    new_adjacency_list = sparse_matrix_to_adjacency_list(matrix)
    
    output_file = f"{os.path.splitext(input_file)[0]}_output.txt"
    save_adjacency_list(output_file, num_nodes, new_adjacency_list)
    print("Processing completed successfully.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 code.py <input_file>")
        sys.exit(1)
    
    input_filename = sys.argv[1]
    power = 2  # Potencia por defecto
    main(input_filename, power)