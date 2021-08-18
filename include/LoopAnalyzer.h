#ifndef _LOOP_ANALYZER_H__
#define _LOOP_ANALYZER_H__

#include "pass.h"
#include <map>
#include <set>
#include <vector>
#include <cmath>

class Function;
class BasicBlock;
class Operand;

class Loop{
private:
    std::set<BasicBlock*> bbs;
    int loopdepth;
    bool isinnerloop;
public:
    int getLoopDepth() {return this->loopdepth;};

public:
    Loop() {bbs.clear();};
    Loop(std::set<BasicBlock*>);
    std::set<BasicBlock*>& getbbs() {return bbs;};
    void getinfo();
    void setLoopDepth(int ld) {this->loopdepth = ld;};
    bool isInnerLoop() {return isinnerloop;};
    void setIsInnerLoop() {this->isinnerloop = true;};
    void clearIsInnerLoop() {this->isinnerloop = false;};
};

class loopptr{
private:
    Loop* ori_loop;
    std::pair<BasicBlock*, BasicBlock*> condandbody;
public:
    loopptr() {condandbody.first = condandbody.second = nullptr; };
    loopptr(Loop* ori_loop) {this->ori_loop = ori_loop; condandbody.first = condandbody.second = nullptr;};
    BasicBlock* getCond() { return condandbody.first; };
    BasicBlock* getBody() { return condandbody.second;};
    void setCond(BasicBlock* bb) { condandbody.first = bb; };
    void setBody(BasicBlock* bb) { condandbody.second = bb; };
};

class LoopAnalyzer
{
private:
    enum{NONE, BACKWARD, FORWARD, TREE, CROSS};
    int i, j;
    std::map<std::pair<BasicBlock*, BasicBlock*>, unsigned> edgeType;
    std::map<BasicBlock*, int> preOrder;
    std::map<BasicBlock*, int> PostOrder;
    std::map<BasicBlock*, int> loopDepth;
    std::set<BasicBlock*> visit;
    void dfs(BasicBlock*);
    void computeLoopDepth();
    std::set<BasicBlock*> computeNaturalLoop(BasicBlock*, BasicBlock*);
    std::set<Loop*> Loops;

    bool isSubset(std::set<BasicBlock*> t_son, std::set<BasicBlock*> t_fat);
    
public:
    void pass(Function *);
    void FindLoops(Function *);
    int getLoopDepth(BasicBlock*);
    std::set<Loop*>& getLoops() {return this->Loops;};
};

#endif