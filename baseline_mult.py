import numpy as np
import sys
import os
from scipy.sparse import csr_matrix
from datetime import datetime

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

def main(file1, file2):
    print(f"Processing files: {file1} and {file2}...")
    
    num_nodes1, adjacency_list1 = read_adjacency_list(file1)
    if adjacency_list1 is None:
        return
    
    num_nodes2, adjacency_list2 = read_adjacency_list(file2)
    if adjacency_list2 is None:
        return
    
    max_num_nodes = max(num_nodes1, num_nodes2)
    
    matrix1 = adjacency_list_to_sparse_matrix(max_num_nodes, adjacency_list1)
    matrix2 = adjacency_list_to_sparse_matrix(max_num_nodes, adjacency_list2)
    
    print("Multiplying matrices...")
    result_matrix = matrix1.dot(matrix2)
    print("Matrix multiplication completed.")
    
    new_adjacency_list = sparse_matrix_to_adjacency_list(result_matrix)
    
    current_date = datetime.now().strftime("%Y%m%d")
    output_file = f"mult_{current_date}.txt"
    save_adjacency_list(output_file, max_num_nodes, new_adjacency_list)
    print("Processing completed successfully.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 code.py <file1> <file2>")
        sys.exit(1)
    
    file1 = sys.argv[1]
    file2 = sys.argv[2]
    main(file1, file2)
