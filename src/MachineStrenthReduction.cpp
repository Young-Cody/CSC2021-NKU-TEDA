#include "MachineStrenthReduction.h"
void strength_reduction::pass() 
{
    for(auto &f : unit->get_func_list())
    {
        for(auto &bb : f->get_block_list())
        {
            auto inst_list = bb->get_inst_list(); 
            vector<m_instruction*> del_list;
            list<m_instruction*> copy_list = inst_list;
            for(auto iter = inst_list.begin(); iter != inst_list.end(); iter++)
            {
                m_instruction* inst = *iter;
                if(inst->getType() == m_instruction::BINARY)
                {
                    if(inst->getOP() == binary_mi::ADD || inst->getOP() == binary_mi::SUB)
                    {
                        if(inst->get_imm_ope() && inst->get_imm_ope()->getval() == 0)
                        {
                            if(inst->getDef()[0]->get_reg() == inst->getUse()[0]->get_reg())
                            {
                                del_list.push_back(inst);
                            }
                            else
                            {
                                mov_mi* i = new mov_mi(mov_mi::MOV, inst->getDef()[0], inst->getUse()[0]);
                                bb->insert_before((m_instruction*) i, inst);
                                del_list.push_back(inst);
                            }
                        }
                    }
                    if(inst->getOP() == binary_mi::MUL || inst->getOP() == binary_mi::DIV)
                    {
                        if(inst->get_imm_ope() && inst->get_imm_ope()->getval() == 1)
                        {
                            if(inst->getDef()[0]->get_reg() == inst->getUse()[0]->get_reg())
                            {
                                del_list.push_back(inst);
                            }
                            else
                            {
                                mov_mi* i = new mov_mi(mov_mi::MOV, inst->getDef()[0], inst->getUse()[0]);
                                bb->insert_before((m_instruction*) i, inst);
                                del_list.push_back(inst);
                            }
                        }
                    }
                }
            }
        
            for(auto inst: del_list)
            {
                bb->remove(inst);
            }
        }
    }
}

strength_reduction::strength_reduction(m_unit* u) 
{
    this->unit = u;
}
