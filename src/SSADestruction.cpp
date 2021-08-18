#include "SSADestruction.h"
#include <set>
#include <vector>
#include "basicblock.h"
#include "unit.h"
#include "instruction.h"

void SSADestruction::pass()
{
    std::vector<Instruction*> del_list;
    for(auto func = unit->begin(); func != unit->end(); func++)
    {
        std::vector<BasicBlock *> bbs = (*func)->getBlockList();
        for(auto bb:bbs)
        {
            if(bb->empty() || !bb->begin()->isPhi())
                continue;
            std::vector<BasicBlock *> preds(bb->pred_begin(), bb->pred_end());
            // insert basicblock in the critical edge
            for(auto pred:preds)
            {
                if(pred->getNumOfSucc() < 2)
                {
                    for(auto inst = bb->begin(); inst != bb->end()->getNext() && inst->isPhi(); inst = inst->getNext())
                    {
                        Operand *dst = inst->getDef();
                        Operand *src = dynamic_cast<PhiInstruction *>(inst)->getSrc(pred);
                        Instruction *copy = new CopyInstruction(dst, src);
                        pred->insertBack(copy);
                    }
                    continue;
                }
                BasicBlock *insert_bb;
                insert_bb = new BasicBlock(*func, symbol_table::gen_label(1));
                Instruction *br = new BranchInstruction(bb);
                insert_bb->insertBack(br);
                for(auto inst = bb->begin(); inst != bb->end()->getNext() && inst->isPhi(); inst = inst->getNext())
                {
                    Operand *dst = inst->getDef();
                    Operand *src = dynamic_cast<PhiInstruction *>(inst)->getSrc(pred);
                    Instruction *copy = new CopyInstruction(dst, src);
                    insert_bb->insertBack(copy);
                }
                Instruction *inst = pred->end();
                if(inst->isUncondBr())
                    dynamic_cast<BranchInstruction*>(inst)->setBranch(insert_bb);
                else
                {
                    IfInstruction *ifinst = dynamic_cast<IfInstruction*>(inst);
                    if(ifinst->getBranch() == bb)
                        ifinst->setBranch(insert_bb);
                    if(ifinst->getFalseBranch() == bb)
                        ifinst->setFalseBranch(insert_bb);
                }
                insert_bb->addPred(pred);
                insert_bb->addSucc(bb);
                pred->eraseSucc(bb);
                pred->addSucc(insert_bb);
                bb->erasePred(pred);
                bb->addPred(insert_bb);
            }
            std::vector<Instruction*> del_list;
            for(auto inst = bb->begin(); inst != bb->end()->getNext() && inst->isPhi(); inst = inst->getNext())
                del_list.push_back(inst);
            for(auto&inst:del_list){
                inst->eraseFromParent();
            }
        }
    }
}
