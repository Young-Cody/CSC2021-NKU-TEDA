#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include "basicblock.h"
#include "symtab.h"

class Unit;

class Function
{
    typedef std::vector<BasicBlock *>::iterator iterator;
    typedef std::vector<BasicBlock *>::reverse_iterator reverse_iterator;

private:
    std::vector<BasicBlock *> block_list;
    symbol_entry *sym_ptr;
    BasicBlock *entry;
    BasicBlock *exit;
    BasicBlock *insert_point;
    Instruction *front, *rear;
    std::map<int, int> label_map;
    std::map<Function*, std::set<Instruction*>> preds, succs;
    int new_label;
    bool critical;

public:
    Function(Unit *, symbol_entry *);
    ~Function();
    void insertInst(Instruction *inst);
    void insertBefore(Instruction *, Instruction *);
    void insertAfter(Instruction *, Instruction *);
    void erase(Instruction *, Instruction *);
    void erase(Instruction *);
    void erase(BasicBlock *);
    void remove(Instruction *, Instruction *);
    void remove(Instruction *);
    void insertBlock(BasicBlock *bb) { block_list.push_back(bb); };
    void setInsertPoint(BasicBlock *bb) { insert_point = bb; };
    void constructCFG();
    void reLabel();
    int getLabel(int);
    BasicBlock *getInsertPoint() { return insert_point; };
    BasicBlock *getEntry() { return entry; };
    BasicBlock *getExit() { return exit; };
    bool isEntry(BasicBlock *bb) { return bb == entry; };
    bool isExit(BasicBlock *bb) { return bb == exit; };
    void compute_dom();
    void compute_idom();
    void compute_dom_frontier();
    void compute_reverse_dom();
    void compute_reverse_idom();
    void compute_reverse_dom_frontier();
    BasicBlock *compute_nearest_marked_postdominator(Instruction *ins);
    std::set<BasicBlock *> compute_natural_loop(BasicBlock *ui, BasicBlock *vi);
    void print();
    Instruction *instBegin() { return front->getNext(); };
    Instruction *instEnd() { return front; };
    std::vector<BasicBlock *> &getBlockList(){return block_list;};
    iterator begin() { return block_list.begin(); };
    iterator end() { return block_list.end(); };
    reverse_iterator rbegin() { return block_list.rbegin(); };
    reverse_iterator rend() { return block_list.rend(); };
    symbol_entry *getSymPtr() { return sym_ptr; };
    bool isCritical() const {return critical;};
    void setCritical() {critical = true;};
    std::map<Function*, std::set<Instruction*>>& getSuccs() {return succs;}
    std::map<Function*, std::set<Instruction*>>& getPreds() {return preds;}
};

#endif
