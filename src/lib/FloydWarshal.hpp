// FloydWarshal.hpp

#ifndef FLOYDWARSHAL_H
#define FLOYDWARSHAL_H

#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

class FloydWarshal {
public:
    static constexpr unsigned INF = std::numeric_limits<unsigned>::max();

    FloydWarshal(unsigned size);

    void resize(unsigned newSize);

    void addEdgeWithMapping(unsigned u, unsigned v, unsigned w);
    unsigned valueWithMapping(unsigned u, unsigned v);
    unsigned nextWithMapping(unsigned u, unsigned v);
    bool hasPathWithMapping(unsigned u, unsigned v);

    void addEdge(unsigned u, unsigned v, unsigned w);
    unsigned value(unsigned u, unsigned v);
    bool hasPath(unsigned u, unsigned v);
    unsigned next(unsigned u, unsigned v);

    void clean();
    void generate();

    unsigned getSize() const;

private:
    unsigned size;
    std::vector<std::vector<unsigned>> graph;
    std::vector<std::vector<unsigned>> path;
    std::unordered_map<unsigned, unsigned> ids;
    unsigned last_key = 0;

    unsigned newKey();
};

#endif // FLOYDWARSHAL_H
