//
// Created by agamemnon on 17-4-28.
//

#ifndef ZTE_TOY_SMALL_INSTANCE_H
#define ZTE_TOY_SMALL_INSTANCE_H
#include <iostream>
#include <lemon/list_graph.h>
#include "instance.h"
#include "milp.h"
#include "draw.h"
#include "algorithms.h"

void calculate_minimum_path_node_limit(Instance &inst,Solution &sol){
    ListGraph &g = inst.G;
    ArcFlag a_flag(g);
    NodeFlag u_flag(g);
    int milp_flag;
    inst.pathNodeLimit=100000;



}

#endif //ZTE_TOY_SMALL_INSTANCE_H
