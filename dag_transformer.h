#ifndef DAG_TRANSFORMER_H
#define DAG_TRANSFORMER_H

#include "lattice.h"
#include "security_relations.h"
#include "taskgen.h"
#include <vector>
#include <map>
#include <set>
#include <memory>

// Represents the security properties of a class
struct SecurityContext {
    int C, I, A; // Confidentiality, Integrity, Availability
};

class DAGTransformer {
public:
    DAGTransformer(const TaskSet& taskSet); // Constructor
    void transformAndPrintBySecurityLevel() const;
    void buildLattice();
    void buildRelations();

private:
    // Defines the mapping from an integer security level to its properties
    const static std::map<int, SecurityContext> SECURITY_DEFINITIONS;

    const TaskSet& individualTaskSet;
    std::set<int> securityLevels; // Stores unique security levels from the task set
    mutable Lattice securityLattice; // Mutable to allow building in a const method context if needed
    mutable std::unique_ptr<SecurityRelations> securityRelations; // Lookup table
    void printGroupedTasks(const std::map<int, std::vector<Task>>& securityClasses) const;
};

#endif // DAG_TRANSFORMER_H