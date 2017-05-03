//
// Created by agamemnon on 17-4-29.
//

#ifndef ZTE_LARGE_HEURISTIC_H
#define ZTE_LARGE_HEURISTIC_H

#include "ItemPath.h"
#include "instance.h"
#include <set>

double insertCost(Target x, list<Target>::iterator it,
                  list <Target> &path_list, ListGraph &g,
                  ListGraph::EdgeMap<double> &edge_length,
                  vector<vector<double >> &dist) {
    double insert_cost = 0.0;
    Target prev = *it;
    Target next = *(++it);
    if (x.isNode) {
        insert_cost += distance(prev, x, g, dist);
        insert_cost += distance(x, next, g, dist);
    }
    if (x.isEdge) {
        double cost_u = 0;
        double cost_v = 0;
        Target u, v;
        u.isNode = true;
        u.id = g.id(g.u(g.edgeFromId(x.id)));
        v.isNode = true;
        v.id = g.id(g.v(g.edgeFromId(x.id)));
        cost_u += distance(prev, u, g, dist);
        cost_u += distance(v, next, g, dist);
        cost_u += edge_length[g.edgeFromId(x.id)];
        cost_v += distance(prev, v, g, dist);
        cost_v += distance(u, next, g, dist);
        cost_v += edge_length[g.edgeFromId(x.id)];
        if (cost_u < cost_v) {
            insert_cost += cost_u;
        } else {
            insert_cost += cost_v;
        }
    }
    insert_cost -= distance(prev, next, g, dist);
}

void insert(Target x, list <Target> &path_list, ListGraph &g,
            ListGraph::EdgeMap<double> &edge_length,
            vector<vector<double >> &dist) {
    double minimum_insert_cost = 1e10;
    double insert_cost;
    list<Target>::iterator best_insert_it = path_list.begin();
    for (list<Target>::iterator it = path_list.begin();
         it != --path_list.end(); ++it) {
        insert_cost = insertCost(x, it, path_list, g, edge_length, dist);
        if (insert_cost < minimum_insert_cost) {
            minimum_insert_cost = insert_cost;
            best_insert_it = it;
        }
    }
    if (x.isEdge) {
        list<Target>::iterator it = best_insert_it;
        Target prev = *it;
        Target next = *(++it);
        double cost_u = 0;
        double cost_v = 0;
        Target u, v;
        u.isNode = true;
        u.id = g.id(g.u(g.edgeFromId(x.id)));
        v.isNode = true;
        v.id = g.id(g.v(g.edgeFromId(x.id)));
        cost_u += distance(prev, u, g, dist);
        cost_u += distance(v, next, g, dist);
        cost_u += edge_length[g.edgeFromId(x.id)];
        cost_v += distance(prev, v, g, dist);
        cost_v += distance(u, next, g, dist);
        cost_v += edge_length[g.edgeFromId(x.id)];
        x.isEdge = false;
        x.isArc = true;
        if (cost_u < cost_v) {
            x.id = g.id(findArc(g, g.nodeFromId(u.id), g.nodeFromId(v.id)));
        } else {
            x.id = g.id(findArc(g, g.nodeFromId(v.id), g.nodeFromId(u.id)));
        }
    }
    path_list.insert(++best_insert_it, x);

}

void farthestInsertion(list <Target> &path_list,
                       ListGraph &g,
                       EdgeCost &edge_length,
                       set <Target> &target_set,
                       vector<vector<double>> &dist) {

    while (!target_set.empty()) {
        double farthest_distance = -1;
        Target farthest_target;
        for (auto &x:target_set) {
            double d = distance(x, path_list, g, dist);
            if (d > farthest_distance) {
                farthest_distance = d;
                farthest_target = x;
            }
        }
        insert(farthest_target, path_list, g, edge_length, dist);
        target_set.erase(farthest_target);
    }
}

void two_exchange(list <Target> &path_list,
                  list<Target>::iterator it1,
                  list<Target>::iterator it2,
                  ListGraph &g,
                  vector<vector<double>> &dist) {

}


#endif //ZTE_LARGE_HEURISTIC_H
