//
// Created by agamemnon on 17-4-27.
//

#ifndef ZTE_TOY_DRAW_H

#include <cairo.h>
#include <iostream>
#include <lemon/dim2.h>
#include <lemon/list_graph.h>
#include <string>
#include "instance.h"

using namespace std;
using namespace lemon;
struct RGBA {
    double r;
    double g;
    double b;
    double a;
};

typedef ListGraph::Node Node;
typedef ListGraph::NodeIt NodeIt;
typedef ListGraph::Edge Edge;
typedef ListGraph::EdgeIt EdgeIt;
typedef ListGraph::Arc Arc;
typedef ListGraph::ArcIt ArcIt;
typedef dim2::Point<double> Point;
typedef ListGraph::NodeMap<bool> NodeFlag;
typedef ListGraph::EdgeMap<bool> EdgeFlag;
typedef ListGraph::ArcMap<bool> ArcFlag;
typedef ListGraph::NodeMap<Point> NodePos;
typedef ListGraph::NodeMap<RGBA> NodeRGBA;
typedef ListGraph::EdgeMap<RGBA> EdgeRGBA;
typedef ListGraph::ArcMap<RGBA> ArcRGBA;
typedef ListGraph::NodeMap<double> NodeSize;
typedef ListGraph::EdgeMap<double> EdgeWidth;
typedef ListGraph::ArcMap<double> ArcWidth;
typedef ListGraph::EdgeMap<string> EdgeLabel;

void draw_network_nodes(ListGraph &g, NodeFlag &flag, NodePos &pos,
                        NodeRGBA &rgba, NodeSize &size, RGBA line_color,
                        cairo_t *cr) {
    for (NodeIt u(g); u != INVALID; ++u) {
        if (flag[u]) {
            cairo_set_line_width(cr, 1.0);
            cairo_set_source_rgba(cr, line_color.r, line_color.g, line_color.b,
                                  line_color.a);
            cairo_arc(cr, pos[u][0], pos[u][1], size[u], 0, 2 * M_PI);
            cairo_stroke(cr);

            cairo_arc(cr, pos[u][0], pos[u][1], size[u], 0, 2 * M_PI);
            cairo_set_source_rgba(cr, rgba[u].r, rgba[u].g, rgba[u].b,
                                  rgba[u].a);
            cairo_fill(cr);
            cairo_stroke(cr);
        }
    }
}

void draw_network_node_labels(ListGraph &g, NodeFlag &flag, NodePos &pos,
                              NodeSize &size, RGBA font_color, cairo_t *cr) {
    cairo_set_source_rgba(cr, font_color.r, font_color.g, font_color.b,
                          font_color.a);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    string label_s;
    const char *label_c;
    cairo_text_extents_t extents;
    for (NodeIt u(g); u != INVALID; ++u) {
        if (flag[u]) {
            label_s = to_string(g.id(u));
            label_c = label_s.data();
            cairo_set_font_size(cr, 0.8 * size[u]);
            cairo_text_extents(cr, label_c, &extents);
            cairo_move_to(cr,
                          pos[u][0] - (extents.width / 2 + extents.x_bearing),
                          pos[u][1] - (extents.height / 2 + extents.y_bearing));
            cairo_show_text(cr, label_c);

            cairo_stroke(cr);
        }
    }
}

void draw_network_edges(ListGraph &g, EdgeFlag &flag, NodePos &pos,
                        EdgeWidth &width, EdgeRGBA &e_color, cairo_t *cr) {

    for (EdgeIt e(g); e != INVALID; ++e) {
        if (flag[e]) {
            cairo_set_source_rgba(cr, e_color[e].r, e_color[e].g, e_color[e].b,
                                  e_color[e].a);
            cairo_set_line_width(cr, width[e]);
            cairo_move_to(cr, pos[g.u(e)][0], pos[g.u(e)][1]);
            cairo_line_to(cr, pos[g.v(e)][0], pos[g.v(e)][1]);
            cairo_stroke(cr);
        }
    }
}

void draw_network_edge_labels(ListGraph &g, EdgeFlag &flag, NodePos &pos,
                              EdgeLabel &label, EdgeRGBA &font_color,
                              cairo_t *cr) {
    cairo_text_extents_t extents;
    const char *label_c;
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);

    for (EdgeIt e(g); e != INVALID; ++e) {
        if (flag[e]) {
            cairo_set_source_rgba(cr, font_color[e].r, font_color[e].g,
                                  font_color[e].b, font_color[e].a);
            label_c = label[e].data();
            // cout << label_c << endl;
            cairo_set_font_size(cr, 10);
            cairo_text_extents(cr, label_c, &extents);
            cairo_move_to(cr,
                          (pos[g.u(e)][0] + pos[g.v(e)][0]) / 2 -
                          (extents.width / 2 + extents.x_bearing),
                          (pos[g.u(e)][1] + pos[g.v(e)][1]) / 2 -
                          (extents.height / 2 + extents.y_bearing));
            cairo_show_text(cr, label_c);

            cairo_stroke(cr);
        }
    }
}

void draw_network_arcs(ListGraph &g, ArcFlag &a_flag, NodePos &pos,
                       ArcWidth &a_width, ArcRGBA &a_color, cairo_t *cr) {
    for (ArcIt a(g); a != INVALID; ++a) {
        if (a_flag[a]) {
            cairo_set_source_rgba(cr, a_color[a].r, a_color[a].g, a_color[a].b,
                                  a_color[a].a);
            cairo_set_line_width(cr, a_width[a]);
            cairo_move_to(cr, pos[g.u(a)][0], pos[g.u(a)][1]);
            cairo_line_to(cr, pos[g.v(a)][0], pos[g.v(a)][1]);

            cairo_stroke(cr);
        }
    }
}

void draw_instance(Instance &inst,
                   const int figure_width,
                   const int figure_height,
                   const int node_size,
                   const string figure_name) {

    ListGraph &g = inst.G;
    ListGraph::EdgeMap<double> &edge_cost = inst.edgeCost;
    ListGraph::EdgeMap<int> &edge_type = inst.edgeType;
    ListGraph::NodeMap<int> &node_type = inst.nodeType;
    ListGraph::NodeMap<Point> &pos = inst.nodePos;
    cairo_surface_t *surface = cairo_image_surface_create(
            CAIRO_FORMAT_ARGB32, figure_width, figure_height);
    cairo_t *cr = cairo_create(surface);

    cairo_rectangle(cr, 0, 0, figure_width, figure_height);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_fill(cr);
    NodeFlag flag(g);
    EdgeFlag e_flag(g);
    NodeRGBA rgba(g);
    EdgeRGBA e_rgba(g);
    NodeSize size(g);
    EdgeWidth e_width(g);
    EdgeLabel e_label(g);
    EdgeRGBA e_font_color(g);
    const RGBA LightBlue = {123.0 / 255.0, 199.0 / 255.0, 1, 1};
    const RGBA Blue = {0.0 / 255.0, 142.0 / 255.0, 211.0 / 255.0, 1};
    const RGBA LightGreen = {34.0 / 255.0, 177.0 / 255.0, 76.0 / 255.0, 1};
    const RGBA LightYellow = {255.0 / 255.0, 192.0 / 255.0, 0.0 / 255.0, 1};
    const RGBA LightRed = {255.0 / 255.0, 24.0 / 255.0, 24.0 / 255.0, 1};
    const RGBA Black = {0.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 1};

    for (NodeIt u(g); u != INVALID; ++u) {
        flag[u] = 1;
        if (node_type[u] == 0)
            rgba[u] = LightBlue;
        if (node_type[u] == 1)
            rgba[u] = LightGreen;
        if (node_type[u] >= 2)
            rgba[u] = LightYellow;
        size[u] = node_size;
    }

    for (EdgeIt e(g); e != INVALID; ++e) {
        e_flag[e] = 1;
        string l = to_string(edge_cost[e]);

        e_label[e] = l.substr(0, l.find(".") + 2);
        e_font_color[e] = Black;
        if (edge_type[e] == 0) {
            e_rgba[e] = Blue;
            e_width[e] = 1;
        }
        if (edge_type[e] == 1) {
            e_rgba[e] = LightGreen;
            e_width[e] = 3;
        }
        if (edge_type[e] == -1) {
            e_rgba[e] = LightRed;
            e_width[e] = 3;
        }
    }

    draw_network_edges(g, e_flag, pos, e_width, e_rgba, cr);
    draw_network_edge_labels(g, e_flag, pos, e_label, e_font_color, cr);
    draw_network_nodes(g, flag, pos, rgba, size, Blue, cr);
    draw_network_node_labels(g, flag, pos, size, Black, cr);

    string filename_s = figure_name + ".png";
    const char *filename_c = filename_s.data();
    cairo_surface_write_to_png(surface, filename_c);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void draw_solution(Instance &inst,
                   ArcFlag &a_flag,
                   NodeFlag &flag,
                   const int figure_width,
                   const int figure_height,
                   const int node_size,
                   const string figure_name) {

    ListGraph &g = inst.G;
    ListGraph::EdgeMap<double> &edge_cost = inst.edgeCost;
    ListGraph::EdgeMap<int> &edge_type = inst.edgeType;
    ListGraph::NodeMap<int> &node_type = inst.nodeType;
    ListGraph::NodeMap<Point> &pos = inst.nodePos;
    cairo_surface_t *surface = cairo_image_surface_create(
            CAIRO_FORMAT_ARGB32, figure_width, figure_height);
    cairo_t *cr = cairo_create(surface);

    cairo_rectangle(cr, 0, 0, figure_width, figure_height);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_fill(cr);
    // NodeFlag flag(g);
    NodeRGBA rgba(g);
    NodeSize size(g);

    const RGBA LightBlue = {123.0 / 255.0, 199.0 / 255.0, 1, 1};
    const RGBA Blue = {0.0 / 255.0, 142.0 / 255.0, 211.0 / 255.0, 1};
    const RGBA LightGreen = {34.0 / 255.0, 177.0 / 255.0, 76.0 / 255.0, 1};
    const RGBA LightYellow = {255.0 / 255.0, 192.0 / 255.0, 0.0 / 255.0, 1};
    const RGBA LightRed = {255.0 / 255.0, 24.0 / 255.0, 24.0 / 255.0, 1};
    const RGBA Black = {0.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 1};

    for (NodeIt u(g); u != INVALID; ++u) {
        // flag[u] = 1;
        if (node_type[u] == 0)
            rgba[u] = LightBlue;
        if (node_type[u] == 1)
            rgba[u] = LightGreen;
        if (node_type[u] >= 2)
            rgba[u] = LightYellow;
        size[u] = node_size;
        if (!flag[u]) {
            size[u] /= 2;
        }
    }

    EdgeFlag e_flag(g);
    EdgeRGBA e_rgba(g);
    EdgeWidth e_width(g);
    EdgeLabel e_label(g);
    EdgeRGBA e_font_color(g);

    for (EdgeIt e(g); e != INVALID; ++e) {
        if (a_flag[g.direct(e, true)] or a_flag[g.direct(e, false)]) {
            e_flag[e] = 1;
        } else {
            e_flag[e] = 1;
        }

        e_label[e] = to_string(edge_cost[e]);
        e_font_color[e] = Black;
        if (edge_type[e] == 0) {
            e_rgba[e] = Blue;
            e_width[e] = 1;
        }
        if (edge_type[e] == 1) {
            e_rgba[e] = LightGreen;
            e_width[e] = 3;
        }
        if (edge_type[e] == -1) {
            e_rgba[e] = LightRed;
            e_width[e] = 3;
        }
        e_rgba[e].a = 0.5;
    }

    ArcRGBA a_rgba(g);
    ArcWidth a_width(g);

    for (ArcIt a(g); a != INVALID; ++a) {
        a_width[a] = 5;
        if (edge_type[a] != 0) {
            a_rgba[a] = e_rgba[a];
        } else {
            a_rgba[a] = Blue;
        }
        a_rgba[a].a = 0.8;
    }

    draw_network_arcs(g, a_flag, pos, a_width, a_rgba, cr);
    draw_network_edges(g, e_flag, pos, e_width, e_rgba, cr);
    // draw_network_edge_labels(g, e_flag, pos, e_label, e_font_color, cr);
    NodeFlag u_flag(g, true);
    draw_network_nodes(g, u_flag, pos, rgba, size, Blue, cr);
    draw_network_node_labels(g, flag, pos, size, Black, cr);

    string filename_s = figure_name + "_solution.png";
    const char *filename_c = filename_s.data();
    cairo_surface_write_to_png(surface, filename_c);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

#define ZTE_TOY_DRAW_H

#endif //ZTE_TOY_DRAW_H
