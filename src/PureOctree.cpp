
#include "PureOctree.h"

PureOctree::PureOctree(ui32 default_value, ui32 max_depth) {
    data.emplace_back(setLeaf(default_value));
    this->max_depth=max_depth<=DEPTH_LIMIT?max_depth:DEPTH_LIMIT;
    position_stack=new ui8[max_depth+1];
    index_stack=new ui32[max_depth];
    index_stack[0] = 0;
}

void PureOctree::optimize() {

}

void PureOctree::set(ui32 material, ui32 x, ui32 y, ui32 z) {

    index_stack[0] = 0;
    int depth;
    for (depth = 1; depth <= max_depth; ++depth) {
        int radius = getDimentionAt(max_depth - depth);
        bool x_greater = x >= radius;
        bool y_greater = y >= radius;
        bool z_greater = z >= radius;
        position_stack[depth] = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
        if (depth == max_depth) {
            break;
        }
        x -= radius * x_greater;
        y -= radius * y_greater;
        z -= radius * z_greater;
        //if current node is leaf and depth is not the target
        if (depth != max_depth && isLeaf(data[index_stack[depth - 1]].children[position_stack[depth]])) {
            //return if parent is already correct material
            if (getValue(data[index_stack[depth - 1]].children[position_stack[depth]]) == material) {
                return;
            }
            data.emplace_back(data[index_stack[depth - 1]].children[position_stack[depth]]);
            data[index_stack[depth - 1]].children[position_stack[depth]] = data.size() - 1;
            last_node_position=position_stack[depth];
        }
        index_stack[depth] = data[index_stack[depth - 1]].children[position_stack[depth]];
    }
    data[index_stack[depth - 1]].children[position_stack[depth]] = setLeaf(material);
}

std::vector<PureOctree::Node> &PureOctree::getData() {
    return data;
}
void PureOctree::remove(ui32 depth) {

}
void PureOctree::clearAndReplace(ui32 material, ui32 depth) {
    //if is not a leaf remove node
    if(!isLeaf(index_stack[depth]))
    {
        for (int i = 0; i < 8; ++i) {
            position_stack[depth+1]=i;
            remove(depth+1);
        }
        Node &n = data[data.size() - 1];
        data[index_stack[depth]] = data[data.size() - 1];
        data[index_stack[depth-1]].children[last_node_position] = index_stack[depth];
        data.pop_back();
        for (int j = 0; j < 8; ++j) {
            if (!isLeaf(n.children[j])) {
                position_stack[depth + 1] = j;
                clearAndReplace(material, depth + 1);
            }
        }
    }
    //set parent pointer to this leaf
    data[index_stack[depth - 1]].children[position_stack[depth]] = setLeaf(material);

}

ui32 PureOctree::getDimentionAt(ui32 depth) {
    return 1 << depth;
}

bool PureOctree::isLeaf(ui32 node) {
    return node & LEAF_FLAG;
}

ui32 PureOctree::getValue(ui32 node) {
    return node & VALUE_MASK;
}

ui32 PureOctree::setLeaf(ui32 node) {
    return node ^ LEAF_FLAG;
}

ui32 PureOctree::setTree(ui32 node) {
    return node & VALUE_MASK;
}

bool PureOctree::combine(ui32 depth) {
    if (depth == 0) return false;
    if (depth == max_depth)return true;
    ui32 material;
    for (int i = 0; i < 8; ++i) {

        index_stack[depth + 1] = data[index_stack[depth]].children[i];
        //combine child if not a leaf if cant be combined then exit
        if (!isLeaf(index_stack[depth + 1]) && !combine(depth + 1)) {
            return false;
        }
        if (i == 0) {
            material = getValue(index_stack[depth + 1]);
        } else if (material != getValue(index_stack[depth + 1])) {
            return false;
        }
    }
    clearAndReplace(material, depth);
    return true;
}

PureOctree::~PureOctree() {
    delete[] index_stack;
    delete[] position_stack;
}

ui32 PureOctree::getMaxDepth() {
    return max_depth;
}

ui32 PureOctree::getDimention() {
    return getDimentionAt(max_depth);
}






