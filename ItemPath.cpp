////
//// Created by agamemnon on 17-4-29.
////
//
//#include "ItemPath.h"
//
//ItemPath::ItemPath(ListGraph &G) {
//    g = &G;
//    head = nullptr;
//    tail = nullptr;
//}
//
//void ItemPath::pushBackArc(ListGraph::Arc a) {
//    if (g->valid(a)) {
//        if (head == nullptr) {
//            head = new(Item);
//            head->id = g->id(a);
//            head->isArc = true;
//            tail = head;
//        } else {
//            tail->next = new(Item);
//            tail->next->id = g->id(a);
//            tail->next->isArc = true;
//            tail->next->prev = tail;
//            tail = tail->next;
//        }
//
//    } else {
//        cout << "invalid edge" << endl;
//    }
//}
//
//void ItemPath::pushBackNode(ListGraph::Node u) {
//    if (g->valid(u)) {
//        if (head == nullptr) {
//            head = new(Item);
//            head->id = g->id(u);
//            head->isNode = true;
//            tail = head;
//        } else {
//            tail->next = new(Item);
//            tail->next->id = g->id(u);
//            tail->next->isNode = true;
//            tail->next->prev = tail;
//            tail = tail->next;
//        }
//    }
//
//}
//
//void ItemPath::print() {
//    Item *ptr = head;
//    if (ptr == nullptr) {
//        cout << "the path is empty" << endl;
//        return;
//    }
//    while (ptr != nullptr) {
//        if (ptr->isArc) {
//            cout << "(" << g->id(g->source(g->arcFromId(ptr->id))) << "->"
//                 << g->id(g->target(g->arcFromId(ptr->id))) << ")--";
//        }
//        if (ptr->isNode) {
//            cout << ptr->id << "--";
//        }
//        ptr = ptr->next;
//    }
//    cout << endl;
//}
//
//ItemPath::~ItemPath() {
//
//}
//
//void ItemPath::clear() {
//    Item *ptr = head;
//    while (ptr != nullptr) {
//        head = ptr->next;
//        delete (ptr);
//        ptr = head;
//    }
//    head = nullptr;
//    tail = nullptr;
//
//}
//
//void ItemPath::insert(Item *ptr, ListGraph::Arc a) {
//    if (g->valid(a)) {
//        if (ptr == tail) {
//            pushBackArc(a);
//        } else {
//            Item *item = new(Item);
//            item->next = ptr->next;
//            item->id = g->id(a);
//            item->isArc = true;
//            ptr->next = item;
//            item->prev = ptr;
//        }
//    } else {
//        cout << "Invalid arc!" << endl;
//    }
//
//
//}
//
//void ItemPath::insert(Item *ptr, ListGraph::Node u) {
//    if (g->valid(u)) {
//        if (ptr == tail) {
//            pushBackNode(u);
//        } else {
//            Item *item = new(Item);
//            item->next = ptr->next;
//            item->id = g->id(u);
//            item->isNode = true;
//            ptr->next = item;
//            item->prev = ptr;
//        }
//    } else {
//        cout << "Invalid node!" << endl;
//    }
//
//}
//
//Item *ItemPath::begin() {
//    return head;
//}
//
//Item *ItemPath::end() {
//    return tail;
//}
//
//double ItemPath::pathLength(vector<vector<double>> &dist,
//                            ListGraph::EdgeMap<double> &edge_length) {
//    if (head == nullptr) {
//        return 0;
//    } else {
//        double path_length = 0;
//        Item *ptr = head;
//        while (ptr != nullptr) {
//            if (ptr->isNode) {
//                if (ptr->next != nullptr) {
//                    if (ptr->next->isNode) {
//                        path_length += dist[ptr->id][ptr->next->id];
//                    }
//                    if (ptr->next->isArc) {
//                        path_length += dist[ptr->id][g->id(
//                                g->source(g->arcFromId(ptr->next->id)))];
//                    }
//                }
//            }
//            if (ptr->isArc) {
//                path_length += edge_length[g->arcFromId(ptr->id)];
//                if (ptr->next != nullptr) {
//                    if (ptr->next->isNode) {
//                        path_length += dist[g->id(g->target(
//                                g->arcFromId(ptr->id)))][ptr->next->id];
//                    }
//                    if (ptr->next->isArc) {
//
//                        path_length += dist[g->id(
//                                g->target(g->arcFromId(ptr->id)))][g->id(
//                                g->source(g->arcFromId(ptr->next->id)))];
//                    }
//                }
//            }
//            ptr = ptr->next;
//        }
//        return path_length;
//    }
//
//}
//
//
//
//
