#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "taskgen.h" // Assumes Task struct is defined here

class Scheduler {
public:
    // Constructor takes a reference to a task set (vector of Tasks)
    explicit Scheduler(std::vector<Task>& taskSet);

    // Step 1: Initialize blocking parameters
    // For each task_i in Gamma:
    //   beta_i = C_i + q_i^td
    //   cnt_i = 1
    void initializeBlockingParameters();
    void printBlockingParameters() const;

    double computeDBF(double L) const;
    std::vector<double> generateFeasibilitySet(double feasibilityBound) const;
    //bool isSchedulable(double feasibilityBound);
    bool isSchedulable();



    //helper functions
    double computeDmax() const;
    double computeFeasibilityBound() const;

    std::vector<double> generateTestingSet1() const;
    std::vector<double> generateTestingSet2() const;







private:
    std::vector<Task>& taskSet;
};

#endif // SCHEDULER_H