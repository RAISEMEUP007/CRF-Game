// FloydWarshal.cpp

#include "FloydWarshal.hpp"

FloydWarshal::FloydWarshal(unsigned size) : size(size) {
    clean();
}

void FloydWarshal::resize(unsigned newSize) {
    size = newSize;
    clean();
}

unsigned FloydWarshal::newKey() {
    int x;
    return last_key++;
}

void FloydWarshal::addEdgeWithMapping(unsigned u, unsigned v, unsigned w) {
    if (ids.find(u) == ids.end()) {
        ids[u] = newKey();
    }
    if (ids.find(v) == ids.end()) {
        ids[v] = newKey();
    }
    addEdge(ids[u], ids[v], w);
}

unsigned FloydWarshal::valueWithMapping(unsigned u, unsigned v) {
    return value(ids.at(u), ids.at(v));
}

unsigned FloydWarshal::nextWithMapping(unsigned u, unsigned v) {
    unsigned val = next(ids.at(u), ids.at(v));
    for (const auto& entry : ids) {
        if (entry.second == val) {
            return entry.first;
        }
    }
    return INF;
}

bool FloydWarshal::hasPathWithMapping(unsigned u, unsigned v) {
    if (ids.find(u) == ids.end() || ids.find(v) == ids.end()) {
        return false;
    }
    return hasPath(ids[u], ids[v]);
}

void FloydWarshal::addEdge(unsigned u, unsigned v, unsigned w) {
    graph[u][v] = w;
}

unsigned FloydWarshal::value(unsigned u, unsigned v) {
    return graph[u][v];
}

bool FloydWarshal::hasPath(unsigned u, unsigned v) {
    return graph[u][v] != INF;
}

unsigned FloydWarshal::next(unsigned u, unsigned v) {
    return path[u][v];
}

void FloydWarshal::clean() {
    last_key = 0;
    ids.clear();

    graph.assign(size, std::vector<unsigned>(size, INF));
    path.assign(size, std::vector<unsigned>(size));

    for (unsigned i = 0; i < size; ++i) {
        for (unsigned j = 0; j < size; ++j) {
            path[i][j] = j;
            if (i == j) {
                graph[i][j] = 0;
            }
        }
    }
}

void FloydWarshal::generate() {
    for (unsigned k = 0; k < size; ++k) {
        for (unsigned i = 0; i < size; ++i) {
            for (unsigned j = 0; j < size; ++j) {
                if (graph[i][k] != INF && graph[k][j] != INF &&
                    graph[i][k] + graph[k][j] < graph[i][j]) {
                    graph[i][j] = graph[i][k] + graph[k][j];
                    path[i][j] = path[i][k];
                }
            }
        }
    }
}

unsigned FloydWarshal::getSize() const {
    return size;
}
