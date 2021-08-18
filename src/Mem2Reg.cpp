#include "Mem2Reg.h"
#include <set>
#include <vector>
#include "basicblock.h"
#include "unit.h"
#include "instruction.h"

Mem2Reg::Mem2Reg(Unit *u) : Pass(u)
{
}

void Mem2Reg::pass()
{
    for (auto func = unit->begin(); func != unit->end(); func++)
    {
        (*func)->compute_dom_frontier();
        getPromotableName(*func);
        insertPhi();
        rename(*func);
    }
}

void Mem2Reg::insertPhi()
{
    std::set<BasicBlock *> flag, defs_bb;
    std::vector<BasicBlock *> wl;
    for (auto &alloca : allocas)
    {
        flag.clear();
        wl.clear();
        defs_bb.clear();
        alloca->removeFromParent();
        Operand *oprd = alloca->getDef();
        oprd->removeDef(alloca);
        auto defs = oprd->getDef();
        auto uses = oprd->getUse();
        Operand *temp;
        for (auto &def : defs)
        {
            auto src = def->getUse().at(0);
            Instruction *copy_inst;
            if (src->isPtr())
            {
                temp = unit->getTemp(symbol_table::gen_temp(typesys.ptr(typesys.int_type)));
                copy_inst = new AddrInstruction(temp, src, unit->getConstant(0));
            }
            else
                copy_inst = new CopyInstruction(oprd, src);
            def->insertBefore(copy_inst);
            def->removeFromParent();
            oprd->removeDef(def);
            src->removeUse(def);
            delete def;
        }
        for (auto &use : uses)
        {
            auto dst = use->getDef();
            if (oprd->isPtr())
            {
                Instruction *copy_inst;
                copy_inst = new AddrInstruction(dst, temp, unit->getConstant(0));
                use->insertBefore(copy_inst);
            }
            else
            {
                for(auto &dst_use:dst->getUse())
                    dst_use->replaceUse(dst, oprd);
            }
            use->removeFromParent();
            oprd->removeUse(use);
            dst->removeDef(use);
            delete use;
        }
        for (auto &i : oprd->getDef())
            defs_bb.insert(i->getParent());
        wl.insert(wl.begin(), defs_bb.begin(), defs_bb.end());
        while (!wl.empty())
        {
            BasicBlock *t = wl.back();
            wl.pop_back();
            for (auto bb = t->getDf().begin(); bb != t->getDf().end(); bb++)
            {
                if (flag.find(*bb) == flag.end())
                {
                    (*bb)->insertPhi(oprd, (*bb)->getNumOfPred());
                    flag.insert(*bb);
                    if (defs_bb.find(*bb) == defs_bb.end())
                        wl.push_back(*bb);
                }
            }
        }
    }
}

void Mem2Reg::rename(Function *func)
{
    reaching_def.clear();
    for (auto &v : allocas)
    {
        Operand *oprd = v->getDef();
        reaching_def[oprd] = std::vector<Operand *>();
        delete v;
    }
    rename(func->getEntry());
}

void Mem2Reg::rename(BasicBlock *bb)
{
    std::map<Operand *, int> count;
    for (auto op : reaching_def)
        count[op.first] = 0;
    if (!bb->empty())
        for (Instruction *inst = bb->begin(); inst != bb->end()->getNext(); inst = inst->getNext())
        {
            Operand *oldoprd = inst->getDef();
            if (oldoprd != nullptr && reaching_def.find(oldoprd) != reaching_def.end())
            {
                count[oldoprd]++;
                Operand *newoprd = getNewName(oldoprd);
                inst->replaceDef(newoprd);
            }
            if (!inst->isPhi())
            {
                for (auto &use : inst->getUse())
                {
                    if (reaching_def.find(use) == reaching_def.end())
                        continue;
                    Operand *reach = reaching_def[use].back();
                    inst->replaceUse(use, reach);
                }
            }
        }
    for (auto sb = bb->succ_begin(); sb != bb->succ_end(); sb++)
    {
        if (!(*sb)->empty())
            for (auto i = (*sb)->begin(); i != (*sb)->end()->getNext() && i->isPhi(); i = i->getNext())
            {
                PhiInstruction *phi_inst = dynamic_cast<PhiInstruction *>(i);
                Operand *v = phi_inst->getRename();
                Operand *src;
                if (!reaching_def[v].empty())
                    src = reaching_def[v].back();
                else
                    src = unit->getConstant(0);
                phi_inst->addSrc(src, bb);
            }
    }
    for (auto child : bb->getDomTree())
        rename(child);
    for (auto cnt : count)
        for (int i = 0; i < cnt.second; i++)
            reaching_def[cnt.first].pop_back();
}

Operand *Mem2Reg::getNewName(Operand *old)
{
    Operand *newop = old->copy();
    reaching_def[old].push_back(newop);
    return newop;
}

void Mem2Reg::getPromotableName(Function *func)
{
    allocas.clear();
    BasicBlock *bb = *func->getEntry()->succ_begin();
    if (bb->empty())
        return;
    for (auto inst = bb->begin(); inst != bb->end()->getNext(); inst = inst->getNext())
    {
        if (!inst->isAlloca())
            break;
        AllocaInstruction *alloca = dynamic_cast<AllocaInstruction *>(inst);
        if (alloca->isPromotable())
            allocas.push_back(alloca);
    }
}
