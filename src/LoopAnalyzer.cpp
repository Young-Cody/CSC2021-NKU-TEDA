#include "LoopAnalyzer.h"
#include "unit.h"

Loop::Loop(std::set<BasicBlock*> ori_bbs){
    bbs.clear();
    for(auto ori_bb : ori_bbs){
        bbs.insert(ori_bb);
    }
}

void Loop::getinfo(){
    // Just for debug
    cerr << "bblist: [";
    for(auto bb : this->getbbs()){
        cerr << bb->getNo() << " ";
    }
    cerr << "], isinner: " << this->isInnerLoop();
    cerr << ", loopdepth: " << this->getLoopDepth();
    cerr << endl;
}

void LoopAnalyzer::pass(Function *func){
    edgeType.clear();
    loopDepth.clear();
    visit.clear();
    Loops.clear();
    for (auto &bb : func->getBlockList())
        loopDepth[bb] = 0;
    i = j = 0;
    dfs(func->getEntry());
    computeLoopDepth();
}

int LoopAnalyzer::getLoopDepth(BasicBlock *bb){
    return loopDepth[bb];
}

void LoopAnalyzer::dfs(BasicBlock *bb){
    visit.insert(bb);
    preOrder[bb] = i;
    i++;
    for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++){
        if (visit.find(*succ) == visit.end()){
            edgeType[{bb, *succ}] = TREE;
            dfs(*succ);
        }
        else if (preOrder[bb] < preOrder[*succ])
            edgeType[{bb, *succ}] = FORWARD;
        else if (PostOrder.find(*succ) == PostOrder.end())
            edgeType[{bb, *succ}] = BACKWARD;
        else
            edgeType[{bb, *succ}] = CROSS;
    }
    PostOrder[bb] = j;
    j = j + 1;
}

void LoopAnalyzer::computeLoopDepth()
{
    for (auto edge : edgeType){
        if (edge.second == BACKWARD){
            auto loop = new Loop(computeNaturalLoop(edge.first.first, edge.first.second));
            Loops.insert(loop);
            for (auto &bb : loop->getbbs())
                loopDepth[bb]++;
        }
    }
}

std::set<BasicBlock *> LoopAnalyzer::computeNaturalLoop(BasicBlock *u, BasicBlock *v){
    std::set<BasicBlock *> loop;
    std::vector<BasicBlock *> s;
    if (u == v)
        loop.insert(u);
    else{
        loop.insert(u);
        loop.insert(v);
        s.push_back(u);
    }
    while (!s.empty()){
        auto t = s.back();
        s.pop_back();
        for (auto pred = t->pred_begin(); pred != t->pred_end(); pred++){
            if (loop.find(*pred) == loop.end()){
                s.push_back(*pred);
                loop.insert(*pred);
            }
        }
    }
    return loop;
}

bool LoopAnalyzer::isSubset(std::set<BasicBlock*> t_son, std::set<BasicBlock*> t_fat){
    /*
    *  [ATTENTION] Here regulate that set i is not father of itself.
    */

    for(auto EleInt_son : t_son){
        if(t_fat.find(EleInt_son) == t_fat.end()){
            return false;
        }
    }
    return t_son.size() != t_fat.size();
}

void LoopAnalyzer::FindLoops(Function* func){
    // [Step 0]: this pass will calc loopdepth of each basicblock, also put loop into loops,
    //           after that, you could access loops by this->getLoops();
    this->pass(func);

    // [Step 1]: calc loop depth
    for(auto loop : this->getLoops()){
        loop->setLoopDepth(0x7fffffff);
        for(auto bb : loop->getbbs()){
            loop->setLoopDepth(min(loop->getLoopDepth(), this->getLoopDepth(bb)));
        }
    }

    /* [Step 2]: calc innerloop ( if i is innerloop i.e. i is not anyone's fath)
    *  Step 2.1: Mark all the loop isinnerloop;
    *  Step 2.2: Clear mark if any loop i is father of any loop j;
    */

    for(auto loop : this->getLoops()){
        loop->setIsInnerLoop();
    }

    for(auto loopi : this->getLoops()){
        for(auto loopj : this->getLoops()){
            if(isSubset(loopi->getbbs(), loopj->getbbs())){
                loopj->clearIsInnerLoop();
            }
        }
    }
}