import os
import numpy as np

# Function to generate a square matrix of order 10 with double type numbers
def generate_matrix():
    return np.random.rand(10, 10)

# Directory where the input files will be saved
input_directory = 'inputs'
os.makedirs(input_directory, exist_ok=True)

# Name of the file that will contain the list of input files
list_file = 'file_list.txt'
list_file_path = os.path.join(input_directory, list_file)

# Open the list file for writing
with open(list_file_path, 'w') as list_f:
    # Generate 50 input files
    for i in range(1, 51):
        file_name = f'input_{i}.txt'
        file_path = os.path.join(input_directory, file_name)
        
        # Write the file name to the list
        list_f.write(f'{file_path}\n')
        
        # Generate two square matrices of order 10
        matrix1 = generate_matrix()
        matrix2 = generate_matrix()
        
        # Save the matrices in the input file
        with open(file_path, 'w') as input_f:
            np.savetxt(input_f, matrix1, fmt='%.6f')
            input_f.write('\n')
            np.savetxt(input_f, matrix2, fmt='%.6f')