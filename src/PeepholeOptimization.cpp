#include "PeepholeOptimization.h"

peephole_optimization::peephole_optimization(m_unit* u) 
{
    this->unit = u;
}

void peephole_optimization::pass() 
{
    for(auto &f : unit->get_func_list())
    {
        for(auto &bb : f->get_block_list())
        {
            auto inst_list = bb->get_inst_list(); 
            vector<m_instruction*> del_list;
            for(auto iter = inst_list.begin(), next_iter = next(iter, 1); next_iter != inst_list.end(); iter++, next_iter++)
            {
                auto inst = *iter;
                auto next_inst = *next_iter;
                // 1. fuse mul and add/sub
                // pattern:
                // mul v0, v1, v2
                // add v3, v4, v0
                // -----
                // mla v3, v1, v2, v4

                if(inst->getType() == m_instruction::BINARY 
                && inst->getOP() == binary_mi::MUL 
                && next_inst->getType() == m_instruction::BINARY
                && next_inst->get_imm_ope() == nullptr
                && next_inst->getUse()[0]->get_shift() == nullptr)
                {
                    if(next_inst->getOP() == binary_mi::ADD)
                    {
                        if(next_inst->getUse().size() > 1 
                        && next_inst->getUse()[1]->get_reg() == inst->getDef()[0]->get_reg())
                        {
                            fused_binary* mla_inst = new fused_binary(fused_binary::MLA, next_inst->getDef()[0], inst->getUse()[0], inst->getUse()[1], next_inst->getUse()[0]);
                            bb->insert_before((m_instruction*)mla_inst, inst);
                            del_list.push_back(inst);
                            del_list.push_back(next_inst);
                        }
                        else if(next_inst->getUse()[0]->get_reg() == inst->getDef()[0]->get_reg())
                        {
                            fused_binary* mla_inst = new fused_binary(fused_binary::MLA, next_inst->getDef()[0], inst->getUse()[0], inst->getUse()[1], next_inst->getUse()[1]);
                            bb->insert_before((m_instruction*)mla_inst, inst);
                            del_list.push_back(inst);
                            del_list.push_back(next_inst);
                        }
                    }
                    if(next_inst->getOP() == binary_mi::SUB
                    && (next_inst->getUse().size() > 1 
                    && next_inst->getUse()[1]->get_reg() == inst->getDef()[0]->get_reg()))
                    {
                        fused_binary* mls_inst = new fused_binary(fused_binary::MLS, next_inst->getDef()[0], inst->getUse()[0], inst->getUse()[1], next_inst->getUse()[0]);
                        bb->insert_before((m_instruction*)mls_inst, inst);
                        del_list.push_back(inst);
                        del_list.push_back(next_inst);
                    }
                }

                // 2. fuse cmp and beq/bne
                // pattern:
                // cmp v0, #0
                // beq .L1
                // b .L2
                // -----
                // cbz v0, .L1
                // b .L2

                // Are you depressed????
                // Are you mad????
                // Are you upset????
                // Are you sad????
                // cbz cbnz can only be used in thumb mode
                // however IT block can't be used in thumb mode

                // if(inst->getType() == m_instruction::CMP 
                // && inst->get_imm_ope() 
                // && inst->get_imm_ope()->getval()==0)
                // {
                //     if(next_inst->getType() == m_instruction::BRANCH){
                //         if(next_inst->getOP() == branch_mi::BEQ)
                //         {
                //             std::ostringstream buf;
                //             buf << ".L" << dynamic_cast<branch_mi*>(next_inst)->get_block();
                //             string label = buf.str();
                //             m_operand* addr_label = new m_operand(m_operand::LABEL, -1, label);
                //             fused_cmp* cbz_inst = new fused_cmp(fused_cmp::CBZ, inst->getUse()[0], addr_label);
                //             bb->insert_before((m_instruction*)cbz_inst, inst);
                //             del_list.push_back(inst);
                //             del_list.push_back(next_inst);
                //         }
                //         if(next_inst->getOP() == branch_mi::BNE)
                //         {
                //             std::ostringstream buf;
                //             buf << ".L" << dynamic_cast<branch_mi*>(next_inst)->get_block();
                //             string label = buf.str();
                //             m_operand* addr_label = new m_operand(m_operand::LABEL, -1, label);
                //             fused_cmp* cbnz_inst = new fused_cmp(fused_cmp::CBNZ, inst->getUse()[0], addr_label);
                //             bb->insert_before((m_instruction*)cbnz_inst, inst);
                //             del_list.push_back(inst);
                //             del_list.push_back(next_inst);
                //         }
                //     }
                // }

                // 3. delete unecessary load imm inst
                // pattern:
                // ldr v0, #100
                // ldr v1, [v2, v0]
                // -----
                // ldr v1, [v2, #100]
                if(inst->getType() == m_instruction::ACCESS && dynamic_cast<access_mi*>(inst)->is_load_imm())
                {
                    if(next_inst->getType() == m_instruction::ACCESS)
                    {
                        if(next_inst->getOP() == access_mi::LDR)
                        {
                            if(next_inst->getUse().size()>1 && next_inst->getUse()[1]->get_reg() == inst->getDef()[0]->get_reg())
                            {
                                if(inst->get_imm_ope() && !inst->get_imm_ope()->isbigimm())
                                {
                                    m_operand* imm = new m_operand(m_operand::IMM, inst->get_imm_ope()->getval());
                                    next_inst->replace_use(next_inst->getUse()[1], imm);
                                    del_list.push_back(inst);
                                }
                            }
                        }
                        else if(next_inst->getOP() == access_mi::STR)
                        {
                            if(next_inst->getUse().size()>2 && next_inst->getUse()[1]->get_reg() == inst->getDef()[0]->get_reg())
                            {
                                if(inst->get_imm_ope() && !inst->get_imm_ope()->isbigimm())
                                {
                                    m_operand* imm = new m_operand(m_operand::IMM, inst->get_imm_ope()->getval());
                                    next_inst->replace_use(next_inst->getUse()[1], imm);
                                    del_list.push_back(inst);
                                }
                                
                            }
                        }
                    }
                }

                // 4. load inst -> mov inst
                // pattern:
                // str r0, [sp, #4]
                // ldr r1, [sp, #4]
                if(inst->getType() == m_instruction::ACCESS && inst->getOP() == access_mi::STR)
                {
                    if(next_inst->getType() == m_instruction::ACCESS && next_inst->getOP() == access_mi::LDR)
                    {
                        if(inst->getUse().size()-1 == next_inst->getUse().size())
                        {
                            int num = 0;
                            int size = next_inst->getUse().size();
                            while(num < size)
                            {
                                if(!(*inst->getUse()[num] == *next_inst->getUse()[num]))
                                {
                                    num = -1;
                                    break;
                                }
                                num++;
                            }
                            if(inst->get_imm_ope() && next_inst->get_imm_ope() &&
                            !(*inst->get_imm_ope() == *next_inst->get_imm_ope()))
                            {
                                continue;
                            }
                            if(num != -1)
                            {
                                mov_mi* mov_inst = new mov_mi(mov_mi::MOV, next_inst->getDef()[0], inst->getUse().back());
                                bb->insert_before((m_instruction*)mov_inst, next_inst);
                                del_list.push_back(next_inst);
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
