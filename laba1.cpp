#include <bits/stdc++.h>
#include <pthread.h>
#include <fstream>
#include <string>

int size_of_array;
int num_of_threads;
int pairs_per_thread;
int step;
int array[100];

bool is_sorted()
{
    for (int i = 1; i < size_of_array; ++i)
    {
        if (array[i - 1] > array[i])
            return false;
    }
    return true;
}

void printArray()
{
    for (int i = 0; i < size_of_array; ++i)
        std::cout << array[i] << " ";
    std::cout << '\n';
}

void calc_pairs (int size_of_array)
{
    if ((size_of_array / 2) % num_of_threads == 0)
    {
        pairs_per_thread = (size_of_array / 2) / num_of_threads;
    }
    else
    {
        pairs_per_thread = (size_of_array / 2) / num_of_threads + 1;
    }
}

void* compare_elements (void* arg)
{
    for(int i = 0; i < pairs_per_thread; ++i)
    {
        int position = step;
        step += 2;
        if (size_of_array > position + 1 && array[position + 1] < array[position])
        {
            std::swap(array[position], array[position + 1]);
        }
    }
    return NULL;

}

void odd_even_sort (pthread_t threads[])
{
    for(int i = 0; i < size_of_array; ++i)
    {
        if (i % 2 == 0)
        {
            step = 0;
            for (int k = 0; k < num_of_threads; ++k)
            {
                pthread_create(&threads[k], NULL, compare_elements, NULL);
            }
            for (int j = 0; j < num_of_threads; ++j)
            {
                pthread_join(threads[j], NULL);
            }
        }
        else
        {
            step = 1;
            for (int k = 0; k < num_of_threads; ++k)
            {
                pthread_create (&threads[k], NULL, compare_elements, NULL);
            }
            for (int j = 0; j < num_of_threads; ++j)
            {
                pthread_join(threads[j], NULL);
            }
        }

        std::cout << "Step " << i + 1 << ": ";
        printArray();
        if (is_sorted())
        {
            return;
        }
    }
}


int main()
{
    std::cout << "Number of threads: ";
    std::cin >> num_of_threads;
    std::ifstream in ("array.txt");
    int i = 0;
    if (in.is_open())
    {
        int x;
        while (in >> x)
        {
            array[i] = x;
            i++;
        }
    }
    in.close();
    size_of_array = i;
    std::cout << "\nGiven array: ";
    printArray();
    std::cout << '\n';
    calc_pairs(size_of_array);
    pthread_t threads[num_of_threads];
    odd_even_sort(threads);
    std::cout << "\nSorted array: ";
    printArray();
}
