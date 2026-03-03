#include "dag_transformer.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

DAGTransformer::DAGTransformer(const TaskSet& taskSet) : individualTaskSet(taskSet) {}

// Define the properties for each security level S.
// This creates a partial order where, for example, S=2 and S=3 are incomparable.
const std::map<int, SecurityContext> DAGTransformer::SECURITY_DEFINITIONS = {
    {1, {0, 0, 0}}, // Lowest: No C, I, or A requirements
    {2, {1, 0, 0}}, // Confidential
    {3, {0, 1, 0}}, // Integrity
    {4, {1, 1, 0}}, // Confidential and Integrity
    {5, {1, 1, 1}}  // Highest: All properties required
};

void DAGTransformer::transformAndPrintBySecurityLevel() const {
    std::map<int, std::vector<Task>> securityClasses;

    // Group tasks by security level S
    for (const auto& task : individualTaskSet) {
        securityClasses[task.S].push_back(task);
    }

    securityLattice.printLattice(); // Assumes lattice is already built
    if (securityRelations) securityRelations->printTable();
    //printGroupedTasks(securityClasses);
}

void DAGTransformer::buildLattice() {
    // Step 1: Extract all unique security levels from the tasks.
    for (const auto& task : individualTaskSet) {
        securityLevels.insert(task.S);
    }
    for (int level : securityLevels) {
        securityLattice.addElement(level);
    }

    // Automatically build the partial order based on security definitions.
    // A level 's1' is less than or equal to 's2' if all of its security
    // properties are less than or equal to those of 's2'.
    for (int s1 : securityLevels) {
        for (int s2 : securityLevels) {
            if (s1 == s2) continue;

            const auto& def1 = SECURITY_DEFINITIONS.at(s1);
            const auto& def2 = SECURITY_DEFINITIONS.at(s2);

            // Check for dominance: s1 <= s2
            if (def1.C <= def2.C && def1.I <= def2.I && def1.A <= def2.A) {
                // We only need to add direct relationships; the closure will handle the rest.
                // However, adding all valid <= relations is also correct and simpler to implement here.
                securityLattice.addOrder(s1, s2);
            }
        }
    }

    securityLattice.buildTransitiveClosure();
}

void DAGTransformer::buildRelations() {
    // Step 2: Use the completed lattice to build the relations lookup table.
    // This assumes buildLattice() has already been called.
    securityRelations = std::make_unique<SecurityRelations>(securityLattice, securityLevels);
}

void DAGTransformer::printGroupedTasks(const std::map<int, std::vector<Task>>& securityClasses) const {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "----------------------------------------\n";

    // Print tasks grouped by security class
    for (const auto& pair : securityClasses) {
        std::cout << "Security Class (S = " << pair.first << "):\n";
        // Using a range-based for loop for cleaner iteration
        for (const auto& task : pair.second) {
            std::cout << "  Task (C=" << task.C
                << ", T=" << task.T
                << ", D=" << task.D
                << ", S=" << task.S
                << ", q_td=" << task.q_td
                << ", q_su=" << task.q_su
                << ")\n";
        }
        std::cout << "\n";
    }
}

const SecurityRelations& DAGTransformer::getSecurityRelations() const {
    return *securityRelations;
}