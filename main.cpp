#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <fstream>
#include <chrono>
#include <mutex>

bool is_prime(int num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    int i = 5;
    while (i * i <= num) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
        i += 6;
    }
    return true;
}

void find_primes(int start, int end, std::vector<int>& primes, std::mutex& mtx) {
    for (int num = start; num < end; ++num) {
        if (is_prime(num)) {
            std::lock_guard<std::mutex> lock(mtx);
            primes.push_back(num);
        }
    }
}

void balanced_prime_check(int max_num, int num_threads) {
    int chunk_size = max_num / num_threads;
    std::vector<std::thread> threads;
    std::vector<int> primes;
    std::mutex mtx;
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = (i == num_threads - 1) ? max_num : (i + 1) * chunk_size;
        threads.emplace_back(find_primes, start, end, std::ref(primes), std::ref(mtx));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    int num_primes = primes.size();

    std::ofstream file("primes.txt");
    if (file.is_open()) {
        for (int prime : primes) {
            file << prime << "\n";
        }
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para escrita." << std::endl;
    }

    std::cout << "Número de threads: " << num_threads << std::endl;
    std::cout << "Tempo total de execução: " << total_time << " ms" << std::endl;
    std::cout << "Quantidade de números avaliados: " << max_num << std::endl;
    std::cout << "Quantidade de números primos encontrados: " << num_primes << std::endl;
}


void unbalanced_prime_check(int max_num, int num_threads) {
    std::vector<std::thread> threads;
    std::vector<int> primes;
    std::mutex mtx;
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        int start = (i * max_num) / num_threads;
        int end = ((i + 1) * max_num) / num_threads;
        threads.emplace_back(find_primes, start, end, std::ref(primes), std::ref(mtx));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    int num_primes = primes.size();

    std::ofstream file("primes_unbalanced.txt");
    if (file.is_open()) {
        for (int prime : primes) {
            file << prime << "\n";
        }
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para escrita." << std::endl;
    }

    std::cout << "Número de threads: " << num_threads << std::endl;
    std::cout << "Tempo total de execução: " << total_time << " ms" << std::endl;
    std::cout << "Quantidade de números avaliados: " << max_num << std::endl;
    std::cout << "Quantidade de números primos encontrados: " << num_primes << std::endl;
}


int main() {
    int max_num = 10000000;

    std::cout << "Executando programa balanceado:" << std::endl;
    for (int num_threads = 1; num_threads <= 10; ++num_threads) {
        std::cout << "Com " << num_threads << " threads:" << std::endl;
        balanced_prime_check(max_num, num_threads);
        std::cout << "-------------------------------------------------" << std::endl;
    }
    
    std::cout << "Executando programa desbalanceado:" << std::endl;
    for (int num_threads = 1; num_threads <= 10; ++num_threads) {
        std::cout << "Com " << num_threads << " threads:" << std::endl;
        unbalanced_prime_check(max_num, num_threads);
        std::cout << "-------------------------------------------------" << std::endl;
    }

    return 0;
}
