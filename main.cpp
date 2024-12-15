﻿#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>

void PrintSet(const std::unordered_set<std::string>& set) {
    int elem_count = 0;
    std::cout << "{ ";
    for (const auto& v : set) {
        if (elem_count >= 1) std::cout << ", ";
        std::cout << v;
        ++elem_count;
    }
    std::cout << " }\n";
}

class Graph {
public:
    Graph(const std::string& filename, bool is_log) : is_log_(is_log){
        LoadGraphFromFile(filename);
    }

    void FindIndependentSet() {
        std::unordered_set<std::string> independent_set;
        std::unordered_set<std::string> removed;

        while (!adj_.empty()) {
            std::string min_vertex;
            int min_degree = INT_MAX;

            for (const auto& vertex : adj_) {
                if (removed.find(vertex.first) == removed.end() && vertex.second.size() < min_degree) {
                    min_degree = vertex.second.size();
                    min_vertex = vertex.first;
                }
            }

            if (min_vertex.empty()) break;

            independent_set.insert(min_vertex);
            removed.insert(min_vertex);

            for (const auto& neighbor : adj_[min_vertex]) {
                removed.insert(neighbor);
            }
        }

        if (!is_log_) {
            std::cout << "Наибольшее независимое множество: ";
            PrintSet(independent_set);
        }
    }

    void FindVertexCover() {
        std::unordered_set<std::string> vertex_cover;
        std::unordered_set<std::string> removed;

        auto adj = adj_;

        while (!adj.empty()) {
            auto it = adj.begin();
            std::string u = it->first;

            if (it->second.empty()) {
                adj.erase(it);
                continue;
            }

            std::string v = it->second.front();

            vertex_cover.insert(u);
            vertex_cover.insert(v);

            removed.insert(u);
            removed.insert(v);

            for (const auto& neighbor : adj[u]) {
                removed.insert(neighbor);
            }
            for (const auto& neighbor : adj[v]) {
                removed.insert(neighbor);
            }

            for (auto it = adj.begin(); it != adj.end();) {
                if (removed.find(it->first) != removed.end()) it = adj.erase(it);
                else ++it;
            }

            for (auto& pair : adj) {
                std::vector<std::string> new_neighbors;
                for (const auto& neighbor : pair.second) {
                    if (removed.find(neighbor) == removed.end()) new_neighbors.push_back(neighbor);
                }
                pair.second = new_neighbors;
            }
            removed.clear();
        }

        if (!is_log_) {
            std::cout << "Наименьшее вершинное покрытие: ";
            PrintSet(vertex_cover);
        }
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> adj_;
    bool is_log_;

    void LoadGraphFromFile(const std::string& file_name) {
        std::ifstream in_file("input/" + file_name);
        if (!in_file.is_open()) throw std::runtime_error("Ошибка открытия файла: " + file_name);

        std::string line;
        while (getline(in_file, line)) {
            std::stringstream ss(line);
            std::string u, v;

            if (ss >> u) {
                if (ss >> v) AddEdge(u, v);
                else if (adj_.find(u) == adj_.end()) adj_[u] = {};
            }
        }
    }

    void AddEdge(const std::string& v, const std::string& w) {
        adj_[v].push_back(w);
        adj_[w].push_back(v);
    }
};

void Finder(const std::string& filename) {
    Graph g(filename, false);
    g.FindIndependentSet();
    g.FindVertexCover();
}

void LOG(std::ofstream& log_file, const std::string& filename) {
    Graph g(filename, true);
    double total_time = 0.0;
    for (int i = 0; i < 10; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        g.FindIndependentSet();
        g.FindVertexCover();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        log_file << duration << "\n";
        total_time += duration;
    }

    log_file << "Среднее время: " << total_time / 10 << "мкс\n\n";
}

int main() {    
    setlocale(LC_ALL, "rus");
    std::cout << "Выберете режим работы:\n";
    std::cout << "1. Найти наибольшее независимое множество вершин и наименьшее вершинное покрытие в графе\n";
    std::cout << "2. Получить данные о скорости выполнения\n";
    int choice;
    std::cin >> choice;
    system("cls");
    switch (choice)
    {
    case 1: {
        std::cout << "Введите имя файла для проверки\n";
        std::cout << "!Важно, что бы файл хранился в директроии input/\n";
        std::string filename;
        std::cin >> filename;
        system("cls");
        Finder(filename);
    }
          break;
    case 2: {
        std::ofstream log("log.txt");
        LOG(log, "test1.txt");
        LOG(log, "test2.txt");
        LOG(log, "test3.txt");
        log.close();
        system("cls");
        std::cout << "Успех!\nРезульаты работы программы были выведены в log.txt\n";
    }
          break;
    default:
        break;
    }

    return 0;
}
