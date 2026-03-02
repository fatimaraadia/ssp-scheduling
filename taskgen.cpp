#include "taskgen.h"
#include <random>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

TaskGenerator::TaskGenerator(unsigned int seed) {
    if (seed == 0) {
        seed = std::random_device{}();
    }
    std::srand(seed);
}

double TaskGenerator::randomDouble(double min, double max) {
    return min + (max - min) * (double(std::rand()) / RAND_MAX);
}

int TaskGenerator::randomInteger(int min, int max) {
    return min + (std::rand() % (max - min + 1));
}

/*
UUniFast algorithm
*/
std::vector<double> TaskGenerator::uunifast(int n, double U) {

    //Generates n utilizations that sum exactly to U.

    std::vector<double> utilizations;
    double sumU = U;

    for (int i = 1; i < n; ++i) {
        double nextSumU = sumU * std::pow(randomDouble(0.0, 1.0), 1.0 / (n - i));
        utilizations.push_back(sumU - nextSumU);
        sumU = nextSumU;
    }

    utilizations.push_back(sumU);
    return utilizations;
}

Task TaskGenerator::generateTask(double utilization) {
    Task task;

    // Random period (continuous)
    task.T = randomDouble(10.0, 1000.0);

    // Execution time from utilization
    task.C = utilization * task.T;

    // Deadline <= period
    task.D = randomDouble(task.C, task.T);

    // Security level S(tau_i) from range [1,5]; i.e., there are 5 security classes
    task.S = randomInteger(1, 5);

    // Overhead
    task.q_td = randomDouble(0.0, task.C/10);
    task.q_su = randomDouble(0.0, task.C/10);

    return task;
}

std::vector<TaskSet> TaskGenerator::generateTaskSets(
    int numTaskSets,
    int tasksPerSet,
    double totalUtilization)
{
    std::vector<TaskSet> allSets;

    for (int s = 0; s < numTaskSets; ++s) {
        TaskSet taskSet;

        std::vector<double> utilizations =
            uunifast(tasksPerSet, totalUtilization);

        for (double u : utilizations) {
            taskSet.push_back(generateTask(u));
        }

        allSets.push_back(taskSet);
    }

    return allSets;
}

void TaskGenerator::printTaskSets(
    const std::vector<TaskSet>& taskSets) const
{
    std::cout << std::fixed << std::setprecision(4);

    for (size_t i = 0; i < taskSets.size(); ++i) {
        std::cout << "Task Set " << i + 1 << ":\n";

        for (size_t j = 0; j < taskSets[i].size(); ++j) {
            const Task& t = taskSets[i][j];

            std::cout << "  t_" << j + 1
                << " (C=" << t.C
                << ", T=" << t.T
                << ", D=" << t.D
                << ", S=" << t.S
                << ", q_td=" << t.q_td
                << ", q_su=" << t.q_su
                << ")\n";
        }

        std::cout << "\n";
    }
}

void TaskGenerator::saveTaskSetsToFile(
    const std::vector<TaskSet>& taskSets,
    const std::string& filename) const
{
    std::ofstream file(filename);
    file << std::fixed << std::setprecision(6);

    for (size_t i = 0; i < taskSets.size(); ++i) {
        file << "Task Set " << i + 1 << ":\n";

        for (size_t j = 0; j < taskSets[i].size(); ++j) {
            const Task& t = taskSets[i][j];

            file << "t_" << j + 1
                << " (C=" << t.C
                << ", T=" << t.T
                << ", D=" << t.D
                << ", S=" << t.S
                << ", q_td=" << t.q_td
                << ", q_su=" << t.q_su
                << ")\n";
        }

        file << "\n";
    }

    file.close();
}