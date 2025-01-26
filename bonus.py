# bonus.py

import matplotlib.pyplot as plt
from sklearn.datasets import load_iris
from sklearn.cluster import KMeans

def elbow_method():
    try:
        iris = load_iris()
        data = iris.data  
        k_values = range(1, 11)
        inertias = []

        for k in k_values:
            kmeans = KMeans(n_clusters=k, init='k-means++', random_state=0)
            kmeans.fit(data)
            inertias.append(kmeans.inertia_)

        plt.figure()
        plt.plot(k_values, inertias, marker='o', linestyle='-', color='b', label='Inertia')
        plt.title('Elbow Method for selection of optimal "k" clusters')
        plt.xlabel('k')
        plt.ylabel('Average Dispersion')
        plt.xticks(k_values)
        plt.grid(False)

     
        elbow_point = calc_elbow(k_values, inertias)
        plt.annotate(f'Elbow (k={elbow_point})',
                     xy=(elbow_point, inertias[elbow_point - 1]),
                     xytext=(elbow_point + 3, inertias[elbow_point - 1] + 300),
                     arrowprops=dict(facecolor='black', arrowstyle='->'))

        plt.legend()


        plt.savefig("elbow.png")

    except Exception as e:
        print(f"An error occurred: {e}")

def calc_elbow(k_values, inertias):
    try:
        max_slope_change = 0
        elbow = k_values[1]

        for i in range(1, len(k_values) - 1):
            slope1 = inertias[i - 1] - inertias[i]
            slope2 = inertias[i] - inertias[i + 1]
            slope_change = abs(slope1 - slope2)

            if slope_change > max_slope_change:
                max_slope_change = slope_change
                elbow = k_values[i]

        return elbow
    except Exception as e:
        return None

if __name__ == "_main_":
    elbow_method()
