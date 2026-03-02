#pragma once
#ifndef TASKGEN_H
#define TASKGEN_H

#include <vector>
#include <string>

struct Task {
    double C;      // Execution time
    double T;      // Period
    double D;      // Deadline
    int S;      // Security parameter S(tau_i)
    double q_td;   // teardown overhead
    double q_su;   // startup overhead

    //Defining task parameters needed for the scheduler class
    double beta;
    int cnt;
};

using TaskSet = std::vector<Task>;

class TaskGenerator {
public:
    TaskGenerator(unsigned int seed = 0);

    std::vector<TaskSet> generateTaskSets(
        int numTaskSets,
        int tasksPerSet,
        double totalUtilization
    );

    void printTaskSets(const std::vector<TaskSet>& taskSets) const;

    void saveTaskSetsToFile(
        const std::vector<TaskSet>& taskSets,
        const std::string& filename
    ) const;

private:
    std::vector<double> uunifast(int n, double U);

    double randomDouble(double min, double max);
    int randomInteger(int min, int max);

    Task generateTask(double utilization);
};

#endif