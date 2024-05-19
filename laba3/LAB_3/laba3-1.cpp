#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <cmath>
#include <chrono>
#include <vector>

int size = 0;
size_t thread_num = 2;

struct ThreadArg{
    double** matrix1;
    double** matrix2;
    double** res_matrix;
    int start_row;
    int end_row;
};

void delete_matrix(double** matrix)
{
    for(int i=0; i< size; ++i)
    {
        delete[]matrix[i];
    }

    delete[]matrix;
}

double** allocate_matrix_memory()
{
    double** matrix = new double*[size];

    for (int i = 0; i < size; ++i)
    {
        matrix[i] = new double[size];
    } 

    return matrix;
}


double** read_matrix(const char* filename){
    int fd = open(filename, O_RDONLY);

    if (fd < 0) 
    {
        std::cout << "open file error" << std::endl;
    }

    off_t file_size = lseek(fd,0,SEEK_END);
    size_t num_elements = file_size/sizeof(double);
    size = sqrt(num_elements);

    double** matrix = allocate_matrix_memory();
    lseek(fd,0,SEEK_SET);

    for(int i = 0; i<size;++i)
    {
        read(fd,matrix[i],size*sizeof (double));
    }

    close(fd);
    return matrix;
}

double** multiply_matrices(double** matrix1, double** matrix2) 
{
    double** res_matrix = allocate_matrix_memory();
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < size; ++i) 
    {
        for (int j = 0; j < size; ++j) 
        {
            res_matrix[i][j] = 0;
            for (int k = 0; k < size; ++k) 
            {
                res_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> time = end - start;
    std::cout << "Однопоточное время: " << time.count() << std::endl;

    return res_matrix;
}

void* thread_multiply(void* tmp){
    ThreadArg* arg = (ThreadArg*)tmp;

    for(int i = arg->start_row; i< arg->end_row;++i)
    {
        for(int j = 0; j< size; ++j)
        {
            arg->res_matrix[i][j] = 0;
            for ( int k =0; k< size; ++k)
            {
                arg->res_matrix[i][j] += arg->matrix1[i][k]*arg->matrix2[k][j];
            }
        }
    }

    return nullptr;
}

double** multiply_matrices_parallel( double** matrix1,  double** matrix2) 
{
    std::cout << "\nЧисло потоков = " << thread_num << std::endl;
    
    std::vector<pthread_t > threads;
    double** res_matrix = allocate_matrix_memory();
    int rows = size/thread_num;

    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < thread_num; ++i) 
    {
        auto arg = new ThreadArg{matrix1, matrix2, res_matrix, i*rows, (i+1)*rows};

        if(i== thread_num - 1)
        {
            arg->end_row = size;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, thread_multiply, arg);
        threads.push_back(tid);
    }
    
    for(size_t i = 0; i < thread_num; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> time = end - start;
    std::cout << "Многопоточное время: " << time.count() << std::endl;
    return res_matrix;
}

void check(double** matrix1, double** matrix2)
{
    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; j++)
        {
            if(matrix1[i][j] != matrix2[i][j])
            {
                std::cout << "Матрицы перемножены с ошибкой" << std::endl;
                return;
            }
        }
    }
    
    std::cout << "Матрицы перемножены верно" << std::endl;
}

int main() {
    double** matrix1 = read_matrix("mat1_big.bin");
    double** matrix2 = read_matrix("mat2_big.bin");
    
    std::cout << "Размер матриц = " << size << std::endl << std::endl;
    
    double** result_matrix = multiply_matrices(matrix1,matrix2);


    double** result = multiply_matrices_parallel(matrix1,matrix2);

    check(result_matrix, result);
    delete_matrix(matrix1);
    delete_matrix(matrix2);
    delete_matrix(result_matrix);
    return 0;
}