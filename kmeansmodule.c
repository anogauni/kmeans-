#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



struct cord
{
    double value;
    struct cord *next;
};

struct vector
{
    struct vector *next;
    struct cord *cords;
};

void initialize_cord(struct cord *c){
    c->value = 0.0;
    c->next = NULL;
}

void initialize_vector(struct vector *v){
    v->next = NULL;
    v->cords = NULL;
}

void freeVector(struct vector *vec){
    struct cord *cord, *temp_cord;
    if (!vec)
    {
        return;
    }
    cord = vec->cords;
    while (cord != NULL)
    {
        temp_cord = cord;
        cord = cord->next;
        free(temp_cord);
    }
    vec->cords = NULL;
}

void cleanupVectorsList(struct vector *head){
    struct vector *temp_vec;

    while (head != NULL)
    {
        temp_vec = head;
        head = head->next;

        freeVector(temp_vec);
        free(temp_vec);
    }
}

void cleanupArrayOfVectors(struct vector *vectors, int k) {
    struct cord *cord, *temp_cord;
    int i;

    for (i = 0; i < k; i++)
    {
        cord = vectors[i].cords;
        while (cord)
        {
            temp_cord = cord;
            cord = cord->next;
            free(temp_cord);
        }
        vectors[i].cords = NULL;
    }
}


void readDataToLinkedList(const char *dataStr, struct vector **data_vec) {
    struct vector *head_vec, *curr_vec;
    struct cord *head_cord, *curr_cord;
    int count = 0;
    int index = 0;

    head_cord = malloc(sizeof(struct cord));
    if (!head_cord)
    {
        return;
    }
    initialize_cord(head_cord);
    curr_cord = head_cord;

    head_vec = malloc(sizeof(struct vector));
    if (!head_vec)
    {

        free(head_cord);
        cleanupVectorsList(head_vec);
        return;
    }
    initialize_vector(head_vec);
    curr_vec = head_vec;
    curr_vec->next = NULL;

    while (dataStr[index]!= '\0')
    {
        if (dataStr[index] == ',')
        {
            curr_cord->next = malloc(sizeof(struct cord));
            if (!curr_cord->next)
            {
                free(head_cord);
                cleanupVectorsList(head_vec);
                return;
            }
            initialize_cord(curr_cord->next);
            curr_cord = curr_cord->next;
            index++;
        }
        else if (dataStr[index] == '\n')
        {
            count++;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            if (!curr_vec->next)
            {
                free(head_cord);
                cleanupVectorsList(head_vec);
                return;
            }
            initialize_vector(curr_vec->next);
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            if (!head_cord)
            {
                cleanupVectorsList(head_vec);
                return;
            }
            initialize_cord(head_cord);
            curr_cord = head_cord;
            index++;
        }
        else
        {
            curr_cord->value = atof(&dataStr[index]);
            while (dataStr[index] != ',' && dataStr[index] != '\n' && dataStr[index] != '\0')
            {
                index++;
            }
        }
    }
    *data_vec = head_vec;

}

struct vector *readCentersToArray(const char *centersStr, int k) {
    
    int i;
    int index = 0, vectorCount = 0;
    struct cord *curr_cord = NULL, *head_cord = NULL;

    struct vector *centers = malloc(sizeof(struct vector) * k);
    if (!centers) {
        return NULL;
    }

    for (i = 0; i < k; i++) {
        initialize_vector(&centers[i]);
        centers[i].next = NULL; 
    }

    while (centersStr[index] != '\0') {
        if (vectorCount >= k) {
            cleanupArrayOfVectors(centers, k); /*Free allocated memory*/ 
            return NULL;
        }

        /*Initialize cords for the current vector*/
        head_cord = malloc(sizeof(struct cord));
        if (!head_cord) {
            cleanupArrayOfVectors(centers, k);
            return NULL;
        }
        initialize_cord(head_cord);
        curr_cord = head_cord;

        /* Parse cords for the current vector*/
        while (centersStr[index] != '\0' && centersStr[index] != '\n') {
            if (centersStr[index] == ',') {
                curr_cord->next = malloc(sizeof(struct cord));
                if (!curr_cord->next) {
                    cleanupArrayOfVectors(centers, k);
                    return NULL;
                }
                initialize_cord(curr_cord->next);
                curr_cord = curr_cord->next;
                index++;
            } else {
                curr_cord->value = atof(&centersStr[index]);
                while (centersStr[index] != ',' && centersStr[index] != '\n' && centersStr[index] != '\0') {
                    index++;
                }
            }
        }

        /*Assign the head_cord to the current vector in the array*/
        centers[vectorCount].cords = head_cord;
        vectorCount++;
        if (centersStr[index] == '\n') {
            index++;
        }
    }

    /*Check if fewer vectors were parsed than expected*/
    if (vectorCount < k) {
        cleanupArrayOfVectors(centers, k);
        return NULL;
    }
    return centers;
}

/* This method receives pointers to two points of the same dimension and returns the distance between them.
The dimensions are pre-validated. */
double calculateDistance(struct vector *point1, struct vector *point2, int d)
{
    int i;
    double delta = 0.0;
    double coord_diff;
    struct cord *first_cord, *second_cord;

    if (!point1 || !point2)
    {
        return -1.0;
    }

    first_cord = point1->cords;
    second_cord = point2->cords;

    if (!first_cord || !second_cord)
    {
        return -1.0;
    }
    for (i = 0; i < d && first_cord != NULL && second_cord != NULL; i++)
    {
        coord_diff = pow(first_cord->value - second_cord->value, 2);
        first_cord = first_cord->next;
        second_cord = second_cord->next;
        delta += coord_diff;
    }

    delta = sqrt(delta);
    return delta;
}

/*Method that calculates convergence and returns 0 iff changes are under epsilon - > if 1 then we should continute iterating */
int calculateConvergence(struct vector old_centroids[], struct vector new_centroids[], int k, double epsilon, int d)
{
    struct vector *old_v;
    struct vector *new_v;
    int i;

    for (i = 0; i < k; i++)
    {

        old_v = &old_centroids[i];
        new_v = &new_centroids[i];


        if (calculateDistance(new_v, old_v, d) >= epsilon)
        {
            return 0;
        }
        
    }
    return 1;
}

/* Method that calculates the distance between a point to all the centroids, and returns the index of the cluster which it is closest to */
int calculateClosestCluster(struct vector centroids[], struct vector *point, int k, int d)
{
    int min_distance_idx;
    int i;
    double distance;

    double *distances = malloc(k * sizeof(double));
    if (!point || !point->cords)
    {
        return -1;
    }
    if (!distances)
    {
        return -1;
    }

    for (i = 0; i < k; i++)
    {
        if (!centroids[i].cords)
        {
            free(distances);
            return -1;
        }
        distance = calculateDistance(&centroids[i], point, d);
        if (distance < 0)
        {
            free(distances);
            return -1;
        }
        distances[i] = distance;
    }
    min_distance_idx = 0;
    for (i = 1; i < k; i++)
    {
        if (distances[i] < distances[min_distance_idx])
        {
            min_distance_idx = i;
        }
    }
    free(distances);
    return min_distance_idx;
}

/*Method that runs one iteration of the kmeans algorithm and returns a list of the updated centroids*/
struct vector *centroidIteration(int k, int d, struct vector oldcentroid[], struct vector *cur_point, int num_points)
{
    int i, p, j, l;
    int *counts;
    struct vector *new_list;
    int index_centroid;
    struct cord *new_list_cord;
    struct cord *cur_point_cord;
    struct cord *prev;
    struct vector *current_point;

    counts = calloc(k, sizeof(int));
    new_list = malloc(k * sizeof(struct vector));

    if (!counts || !new_list)
    {
        if (!counts && !new_list)
        {
            return NULL;
        }
        if (counts)
        {
            free(counts);
        }
        if (new_list)
        {
            free(new_list);
        }
        return NULL;
    }
    for (i = 0; i < k; i++)
    {
        initialize_vector(&new_list[i]);
    }
    for (i = 0; i < k; i++)
    {
        prev = NULL;
        new_list[i].next = NULL;
        new_list[i].cords = NULL;

        for (j = 0; j < d; j++)
        {
            struct cord *new_cord = malloc(sizeof(struct cord));
            if (!new_cord)
            {
                for (l = 0; l <= i; l++)
                {
                    struct cord *cord = new_list[l].cords, *temp_cord;
                    while (cord)
                    {
                        temp_cord = cord;
                        cord = cord->next;
                        free(temp_cord);
                    }
                }
                free(new_list);
                free(counts);
                return NULL;
            }
            new_cord->value = 0.0;
            new_cord->next = NULL;

            if (prev == NULL)
            {
                new_list[i].cords = new_cord;
            }
            else
            {
                prev->next = new_cord;
            }
            prev = new_cord;
        }
    }

    current_point = cur_point;
    for (p = 0; p < num_points; p++)
    {
        if (!current_point || !current_point->cords)
        {
            cleanupArrayOfVectors(new_list, k);
            free(counts);
            free(new_list);
            return NULL;
        }

        index_centroid = calculateClosestCluster(oldcentroid, current_point, k, d);
        if (index_centroid < 0 || index_centroid >= k)
        {
            cleanupArrayOfVectors(new_list, k);
            free(counts);
            free(new_list);
            return NULL;
        }

        new_list_cord = new_list[index_centroid].cords;
        cur_point_cord = current_point->cords;

        while (new_list_cord && cur_point_cord)
        {
            new_list_cord->value += cur_point_cord->value;
            new_list_cord = new_list_cord->next;
            cur_point_cord = cur_point_cord->next;
        }

        counts[index_centroid]++;

        current_point = current_point->next;
    }

    for (i = 0; i < k; i++)
    {
        struct cord *current_cord = new_list[i].cords;
        if (counts[i] > 0)
        {
            while (current_cord)
            {
                current_cord->value /= counts[i];
                current_cord = current_cord->next;
            }
        }
    }

    free(counts);
    return new_list;
}


static PyObject *fit(PyObject *self, PyObject *args){
    int k, iter, numPoints, numCoords, iteration;
    double epsilon;
    const char *dataStr, *centersStr;
    PyObject *py_res;
    int i;
    struct cord *curr_cord;
    struct vector *data_vec = NULL;
    struct vector *centers = NULL;
    struct vector *new_centroids = NULL;

    /* This parses the Python arguments*/
    if(!PyArg_ParseTuple(args, "iiiidss", &k, &iter, &numPoints, &numCoords, &epsilon, &dataStr, &centersStr)) {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }    
    
 

    readDataToLinkedList(dataStr, &data_vec);
    if (data_vec == NULL)
    {
        return NULL;
    }
    centers = readCentersToArray(centersStr, k);
    if (centers == NULL)
    {
        cleanupVectorsList(data_vec);
        return NULL;
    }

    iteration = 0;
    while (iteration < iter){
        new_centroids = centroidIteration(k, numCoords, centers, data_vec, numPoints);
        if (!new_centroids)
        {
            cleanupArrayOfVectors(centers, k);
            free(centers);
            cleanupVectorsList(data_vec);
            return NULL;
        }
        if (calculateConvergence(centers, new_centroids, k, epsilon, numCoords))
        {
            cleanupArrayOfVectors(centers, k);
            free(centers);
            break;
        }
        cleanupArrayOfVectors(centers, k);
        free(centers);
        centers = new_centroids;
        iteration++;
    }

    /* Finished iterations and preparing PyObject */

    py_res = PyList_New((Py_ssize_t)k);
    if (py_res == NULL) {
        cleanupArrayOfVectors(new_centroids, k);
        free(new_centroids);
        cleanupVectorsList(data_vec);
        return NULL;
    }
    for (i = 0; i < k; i++) {
        PyObject *py_res_i = PyList_New(0);
        if (py_res_i == NULL) {
            cleanupArrayOfVectors(new_centroids, k);
            free(new_centroids);
            cleanupVectorsList(data_vec);
            return NULL;
        }
        curr_cord = new_centroids[i].cords;
        while (curr_cord != NULL) {
            PyObject *py_cord = PyFloat_FromDouble(curr_cord->value);
            if (py_cord == NULL) {
                cleanupArrayOfVectors(new_centroids, k);
                free(new_centroids);
                cleanupVectorsList(data_vec);
                return NULL;
            }
            
            if (PyList_Append(py_res_i, py_cord) < 0) {
                Py_XDECREF(py_cord);
                cleanupArrayOfVectors(new_centroids, k);
                free(new_centroids);
                cleanupVectorsList(data_vec);
                return NULL;
            }
            Py_XDECREF(py_cord);
            curr_cord = curr_cord->next;
        }

        PyList_SetItem(py_res, i, py_res_i);
    }

    cleanupArrayOfVectors(new_centroids, k);
    free(new_centroids);
    cleanupVectorsList(data_vec);

    return py_res;
}

static PyMethodDef mykmeanssp_methods[] = {
    {"fit",                   /* the Python method name that will be used */
      (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters accepted for this function */
      PyDoc_STR("A method that runs the kmeans++ algorithm and accepts ints: k, iter, numPoints, numCoords,  double epsilon and strings dataStr, centersStr")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a sentinel. Python looks for this entry to know that all
                                of the functions for the module have been defined. */ 
};

static struct PyModuleDef mykmeanssp_module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    mykmeanssp_methods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_mykmeanssp(void){
    PyObject *m;
    m = PyModule_Create(&mykmeanssp_module);
    if (!m) {
        return NULL;
    }
    return m;
}