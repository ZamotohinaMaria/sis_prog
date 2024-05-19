#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <chrono>
#include <vector>
#include <chrono>

int size = 0;
size_t thread_num = 3;
pthread_mutex_t  mutex ;
volatile int  result_index = -1;

struct ThreadArg{
    int* array;
    int number;
    int start_index;
    int end_index;
};

void delete_array(int* array)
{
    delete[]array;
}

int* allocate_array_memory()
{
    int* array = new int[size];
    return array;
}

int* read_array(const char* filename)
{
    int fd = open(filename, O_RDONLY);

    off_t file_size = lseek(fd,0,SEEK_END);
    size = file_size/sizeof(int);

    int* array = allocate_array_memory();
    lseek(fd,0,SEEK_SET);

    read(fd, array, size * sizeof (int));
    array[999998] = 1000;
    close(fd);
    return array;
}

void search(int* array, int number)
{
    int result = -1;
    
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < size; ++i)
    {
        if(array[i] == number)
        {
            result = i;
            break;
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> time = end - start;

    std::cout << "Однопоточное время: " << time.count() << std::endl;
    std::cout << "Индекс элемента = " << result << std::endl;
}

void* thread_search(void* tmp)
{
    ThreadArg* arg = (ThreadArg*)tmp;
    int local_index = -1;

    for (int i = arg->start_index; i < arg->end_index; ++i)
    {
        if(arg->array[i] == arg->number)
        {
            local_index = i;
            break;
        }
    }

    pthread_mutex_lock(&mutex);

    if (local_index != -1 && local_index < result_index || result_index == -1)
    {
        result_index = local_index;
    }

    pthread_mutex_unlock(&mutex);
    delete arg;
    return nullptr;
}

void search_parallel(int* array, int number)
{ 
    std::cout << "\nЧисло потоков = " << thread_num << std::endl;

    pthread_mutex_init(&mutex, NULL);
    std::vector<pthread_t > threads;

    int slice = size/thread_num;
    int start_index = 0;
    int end_index = 0;
    
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < thread_num; ++i)
    {
        auto arg = new ThreadArg{array, number ,i*slice, (i+1)*slice};
        if (i == thread_num - 1)
        {
            arg->end_index = size; 
        }

        pthread_t tid;
        pthread_create(&tid, NULL, thread_search, arg);
        threads.push_back(tid);
        start_index += slice;
    }

    for(size_t i = 0; i < thread_num; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> time = end - start;
    std::cout << "Многопоточное время: " << time.count() << std::endl;
    std::cout << "Индекс элемента = " << result_index << std::endl;
}

int main() {
    int* array = read_array("array_big.bin");
    std::cout << "Размер массива = " << size << std::endl << std::endl;

    int number = 1000;
    search(array, number);
    search_parallel(array, number);

    delete_array(array);
    return 0;
}