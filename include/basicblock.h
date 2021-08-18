#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__
#include <vector>
#include <set>
#include "instruction.h"

class Function;

class BasicBlock
{
    typedef std::vector<BasicBlock *>::iterator bb_iterator;
    typedef std::set<BasicBlock *> dominators;
    typedef BasicBlock *idominator;
    typedef std::set<BasicBlock *> dom_frontier;
    typedef std::set<BasicBlock *> reverse_dominators;
    typedef BasicBlock *reverse_idominator;
    typedef std::set<BasicBlock *> reverse_dom_frontier;

private:
    std::vector<BasicBlock *> pred, succ;
    std::vector<BasicBlock *> dom_tree;
    Instruction *begin_inst, *end_inst;
    Function *parent;
    dominators dom;
    idominator idom;
    dom_frontier df;
    reverse_dom_frontier rdf;
    int no;
    bool marked; // for useless code elimination, if an instruction in this bb marked, then the bb marked

public:
    BasicBlock(Function *, int);
    void insert(Instruction *);
    void insertFront(Instruction *);
    void insertBack(Instruction *);
    void insertBefore(Instruction *, Instruction *);
    void insertAfter(Instruction *, Instruction *);
    void insertPhi(Operand *, int);
    void erase(Instruction *, Instruction *);
    void erase(Instruction *);
    void eraseFromParent();
    void remove(Instruction *);
    void remove(Instruction *, Instruction *);
    void setInsts(Instruction *, Instruction *);
    bool empty() const { return begin_inst == nullptr; }
    void print();
    bool isDominatedBy(BasicBlock *) const;
    bool isPredOf(BasicBlock *) const;
    std::vector<BasicBlock *> &getDomTree() { return dom_tree; };
    bool succEmpty() const { return succ.empty(); };
    bool predEmpty() const { return pred.empty(); };
    void addSucc(BasicBlock *);
    void eraseSucc(BasicBlock *);
    void eraseAllSucc();
    void addPred(BasicBlock *);
    void erasePred(BasicBlock *);
    void setSucc(BasicBlock *);
    void setPred(BasicBlock *);
    void setSucc(bb_iterator, bb_iterator);
    void setPred(bb_iterator, bb_iterator);
    int getNo() { return no; };
    void reLabel();
    bool isMarked() { return marked; };
    void setMarked() { marked = true; };
    void clearMarked() { marked = false; };
    Function *getParent() { return parent; };
    dominators &getDom() { return dom; };
    idominator &getIdom() { return idom; };
    dom_frontier &getDf() { return df; };
    // before calling this, u shall call compute_reverse_dom
    reverse_dominators &getRDom() { return dom; };
    // before calling this, u shall call compute_reverse_idom
    reverse_idominator &getRIdom() { return idom; };
    reverse_dom_frontier &getRDF() { return rdf; };
    Instruction *begin() { return begin_inst; };
    Instruction *end() { return end_inst; };
    Instruction *rbegin() { return end_inst; };
    Instruction *rend() { return begin_inst; };
    bb_iterator succ_begin() { return succ.begin(); };
    bb_iterator succ_end() { return succ.end(); };
    bb_iterator pred_begin() { return pred.begin(); };
    bb_iterator pred_end() { return pred.end(); };
    int getNumOfPred() const { return pred.size(); };
    int getNumOfSucc() const { return succ.size(); };
    int getNumOfInst();
    BasicBlock *split(Instruction *);
    BasicBlock *merge(BasicBlock *);
};

#endif