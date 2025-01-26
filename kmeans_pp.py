import numpy as np
import pandas as pd
import sys
import mykmeanssp

def validateIter(iter):
    try:
        iter = float(iter)
        return iter.is_integer() and 1 < iter < 1000
    except:
        return False

""" Method that validates user provided k and returns if k is an integer by value and between (1,N) """
def validateK(k):
    try:
        k = float(k)
        return k.is_integer()
    except:
        return False

"""Method which activates the kmeans algorithm
    @params:
    (int) k: for kmeans
    (int) iter: number of iterations (1 < iter < 1000)
    (float) epsilon: epsilon for convergence (epsilon >= 0)
    (string) file_name_1, file_name_2: names of files
"""
def kmeans(k, iter, epsilon, file_name_1, file_name_2):
    
    file1 = pd.read_csv(file_name_1, header=None).sort_values(by=0)
    file2 = pd.read_csv(file_name_2, header=None).sort_values(by=0)
    dataFrame = pd.merge(file1, file2, how='inner', on=0, suffixes=('_file1', '_file2'))
    data = np.delete(dataFrame.to_numpy(), 0, axis=1)
    num_points, num_coords = data.shape
    
    if k >= num_points or k <= 1:
        print("Invalid number of clusters!")
        sys.exit(1)
    
    # kmeans++ algorithm
    centers = np.zeros((k, num_coords))
    original_centroids_indexes = np.zeros(k)

    np.random.seed(1234)

    random_index = np.random.choice(num_points)
    original_centroids_indexes[0] = int(random_index)
    centers[0] = np.copy(data[random_index])

    for i in range(1, k):
        distances = np.array([min([np.linalg.norm(center_point - data_point) for center_point in centers[:i]]) for data_point in data])
        probs = distances / distances.sum()
        random_index = np.random.choice(num_points, p=probs)
        original_centroids_indexes[i] = int(random_index)
        centers[i] = np.copy(data[random_index])

    centersStr = '\n'.join([', '.join(map(str, row)) for row in centers])
    dataStr = '\n'.join([', '.join(map(str, row)) for row in data]) + '\n'

    res = mykmeanssp.fit(k, iter, num_points, num_coords, epsilon, dataStr, centersStr)
    
    if (res == None):
        print("An Error Has Occurred")
        sys.exit(1)
    else:
        print(','.join(map(str, map(int, original_centroids_indexes))))
        for i, centroid in enumerate(res):
            line = ",".join("{:.4f}".format(coordinate) for coordinate in centroid)
            print(line)

if __name__ == "__main__":
    try:
        if len(sys.argv) < 5:  # check if there are at least 4 args (k, eps, file_name_1, file_name_2) - iter is optional 
            print("An Error Has Occurred")
            sys.exit(1)  
        try:
            k = sys.argv[1]  # number of clusters
            if validateK(k) == False:
                print("Invalid number of clusters!")
                sys.exit(1)
            else:
                k = int(k)
            # check if iterations is provided
            if len(sys.argv) == 6:
                iter = sys.argv[2]  # read iterations if provided 
                epsilon = sys.argv[3]
                file_name_1 = sys.argv[4]
                file_name_2 = sys.argv[5]   
            elif len(sys.argv) == 5:  
                iter = 300  # default value for iterations
                epsilon = sys.argv[2]
                file_name_1 = sys.argv[3]
                file_name_2 = sys.argv[4]
            else:
                print("An Error Has Occurred")
                sys.exit(1)
            
            epsilon = float(epsilon)
            
            if epsilon < 0:
                print("Invalid epsilon!")
                sys.exit(1)
            if validateIter(iter) == False:
                print("Invalid maximum iteration!")
                sys.exit(1)
            else:
                iter = int(iter)

        except ValueError:
            print("An Error Has Occurred")
            sys.exit(1)
        
        # Call kmeans method and print result
        kmeans(k, iter, epsilon, file_name_1, file_name_2)

    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)