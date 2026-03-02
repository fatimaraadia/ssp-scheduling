#ifndef SECURITY_RELATIONS_H
#define SECURITY_RELATIONS_H

#include "lattice.h"
#include <map>
#include <set>


class SecurityRelations {
public:
    // Constructor that builds the lookup table from a given lattice.
    SecurityRelations(const Lattice& lattice, const std::set<int>& elements);

    // Returns the pre-computed set of all Sj where Si is NOT <= Sj.
    const std::set<int>& getNonDominatingSet(int Si) const;

    // Prints the full lookup table for verification.
    void printTable() const;

private:
    // The lookup table: map<Si, set_of_Sj_where_Si_not_leq_Sj>>
    std::map<int, std::set<int>> nonDominatingLookup;
};

#endif // SECURITY_RELATIONS_H#pragma once
