#include "basicblock.h"
#include "function.h"
#include <algorithm>

void BasicBlock::insert(Instruction *i)
{
    if (begin_inst == nullptr){
        begin_inst = i;
    }
    end_inst = i;
    parent->insertInst(i);
}

// insert the instruction to the front of the basicblock.
void BasicBlock::insertFront(Instruction *inst)
{
    insertBefore(inst, begin_inst);
}

// insert the instruction to the back of the basicblock.
void BasicBlock::insertBack(Instruction *inst) 
{
    insertBefore(inst, end_inst);
}

// insert the instruction dst before src.
void BasicBlock::insertBefore(Instruction *dst, Instruction *src)
{
    if (src == nullptr)
    {
        parent->insertInst(dst);
        begin_inst = end_inst = dst;
    }
    else
    {
        parent->insertBefore(dst, src);
        if (src == begin_inst)
            begin_inst = dst;
    }
    dst->setParent(this);
}

// insert the instruction dst after src.
void BasicBlock::insertAfter(Instruction *dst, Instruction *src)
{
    if (src == nullptr)
    {
        parent->insertInst(dst);
        begin_inst = end_inst = dst;
    }
    else
    {
        parent->insertAfter(dst, src);
        if (src == end_inst)
            end_inst = dst;
    }
    dst->setParent(this);
}

// insert Phi instruction.
void BasicBlock::insertPhi(Operand *o, int num_src)
{
    Instruction *inst = new PhiInstruction(o, num_src);
    insertBefore(inst, begin_inst);
}

// erase intructions in the range of [start, end] in this basicblock.
void BasicBlock::erase(Instruction *start, Instruction *end)
{
    if (start == begin_inst && end == end_inst)
        begin_inst = end_inst = nullptr;
    else if (start == begin_inst)
        begin_inst = end->getNext();
    else if (end == end_inst)
        end_inst = start->getPrev();
    parent->erase(start, end);
}

// erase the intruction in this basicblock.
void BasicBlock::erase(Instruction *inst)
{
    remove(inst);
    delete inst;
}

// erase this basicblock from its parent.
void BasicBlock::eraseFromParent()
{
    if (!empty())
        erase(begin_inst, end_inst);
    for (auto &bb : succ)
        bb->erasePred(this);
    for (auto &bb : pred)
        bb->eraseSucc(this);
    parent->erase(this);
}

// remove the instruction from intruction list but not delete it.
void BasicBlock::remove(Instruction *inst)
{
    if (inst == begin_inst && inst == end_inst)
        begin_inst = end_inst = nullptr;
    else if (inst == begin_inst){
        begin_inst = inst->getNext();
    }
    else if (inst == end_inst)
        end_inst = inst->getPrev();
    parent->remove(inst);
}

// remove intructions in the range of [start, end] in this basicblock.
void BasicBlock::remove(Instruction *start, Instruction *end)
{
    if (start == begin_inst && end == end_inst)
        begin_inst = end_inst = nullptr;
    else if (start == begin_inst)
        begin_inst = end->getNext();
    else if (end == end_inst)
        end_inst = start->getPrev();
    parent->remove(start, end);
}

void BasicBlock::setInsts(Instruction *begin, Instruction *end)
{
    begin_inst = begin;
    end_inst = end;
}

void BasicBlock::print()
{
    if (empty())
        return;
    printf("%d:", no);
    if (!pred.empty() && !parent->isEntry(pred[0]))
    {
        printf("                                                ; preds = %%%d", pred[0]->getNo());
        for (auto i = pred.begin() + 1; i != pred.end(); i++)
            printf(", %%%d", (*i)->getNo());
    }
    printf("\n");
    for (auto i = begin_inst; i != end_inst->getNext(); i = i->getNext())
        i->print();
}

// return whether this basicblock is dominated by bb.
bool BasicBlock::isDominatedBy(BasicBlock *bb) const
{
    return dom.find(bb) != dom.end();
}

// return whether this basicblock is predecessor of bb.
bool BasicBlock::isPredOf(BasicBlock *bb) const
{
    if (std::find(succ.begin(), succ.end(), bb) == succ.end())
        return false;
    return true;
}

void BasicBlock::addSucc(BasicBlock *bb)
{
    succ.push_back(bb);
}

// erase the successor basicclock bb.
void BasicBlock::eraseSucc(BasicBlock *bb)
{
    succ.erase(std::find(succ.begin(), succ.end(), bb));
}

// erase all successor basicblocks.
void BasicBlock::eraseAllSucc()
{
    for (auto bb : succ)
        bb->erasePred(this);
    succ.clear();
}

void BasicBlock::addPred(BasicBlock *bb)
{
    pred.push_back(bb);
}

// erase the predecessor basicblock bb.
void BasicBlock::erasePred(BasicBlock *bb)
{
    pred.erase(std::find(pred.begin(), pred.end(), bb));
    if (!empty())
    {
        for (auto inst = begin_inst; inst != end_inst->getNext() && inst->isPhi(); inst = inst->getNext())
            dynamic_cast<PhiInstruction *>(inst)->removeSrc(bb);
    }
}

void BasicBlock::setSucc(BasicBlock *bb)
{
    succ.clear();
    succ.push_back(bb);
}

void BasicBlock::setPred(BasicBlock *bb)
{
    pred.clear();
    pred.push_back(bb);
}

void BasicBlock::setSucc(bb_iterator begin, bb_iterator end)
{
    succ.clear();
    succ.insert(succ.begin(), begin, end);
}

void BasicBlock::setPred(bb_iterator begin, bb_iterator end)
{
    pred.clear();
    pred.insert(pred.begin(), begin, end);
}

void BasicBlock::reLabel()
{
    if (empty())
        return;
    no = parent->getLabel(no);
    for (auto i = begin_inst; i != end_inst->getNext(); i = i->getNext()){
        i->reLabel();
    }
}

/**
 * Split the basicblock.
 * The instructions before parameter it, i.e. [:it) are in the old basicblock.
 * Instructions after it are in the new basicblock.
 * Return the new instance of the basicblock.
 * */
BasicBlock *BasicBlock::split(Instruction *it)
{
    if (it == nullptr || it->getParent() != this)
    {
        BasicBlock *bb = nullptr;
        if (parent->getInsertPoint() == this)
        {
            bb = new BasicBlock(parent, symbol_table::gen_label(1));
            parent->setInsertPoint(bb);
        }
        return bb;
    }
    BasicBlock *bb = new BasicBlock(parent, symbol_table::gen_label(1));
    auto i = it;
    while (i != end_inst->getNext())
    {
        i->setParent(bb);
        i = i->getNext();
    }
    bb->setInsts(it, end_inst);
    end_inst = it->getPrev();
    if (parent->getInsertPoint() == this)
        parent->setInsertPoint(bb);
    return bb;
}

//merge this basicblock with bb, erase basicblock bb and return this.
BasicBlock *BasicBlock::merge(BasicBlock *bb)
{
    succ = bb->succ;
    if (!bb->empty())
    {
        Instruction *start, *end;
        start = bb->begin_inst;
        end = bb->end_inst;
        bb->remove(start, end);
        insertAfter(start, end_inst);
        end_inst = end;
        Instruction *inst = start;
        while (inst != end->getNext())
        {
            inst->setParent(this);
            inst = inst->getNext();
        }
    }
    for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        (*succ)->addPred(this);
    bb->eraseFromParent();
    return this;
}

int BasicBlock::getNumOfInst(){
    int ret = 0;
    for(auto ins = this->begin(); ins != this->end()->getNext(); ins = ins->getNext()){
        ret += 1;
    }
    return ret;
}

BasicBlock::BasicBlock(Function *f, int no)
{
    this->no = no;
    f->insertBlock(this);
    begin_inst = end_inst = nullptr;
    idom = nullptr;
    parent = f;
}
