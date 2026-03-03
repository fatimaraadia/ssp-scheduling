#ifndef NP_SCHEDULER_H
#define NP_SCHEDULER_H

#include <vector>
#include "taskgen.h"
#include "security_relations.h" // Access to securityRelations

class NPScheduler {
public:
    // Constructor with task set and security relations
    NPScheduler(std::vector<Task>& taskSet,
                const SecurityRelations& secRel);

    void printBlockingParameters() const;

    // Initialize beta_i = C_i + q_td
    void initializeBlockingParameters();

    // Schedulability test without cnt/splitting
    bool isSchedulable();

private:
    std::vector<Task>& taskSet;
    const SecurityRelations& securityRelations;

    // DBF for a single task
    double dbf(const Task& task, double L) const;

    // Total DBF
    double totalDBF(double L) const;

    // Maximum blocking by higher-security tasks
    double computeBlocking(double L) const;
};

#endif // NP_SCHEDULER_H