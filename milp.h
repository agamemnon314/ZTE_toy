//
// Created by agamemnon on 17-4-27.
//

#ifndef ZTE_TOY_MILP_H
#define ZTE_TOY_MILP_H

#include <ilcplex/ilocplex.h>
#include <iostream>
#include <lemon/connectivity.h>
#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include "instance.h"

ILOSTLBEGIN
using namespace std;
using namespace lemon;
typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;
typedef ListGraph::Arc Arc;
typedef ListGraph::ArcIt ArcIt;
typedef ListGraph::ArcMap<bool> ArcFlag;
typedef ListGraph::NodeMap<bool> NodeFlag;


ILOLAZYCONSTRAINTCALLBACK6(SubtourEliminationLazyCallback, const ListGraph &, g,
                           ListGraph::ArcMap<IloBoolVar> &, x,
                           ListGraph::NodeMap<IloBoolVar> &, y,
                           ListGraph::NodeMap<int> &, node_type,
                           IloRangeArray &, lazypool, IloCplex &, cplex) {
    IloEnv env = getEnv();

    ListGraph h;
    ListGraph::NodeMap<Node> nr(g);
    ListGraph::NodeMap<Node> ncr(h);
    for (NodeIt u(g); u != INVALID; ++u) {
        if (getValue(y[u]) > 0.99) {
            Node v = h.addNode();
            nr[u] = v;
            ncr[v] = u;
        }
    }
    for (ArcIt a(g); a != INVALID; ++a) {
        if (getValue(x[a]) > 0.99) {
            h.addEdge(nr[g.source(a)], nr[g.target(a)]);
        }
    }

    ListGraph::NodeMap<int> comp_h(h);
    ListGraph::NodeMap<int> comp_g(g);
    for (NodeIt u(g); u != INVALID; ++u) {
        comp_g[u] = -1;
    }
    int n_comp = 0;
    n_comp = connectedComponents(h, comp_h);
    for (NodeIt v(h); v != INVALID; ++v) {
        comp_g[ncr[v]] = comp_h[v];
    }
    if (n_comp > 1) {
        // cout << n_comp << endl;
        int depot_component_label = 0, terminal_component_label = 0;
        for (NodeIt u(g); u != INVALID; ++u) {
            if (node_type[u] == 2) {
                depot_component_label = comp_g[u];
            }
            if (node_type[u] == 3) {
                terminal_component_label = comp_g[u];
            }
        }

        // cout << "fuck!" << endl;
        IloExprArray out_exprs(env, n_comp);
        IloExprArray in_exprs(env, n_comp);
        for (int i = 0; i < n_comp; i++) {
            out_exprs[i] = IloExpr(env);
            in_exprs[i] = IloExpr(env);
        }
        for (ArcIt a(g); a != INVALID; ++a) {
            if (comp_g[g.source(a)] != comp_g[g.target(a)]) {
                if (comp_g[g.source(a)] != -1) {
                    out_exprs[comp_g[g.source(a)]] += x[a];
                }
                if (comp_g[g.target(a)] != -1) {
                    in_exprs[comp_g[g.target(a)]] += x[a];
                }
            }
        }

        for (int i = 0; i < n_comp; i++) {
            if (i != depot_component_label) {
                add(in_exprs[i] >= 1);
                lazypool.add(in_exprs[i] >= 1);
            }
            if (i != terminal_component_label) {
                add(out_exprs[i] >= 0);
                lazypool.add(out_exprs[i] >= 1);
            }
            out_exprs[i].end();
            in_exprs[i].end();
        }
        out_exprs.end();
        in_exprs.end();
    }

    h.clear();

    return;
}

int x_y_basic(const Instance &inst, ArcFlag &a_flag, NodeFlag &u_flag) {
    ListGraph &g = inst.G;
    ListGraph::EdgeMap<double> &edge_cost = inst.edgeCost;
    ListGraph::EdgeMap<int> &edge_type = inst.edgeType;
    ListGraph::NodeMap<int> &node_type = inst.nodeType;
    const int path_node_limit = inst.pathNodeLimit;
    int out_flag = 0;
    IloEnv env;
    try {
        IloModel model(env);

        // define edge variable x
        ListGraph::ArcMap<IloBoolVar> x(g);
        for (ArcIt a(g); a != INVALID; ++a) {
            x[a] = IloBoolVar(env);
        }

        // define node variable y
        ListGraph::NodeMap<IloBoolVar> y(g);
        for (NodeIt u(g); u != INVALID; ++u) {
            y[u] = IloBoolVar(env);
        }

        IloExpr expr(env);
        // constraints
        // degree constraints
        for (NodeIt u(g); u != INVALID; ++u) {
            // depot node
            if (node_type[u] == 2) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                // x(out)-x(in)==1
                model.add(expr == 1);
                expr.clear();
            }
            // terminal node
            if (node_type[u] == 3) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                // x(in)-x(out)==1
                model.add(expr == 1);
                expr.clear();
            }
            // target nodes
            if (node_type[u] == 1) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                // x(out)-x(in)==0
                model.add(expr == 0);
                expr.clear();
            }
            // other nodes
            if (node_type[u] == 0) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                // x(out)-x(in)==0
                model.add(expr == 0);
                expr.clear();
            }
        }

        // xij<=yi xij<=yj
        for (ArcIt a(g); a != INVALID; ++a) {
            model.add(x[a] <= y[g.source(a)]);
            model.add(x[a] <= y[g.target(a)]);
        }
        // x(delta(i))>yi
        for (NodeIt u(g); u != INVALID; ++u) {
            for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                expr += x[a];
            }
            if (node_type[u] != 3) {
                model.add(expr >= y[u]);
            }
            expr.clear();
            for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                expr += x[a];
            }
            if (node_type[u] != 2) {
                model.add(expr >= y[u]);
            }
            expr.clear();
        }

        // s,t must be visited
        for (NodeIt u(g); u != INVALID; ++u) {
            if (node_type[u] >= 1) {
                model.add(y[u] == 1);
            }
        }
        //  edge constraints
        for (EdgeIt e(g); e != INVALID; ++e) {
            if (edge_type[e] == 1) {
                IloConstraint cons =
                        x[g.direct(e, true)] + x[g.direct(e, false)] >= 1;
                model.add(cons);
            }
            if (edge_type[e] == -1) {
                IloConstraint cons =
                        x[g.direct(e, true)] + x[g.direct(e, false)] == 0;
                model.add(cons);
            }
        }
        // path node limit constraints
        for (ArcIt a(g); a != INVALID; ++a) {
            expr += x[a];
        }
        model.add(expr + 1 <= path_node_limit);

        expr.end();
        // edge cost
        IloExpr obj(env);
        for (ArcIt a(g); a != INVALID; ++a) {
            obj += edge_cost[a] * x[a];
        }

        model.add(IloMinimize(env, obj));
        obj.end();

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::Preprocessing::Presolve, IloFalse);
        cplex.setParam(IloCplex::Param::Threads, 1);
        cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                       IloCplex::Traditional);

        IloRangeArray lazypool(env);
        cplex.use(SubtourEliminationLazyCallback(env, g, x, y, node_type,
                                                 lazypool,
                                                 cplex));

        cplex.solve();
        if (cplex.getStatus() == IloAlgorithm::Optimal) {
            out_flag = (int) cplex.getObjValue();
            cplex.out() << "Optimal value: " << cplex.getObjValue() << endl;

            for (ArcIt a(g); a != INVALID; ++a) {
                if (cplex.getValue(x[a]) >= 0.9) {

                    a_flag[a] = 1;
                    cout << g.id(g.source(a)) << "--" << g.id(g.target(a))
                         << "  edege cost : " << edge_cost[a]
                         //                         << "  edege id : " << g.id(a)
                         //                         << "  edege flag : " << edge_type[a]
                         << endl;
                } else {
                    a_flag[a] = 0;
                }
            }
            for (NodeIt u(g); u != INVALID; ++u) {
                if (cplex.getValue(y[u]) >= 0.9) {
                    u_flag[u] = 1;
                } else {
                    u_flag[u] = 0;
                }
            }
        }
        if (cplex.getStatus() == IloAlgorithm::Infeasible) {
            out_flag = -1;
        }
    } catch (const IloException &e) {
        cerr << "Exception caught: " << e << endl;
    } catch (...) {
        cerr << "Unknown exception caught!" << endl;
    }

    env.end();
    return out_flag;
}

int x_y_steiner(const Instance &inst, ArcFlag &a_flag, NodeFlag &u_flag) {
    ListGraph &g = inst.G;
    ListGraph::EdgeMap<double> &edge_cost = inst.edgeCost;
    ListGraph::EdgeMap<int> &edge_type = inst.edgeType;
    ListGraph::NodeMap<int> &node_type = inst.nodeType;
    const int path_node_limit = inst.pathNodeLimit;
    const int target_node_punishment = inst.targetNodePunishment;
    const int target_edge_punishment = inst.targetEdgePunishment;
    const int forbidden_edge_punishment = inst.forbiddenEdgePunishment;
    const int path_node_limit_punishment = inst.pathNodeLimitPunishment;

    int out_flag = 0;
    IloEnv env;
    try {
        IloModel model(env);

        // define edge variable x
        ListGraph::ArcMap<IloBoolVar> x(g);
        for (ArcIt a(g); a != INVALID; ++a) {
            x[a] = IloBoolVar(env);
        }


        // define edge flag variable xs
        ListGraph::EdgeMap<IloBoolVar> xs(g);
        for (EdgeIt e(g); e != INVALID; ++e) {
            xs[e] = IloBoolVar(env);
        }



        // define node variable y
        ListGraph::NodeMap<IloBoolVar> y(g);
        for (NodeIt u(g); u != INVALID; ++u) {
            y[u] = IloBoolVar(env);
        }


        // define path length variable dp
        IloIntVar dp(env);


        IloExpr expr(env);
        // constraints
        // degree constraints
        for (NodeIt u(g); u != INVALID; ++u) {
            // depot node
            if (node_type[u] == 2) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                // x(out)-x(in)==1
                model.add(expr == 1);
                expr.clear();
            }
            // terminal node
            if (node_type[u] == 3) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                // x(in)-x(out)==1
                model.add(expr == 1);
                expr.clear();
            }
            // target nodes
            if (node_type[u] == 1) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                // x(out)-x(in)==0
                model.add(expr == 0);
                expr.clear();
            }
            // other nodes
            if (node_type[u] == 0) {
                for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                    expr += x[a];
                }
                for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                    expr -= x[a];
                }
                // x(out)-x(in)==0
                model.add(expr == 0);
                expr.clear();
            }
        }

        // xij<=yi xij<=yj
        for (ArcIt a(g); a != INVALID; ++a) {
            model.add(x[a] <= y[g.source(a)]);
            model.add(x[a] <= y[g.target(a)]);
        }
        // x(delta(i))>yi
        for (NodeIt u(g); u != INVALID; ++u) {
            for (ListGraph::OutArcIt a(g, u); a != INVALID; ++a) {
                expr += x[a];
            }
            if (node_type[u] != 3) {
                model.add(expr >= y[u]);
            }
            expr.clear();
            for (ListGraph::InArcIt a(g, u); a != INVALID; ++a) {
                expr += x[a];
            }
            if (node_type[u] != 2) {
                model.add(expr >= y[u]);
            }
            expr.clear();
        }
        if (target_node_punishment >= 0) {
            // s,t must be visited
            for (NodeIt u(g); u != INVALID; ++u) {
                if (node_type[u] >= 2) {
                    model.add(y[u] == 1);
                }
            }
        } else {
            // s,t and all target nodes must be visited
            for (NodeIt u(g); u != INVALID; ++u) {
                if (node_type[u] >= 1) {
                    model.add(y[u] == 1);
                }
            }
        }

        //  edge constraints
        if (forbidden_edge_punishment >= 0 || target_edge_punishment >= 0) {
            for (EdgeIt e(g); e != INVALID; ++e) {
                model.add(xs[e] >= x[g.direct(e, true)]);
                model.add(xs[e] >= x[g.direct(e, false)]);
                model.add(
                        xs[e] <= x[g.direct(e, true)] + x[g.direct(e, false)]);
            }
        }
        if (target_edge_punishment < 0) {
            for (EdgeIt e(g); e != INVALID; ++e) {
                if (edge_type[e] == 1) {
                    model.add(
                            x[g.direct(e, true)] + x[g.direct(e, false)] >= 1);
                }
            }
        }
        if (forbidden_edge_punishment < 0) {
            for (EdgeIt e(g); e != INVALID; ++e) {
                if (edge_type[e] == -1) {
                    model.add(
                            x[g.direct(e, true)] + x[g.direct(e, false)] == 0);
                }
            }
        }


        if (path_node_limit_punishment >= 0) {
            // path node limit constraints
            for (ArcIt a(g); a != INVALID; ++a) {
                expr += x[a];
            }
            model.add(dp >= expr + 1 - path_node_limit);
            model.add(dp >= 0);
        } else {
            for (ArcIt a(g); a != INVALID; ++a) {
                expr += x[a];
            }
            model.add(expr + 1 <= path_node_limit);
        }
        expr.clear();


        expr.end();

        // objective
        IloExpr obj(env);
        // edge cost
        for (ArcIt a(g); a != INVALID; ++a) {
            obj += edge_cost[a] * x[a];
        }
        if (target_node_punishment >= 0) {
            // target node punishment
            for (NodeIt u(g); u != INVALID; ++u) {
                if (node_type[u] == 1) {
                    obj += target_node_punishment * (1 - y[u]);
                }
            }
        }


        //edge punishment
        for (EdgeIt e(g); e != INVALID; ++e) {
            if (target_edge_punishment >= 0) {
                //target edge punishment
                if (edge_type[e] == 1) {
                    obj += target_edge_punishment * (1 - xs[e]);
                }
            }
            if (forbidden_edge_punishment >= 0) {
                //forbidden edge punishment
                if (edge_type[e] == -1) {
                    obj += forbidden_edge_punishment * (xs[e]);
                }
            }

        }

        if (path_node_limit_punishment >= 0) {
            //  path node limit punishment
            obj += path_node_limit_punishment * dp;
        }

        model.add(IloMinimize(env, obj));
        obj.end();

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::Preprocessing::Presolve, IloFalse);
        cplex.setParam(IloCplex::Param::Threads, 1);
        cplex.setParam(IloCplex::Param::MIP::Strategy::Search,
                       IloCplex::Traditional);

        IloRangeArray lazypool(env);
        cplex.use(SubtourEliminationLazyCallback(env, g, x, y, node_type,
                                                 lazypool,
                                                 cplex));


        ///////////////////////////////////////////////////////
//        cplex.setOut(env.getNullStream());
        cplex.setParam(IloCplex::Param::TimeLimit,1000);
        cplex.solve();
        cplex.out() << cplex.getStatus() << endl;
        if (cplex.getStatus() == IloAlgorithm::Optimal) {
            out_flag = (int) cplex.getObjValue();
            cplex.out() << "Optimal value: " << cplex.getObjValue() << endl;

            for (ArcIt a(g); a != INVALID; ++a) {
                if (cplex.getValue(x[a]) >= 0.9) {
                    a_flag[a] = 1;
                } else {
                    a_flag[a] = 0;
                }
            }
            for (NodeIt u(g); u != INVALID; ++u) {
                if (cplex.getValue(y[u]) >= 0.9) {
                    u_flag[u] = 1;
                } else {
                    u_flag[u] = 0;
                }
            }
        }
        if(cplex.getStatus()==IloAlgorithm::Feasible){
            out_flag = (int) cplex.getObjValue();
            cplex.out() << "Feasible solution value: " << cplex.getObjValue() << endl;

            for (ArcIt a(g); a != INVALID; ++a) {
                if (cplex.getValue(x[a]) >= 0.9) {
                    a_flag[a] = 1;
                } else {
                    a_flag[a] = 0;
                }
            }
            for (NodeIt u(g); u != INVALID; ++u) {
                if (cplex.getValue(y[u]) >= 0.9) {
                    u_flag[u] = 1;
                } else {
                    u_flag[u] = 0;
                }
            }
        }
        if (cplex.getStatus() == IloAlgorithm::Infeasible) {
            out_flag = -1;
        }
    } catch (const IloException &e) {
        cerr << "Exception caught: " << e << endl;
    } catch (...) {
        cerr << "Unknown exception caught!" << endl;
    }

    env.end();
    return out_flag;
}

#endif //ZTE_TOY_MILP_H
