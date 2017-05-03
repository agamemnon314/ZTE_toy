//
// Created by agamemnon on 17-4-29.
//

#ifndef ZTE_LARGE_ITEMPATH_H
#define ZTE_LARGE_ITEMPATH_H

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>
#include <lemon/path.h>

using namespace std;
using namespace lemon;

struct Target {
    bool isNode;
    bool isEdge;
    bool isArc;
    int id;

    Target() {
        isNode = false;
        isEdge = false;
        isArc = false;
        id = -1;
    }

    void clear() {
        isNode = false;
        isEdge = false;
        isArc = false;
        id = -1;
    }

    Target &operator=(const Target &t) {
        this->isNode = t.isNode;
        this->isArc = t.isArc;
        this->isEdge = t.isEdge;
        this->id = t.id;
    }

    bool operator<(const Target &t) const {
        int a, b;
        if (isNode) {
            a = 1;
        }
        if (isEdge) {
            a = 2;
        }
        if (isArc) {
            a = 3;
        }
        if (t.isNode) {
            b = 1;
        }
        if (t.isEdge) {
            b = 2;
        }
        if (t.isArc) {
            b = 3;
        }
        if (a != b) {
            return a < b;
        } else {
            return id < t.id;
        }

    }
};

void print_list(list <Target> &path_list, ListGraph &g) {
    for (auto &x:path_list) {
        if (x.isNode) {
            cout << x.id << "--";
        }
        if (x.isArc) {
            cout << "(" << g.id(g.source(g.arcFromId(x.id))) << "->"
                 << g.id(g.target(g.arcFromId(x.id))) << ")--";
        }
    }
    cout << endl;
}

double distance(Target x, Target y,
                ListGraph &g, vector<vector<double >> &dist) {
    if (x.isNode) {
        if (y.isNode) {
            int id1 = x.id;
            int id2 = y.id;
            return dist[id1][id2];
        }
        if (y.isArc) {
            int id1 = x.id;
            int id2 = g.id(g.source(g.arcFromId(y.id)));
            return dist[id1][id2];
        }
        if (y.isEdge) {
            int id1 = x.id;
            int id2 = g.id(g.u(g.edgeFromId(y.id)));
            int id3 = g.id(g.v(g.edgeFromId(y.id)));
            if (dist[id1][id2] < dist[id1][id3]) {
                return dist[id1][id2];
            } else {
                return dist[id1][id3];
            }
        }
    }
    if (x.isArc) {
        if (y.isNode) {
            int id1 = g.id(g.target(g.arcFromId(x.id)));
            int id2 = y.id;
            return dist[id1][id2];
        }
        if (y.isArc) {
            int id1 = g.id(g.target(g.arcFromId(x.id)));
            int id2 = g.id(g.source(g.arcFromId(y.id)));
            return dist[id1][id2];
        }
        if (y.isEdge) {
            int id1 = g.id(g.target(g.arcFromId(x.id)));
            int id2 = g.id(g.u(g.edgeFromId(y.id)));
            int id3 = g.id(g.v(g.edgeFromId(y.id)));
            if (dist[id1][id2] < dist[id1][id3]) {
                return dist[id1][id2];
            } else {
                return dist[id1][id3];
            }
        }
    }
    if (x.isEdge) {
        if (y.isNode) {
            int id0 = g.id(g.u(g.edgeFromId(x.id)));
            int id1 = g.id(g.v(g.edgeFromId(x.id)));
            int id2 = y.id;
            if (dist[id0][id1] < dist[id1][id2]) {
                return dist[id0][id1];
            } else {
                return dist[id1][id2];
            }
        }
        if (y.isArc) {
            int id0 = g.id(g.u(g.edgeFromId(x.id)));
            int id1 = g.id(g.v(g.edgeFromId(x.id)));
            int id2 = g.id(g.source(g.arcFromId(y.id)));
            if (dist[id0][id1] < dist[id1][id2]) {
                return dist[id0][id1];
            } else {
                return dist[id1][id2];
            }
        }
        if (y.isEdge) {
            int id0 = g.id(g.u(g.edgeFromId(x.id)));
            int id1 = g.id(g.v(g.edgeFromId(x.id)));
            int id2 = g.id(g.u(g.arcFromId(y.id)));
            int id3 = g.id(g.v(g.arcFromId(y.id)));
            vector<double> xd;
            xd.push_back(dist[id0][id2]);
            xd.push_back(dist[id0][id3]);
            xd.push_back(dist[id1][id2]);
            xd.push_back(dist[id1][id3]);
            double d = 1e10;
            for (auto &a:xd) {
                if (a < d) {
                    d = a;
                }
            }
            return d;
        }
    }
}

double distance(Target x, list <Target> &path_list,
                ListGraph &g, vector<vector<double >> &dist) {
    double d = 1e10;
    for (auto &y:path_list) {
        double xd = distance(x, y, g, dist);
        if (xd < d) {
            d = xd;
        }
    }
}

double pathLength(list <Target> &path_list, ListGraph &g,
                  ListGraph::EdgeMap<double> &edge_length,
                  vector<vector<double >> &dist) {
    double path_length = 0;
    for (list<Target>::iterator it = path_list.begin();
         it != --path_list.end(); ++it) {
        list<Target>::iterator it_next = it;
        ++it_next;
        path_length += distance(*it, *it_next, g, dist);
        Target target = *it;
        if (target.isArc) {
            path_length += edge_length[g.arcFromId(target.id)];
        }
    }
}


#endif //ZTE_LARGE_ITEMPATH_H
