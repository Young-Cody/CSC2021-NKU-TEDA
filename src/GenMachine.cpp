#include "GenMachine.h"
static void Exception(string info){ cerr << "Exception in " << info << endl;}

gen_machine::gen_machine(Unit* unit)
{
    this->unit = unit;
}

m_unit* gen_machine::convert_unit(ostream& out)
{
    m_unit* m_u = new m_unit(unit->getGlobal(), out);
    for(auto iter = unit->begin(); iter != unit->end(); iter++)
    {
        m_function* m_func = nullptr;
        convert_func(m_func, (*iter), m_u);
        m_u->insert_func(m_func);
    }
    m_u->set_vreg(vreg_num);
    return m_u;
}

void gen_machine::convert_func(m_function*& m_func, Function* func, m_unit* parent)
{
    cur_func = func;
    m_func = new m_function(func->getSymPtr(), parent);
    std::map<BasicBlock*, m_block*> m;
    for(auto bb = func->begin(); bb != func->end(); bb++)
    {
        m_block *m_bb;
        convert_block(m_bb, *bb, m_func);
        m_func->insert_blk(m_bb);
        m[*bb] = m_bb;
    }
    for(auto bb = func->begin(); bb != func->end(); bb++)
    {
        m_block *m_bb;
        m_bb = m[*bb];
        for(auto pred = (*bb)->pred_begin(); pred != (*bb)->pred_end(); pred++)
            m_bb->add_pred(m[*pred]);
        for(auto succ = (*bb)->succ_begin(); succ != (*bb)->succ_end(); succ++)
            m_bb->add_succ(m[*succ]);
    }
    m_func->set_entry(m[func->getEntry()]);
    m_func->set_exit(m[func->getExit()]);
    m_instruction *inst = gen_stack_mi(stack_mi::PUSH, std::vector<m_operand*>());
    (*(m_func->get_block_list().begin()))->insert_front(inst);
    m_func->add_reg_back_patch(inst);
    m_operand* Rm = gen_imm_operand(-1, m_func->get_entry());
    m_operand* Rd = gen_reg_operand(13, m_operand::REG);
    m_operand* Rn = gen_reg_operand(13, m_operand::REG);
    inst = gen_binary_mi(binary_mi::SUB, Rd, Rn, Rm);
    inst->set_parent(m_func->get_entry());
    m_func->get_entry()->insert_inst(inst);
    m_func->add_sp_backpatch(inst);
}

void gen_machine::convert_block(m_block*& m_bb, BasicBlock* bb, m_function* parent)
{
    m_bb = new m_block(bb->getNo(), parent);
    if(bb == nullptr)
        exit(1);
    if(bb->empty())
        return;
    for(auto iter = bb->begin(); iter != bb->end()->getNext(); iter = iter->getNext())
    {
        int kind = iter->getKind();
        if(iter->get_vec_flag())
        {
            m_operand* d0 = gen_reg_operand(0, m_operand::DREG);
            m_operand* d1 = gen_reg_operand(1, m_operand::DREG);
            m_operand* d2 = gen_reg_operand(2, m_operand::DREG);
            m_operand* d3 = gen_reg_operand(3, m_operand::DREG);
            if(kind == LOAD || kind == STORE)
            {
                vector<m_operand*> list = {d0, d1, d2, d3};
                if(kind == LOAD)
                {
                    m_operand* base = gen_operand(iter->getOperands()[1], m_bb);
                    m_bb->insert_inst(gen_vec_access(vaccess::VLDR, list, base));
                }
                else
                {
                    if(iter->getOperands()[1]->isConstant())
                    {
                        m_operand* imm = gen_operand(iter->getOperands()[1], m_bb);
                        m_bb->insert_inst(gen_vec_mov(d0, imm));
                        m_bb->insert_inst(gen_vec_mov(d1, imm));
                        m_bb->insert_inst(gen_vec_mov(d2, imm));
                        m_bb->insert_inst(gen_vec_mov(d3, imm));
                    }
                    m_operand* base = gen_operand(iter->getOperands()[0], m_bb);
                    m_bb->insert_inst(gen_vec_access(vaccess::VSTR, list, base));
                }
            }
            else if(kind == COPY)
            {
                m_operand* imm = gen_operand(iter->getUse()[0], m_bb);
                m_bb->insert_inst(gen_vec_mov(d0, imm));
                m_bb->insert_inst(gen_vec_mov(d1, imm));
                m_bb->insert_inst(gen_vec_mov(d2, imm));
                m_bb->insert_inst(gen_vec_mov(d3, imm));
            }
            continue;
        }
        if(kind == BINARY)
        {
            BinaryInstruction* inst = dynamic_cast<BinaryInstruction*>(iter);
            convert_binary_inst(m_bb, inst);
        }
        else if(kind == UNARY)
        {
            UnaryInstruction* inst = dynamic_cast<UnaryInstruction*>(iter);
            convert_unary_inst(m_bb, inst);
        }
        else if(kind == COPY)
        {
            CopyInstruction* inst = dynamic_cast<CopyInstruction*>(iter);
            convert_copy_inst(m_bb, inst);
        }
        else if(kind == RET)
        {
            RetInstruction* inst = dynamic_cast<RetInstruction*>(iter);
            convert_ret_inst(m_bb, inst);
        }
        else if(kind == CALL)
        {
            CallInstruction* inst = dynamic_cast<CallInstruction*>(iter);
            convert_call_inst(m_bb, inst);
            parent->add_saved_regs(14);
        }
        else if(kind == LOAD)
        {
            LoadInstruction* inst = dynamic_cast<LoadInstruction*>(iter);
            convert_load_inst(m_bb, inst);
        }
        else if(kind == STORE)
        {
            StoreInstruction* inst = dynamic_cast<StoreInstruction*>(iter);
            convert_store_inst(m_bb, inst);
        }
        else if(kind == ADDR)
        {
            AddrInstruction* inst = dynamic_cast<AddrInstruction*>(iter);
            convert_addr_inst(m_bb, inst);
        }
        else if(kind == ALLOCA)
        {
            AllocaInstruction* inst = dynamic_cast<AllocaInstruction*>(iter);
            convert_alloca_inst(m_bb, inst);
        }
        else if(kind == BRANCH)
        {
            BranchInstruction* inst = dynamic_cast<BranchInstruction*>(iter);
            convert_branch_inst(m_bb, inst);
        }
        else if(kind == CMP)
        {
            // CMP & IF pairs
            CmpInstruction* cmp_inst = dynamic_cast<CmpInstruction*>(iter);
            if(iter->getNext()->getKind()!=IF) 
            {
                convert_cmp_inst(m_bb, cmp_inst);
            }
            else
            {
                iter = iter->getNext();
                IfInstruction* if_inst = dynamic_cast<IfInstruction*>(iter);
                convert_cmp_if_inst(m_bb, cmp_inst, if_inst);
            }
        }
        else if(kind == IF)
        {
            Exception("gen_machine::convert_block IF instruction should not be here!");
        }
        else if(kind == PHI)
        {
            Exception("TODO");
        }
        else
        {
            Exception("gen_machine::convert_block");
        }
    
    }
    // if(bb->begin()->getKind() == ALLOCA)
    // {
    //     m_operand* Rm = gen_imm_operand(-1, m_bb);
    //     m_operand* Rd = gen_reg_operand(13, m_operand::REG);
    //     m_operand* Rn = gen_reg_operand(13, m_operand::REG);
    //     m_bb->insert_front(gen_binary_mi(binary_mi::SUB, Rd, Rn, Rm));
    //     m_bb->get_parent()->add_sp_backpatch(m_bb->get_inst_list().front());
    // }
}

void gen_machine::convert_binary_inst(m_block*& m_bb, BinaryInstruction* inst)
{
    if(inst->getDef() == nullptr)
        exit(2);
    if(inst->getUse().size() != 2)
        exit(3);
    m_operand* Rd = gen_operand(inst->getDef(), m_bb);
    m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
    m_operand* Rm = gen_operand(inst->getUse()[1], m_bb);
    // m_operand* r0_0 = nullptr, *r1_0 = nullptr;
    m_operand* div_reg0, *div_reg1, *mul_reg0, *mul_reg1;
    int op = inst->getOP();
    if(Rn->isimm() && Rm->isimm())
    {
        if(op == BinaryInstruction::ADD) Rn->setval(Rn->getval() + Rm->getval());
        else if(op == BinaryInstruction::MINUS) Rn->setval(Rn->getval() - Rm->getval());
        else if(op == BinaryInstruction::MUL) Rn->setval(Rn->getval() * Rm->getval());
        else if(op == BinaryInstruction::MOD) Rn->setval(Rn->getval() % Rm->getval());
        else if(op == BinaryInstruction::DIV) Rn->setval(Rn->getval() / Rm->getval());
        else if(op == BinaryInstruction::LSL) Rn->setval(Rn->getval() << Rm->getval());
        else if(op == BinaryInstruction::LSR) Rn->setval((unsigned)Rn->getval() >> Rm->getval());
        else if(op == BinaryInstruction::ASR) Rn->setval(Rn->getval() >> Rm->getval());
        else if(op == BinaryInstruction::AND) Rn->setval(Rn->getval() & Rm->getval());
        else if(op == BinaryInstruction::OR) Rn->setval(Rn->getval() | Rm->getval());

        if(Rn->isbigimm())
        {
            Rn = load_imm(m_bb, Rn);
        }
        m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
        return;
    }
    switch(op)
    {
        // using __aeabi_idiv to realize div and mod
        // case BinaryInstruction::DIV:
        //     if(Rm->isimm())
        //     {
        //         // if Rm = 2^n
        //         // if(((Rm->getval()-1) & Rm->getval()) == 0)
        //         // {
        //         //     shift_type* shift = new shift_type(shift_type::ASR , __builtin_ctz(Rm->getval()));
        //         //     Rn->set_shift(shift);
        //         //     m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
        //         //     return;
        //         // }
        //         // else 
        //             Rm = load_imm(m_bb, Rm);
        //     }
        // case BinaryInstruction::MOD:
        //     r0_0 = gen_reg_operand(0, m_operand::REG);
        //     r1_0 = gen_reg_operand(1, m_operand::REG);
        //     m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, r0_0, Rn));
        //     m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, r1_0, Rm));
        //     if(op == BinaryInstruction::DIV)
        //     {
        //         m_operand* addr_Rd = gen_label_operand("__aeabi_idiv");
        //         m_operand* r0 = gen_reg_operand(0, m_operand::REG);
        //         m_bb->insert_inst(gen_branch_mi(branch_mi::BL, addr_Rd));
        //         m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, r0));
        //     }
        //     else
        //     {
        //         m_operand* addr_Rd = gen_label_operand("__aeabi_idivmod");
        //         m_operand* r1 = gen_reg_operand(1, m_operand::REG);
        //         m_bb->insert_inst(gen_branch_mi(branch_mi::BL, addr_Rd));
        //         m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, r1));
        //     }
        //     m_bb->get_parent()->add_saved_regs(14);
        //     break;
        case BinaryInstruction::DIV:
            if(Rn->isimm()) Rn = load_imm(m_bb, Rn);
            if(Rm->isimm()) Rm = load_imm(m_bb, Rm);
            m_bb->insert_inst(gen_binary_mi(binary_mi::DIV, Rd, Rn, Rm));
            break;
        case BinaryInstruction::MOD:
            if(Rn->isimm()) Rn = load_imm(m_bb, Rn);
            if(Rm->isimm()) Rm = load_imm(m_bb, Rm);
            div_reg0 = gen_reg_operand(vreg_num);
            div_reg1 = gen_reg_operand(vreg_num++);
            mul_reg0 = gen_reg_operand(vreg_num);
            mul_reg1 = gen_reg_operand(vreg_num++);
            m_bb->insert_inst(gen_binary_mi(binary_mi::DIV, div_reg0, Rn, Rm));
            Rn = gen_reg_operand(Rn->get_reg());
            Rm = gen_reg_operand(Rm->get_reg());
            m_bb->insert_inst(gen_binary_mi(binary_mi::MUL, mul_reg0, div_reg1, Rm));
            m_bb->insert_inst(gen_binary_mi(binary_mi::SUB, Rd, Rn, mul_reg1));
            break;
        case BinaryInstruction::MUL:
            if(Rn->isimm())
            {
                m_operand* tmp = Rm;
                Rm = Rn;
                Rn = tmp;
            }
            if(Rm->isimm())
            {
                // if Rm = 2^n
                // if(((Rm->getval()-1) & Rm->getval()) == 0)
                // {
                //     shift_type* shift = new shift_type(shift_type::LSL , __builtin_ctz(Rm->getval()));
                //     Rn->set_shift(shift);
                //     m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
                //     return;
                // }
                // else 
                Rm = load_imm(m_bb, Rm);
            }
            m_bb->insert_inst(gen_binary_mi(op, Rd, Rn, Rm));
            break;
        case BinaryInstruction::ADD:
            if(Rn->isimm())
            {
                m_operand* tmp = Rm;
                Rm = Rn;
                Rn = tmp;
            }
        case BinaryInstruction::AND:
        case BinaryInstruction::OR:
        case BinaryInstruction::ASR:
        case BinaryInstruction::LSL:
        case BinaryInstruction::LSR:
        case BinaryInstruction::MINUS:
            if(Rn->isimm())  Rn = load_imm(m_bb, Rn);
            m_bb->insert_inst(gen_binary_mi(op, Rd, Rn, Rm));
            break;
        default:
            Exception("gen_machine::convert_binary_inst");
            cerr << op << endl;
    }
}

void gen_machine::convert_unary_inst(m_block*& m_bb, UnaryInstruction* inst)
{
    if(inst->getDef() == nullptr)
        exit(4);
    if(inst->getOP() == UnaryInstruction::UMINUS)
    {
        m_operand* Rd = gen_operand(inst->getDef(), m_bb);
        m_operand* Rn = gen_imm_operand(0, m_bb);
        Rn = load_imm(m_bb, Rn);
        m_operand* Rm = gen_operand(inst->getUse()[0], m_bb);
        m_bb->insert_inst(gen_binary_mi(binary_mi::SUB, Rd, Rn, Rm));
    }
    else if(inst->getOP() == UnaryInstruction::NOT)
    {
        m_operand* Rd = gen_operand(inst->getDef(), m_bb);
        m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
        if(Rn->isimm()) Rn = load_imm(m_bb, Rn);
        m_operand* Rm = gen_operand(inst->getUse()[0], m_bb);
        m_bb->insert_inst(gen_test_mi(Rn, Rm));
        
        m_operand* reg_0 = gen_reg_operand(vreg_num);
        m_operand* reg_1 = gen_reg_operand(vreg_num++, m_operand::VREG, shift_type::LSL, 1);
        m_operand* reg_2 = gen_reg_operand(vreg_num);
        m_operand* reg_3 = gen_reg_operand(vreg_num++, m_operand::VREG, shift_type::LSR, 31);
        m_bb->insert_inst(gen_mrs_mi(reg_0));
        m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, reg_2, reg_1));
        m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, reg_3));
    }
    else
    {
        Exception("gen_machine::convert_unary_inst");
    }
}

void gen_machine::convert_copy_inst(m_block*& m_bb, CopyInstruction* inst)
{
    if(inst->getDef() == nullptr)
        exit(5);
    if(inst->getUse().size() != 1)
        exit(6);
    m_operand* Rd = gen_operand(inst->getDef(), m_bb);
    m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
    m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
}

void gen_machine::convert_branch_inst(m_block*& m_bb, BranchInstruction* inst)
{
    if(inst->getBranch() == nullptr)
        exit(7);
    std::ostringstream buf;
    buf << ".L" << inst->getBranch()->getNo();
    string label = buf.str();
    m_operand* Rd = gen_label_operand(label);
    m_instruction *br = gen_branch_mi(branch_mi::B, Rd);
    dynamic_cast<branch_mi*>(br)->set_block(inst->getBranch()->getNo());
    m_bb->insert_inst(br);
}

void gen_machine::convert_cmp_inst(m_block*& m_bb, CmpInstruction* inst) 
{
    m_operand* Rd = gen_operand(inst->getDef(), m_bb);
    m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
    m_operand* Rm = gen_operand(inst->getUse()[1], m_bb);
    if(Rn->isimm()) Rn = load_imm(m_bb, Rn);
    m_bb->insert_inst(gen_cmp_mi(Rn, Rm));
    
    m_operand* imm = gen_imm_operand(1, m_bb);
    m_bb->insert_inst(gen_it_mi(inst->getOP()));
    m_instruction* i = gen_mov_mi(mov_mi::MOV, Rd, imm);
    i->setCond(inst->getOP());
    m_bb->insert_inst(i);
}

void gen_machine::convert_cmp_if_inst(m_block*& m_bb, CmpInstruction* cmp_inst, IfInstruction* if_inst)
{
    if(cmp_inst->getUse().size() != 2)
        exit(8);
    if(cmp_inst->getDef() == nullptr)
        exit(9);
    if(if_inst->getBranch() == nullptr)
        exit(10);
    if(if_inst->getFalseBranch() == nullptr)
        exit(11);
    m_operand* Rn = gen_operand(cmp_inst->getUse()[0], m_bb);
    m_operand* Rm = gen_operand(cmp_inst->getUse()[1], m_bb);
    if(Rn->isimm())  Rn = load_imm(m_bb, Rn);
    std::ostringstream buf;
    string tmp;
    buf << ".L" << if_inst->getBranch()->getNo();
    tmp = buf.str();
    buf.str("");
    m_operand* true_label = gen_label_operand(tmp);
    buf << ".L" << if_inst->getFalseBranch()->getNo();
    tmp = buf.str();
    m_operand* false_label = gen_label_operand(tmp);
    m_bb->insert_inst(gen_cmp_mi(Rn, Rm));
    m_instruction *true_branch = gen_branch_mi(cmp_inst->getOP(), true_label);
    m_instruction *false_branch = gen_branch_mi(branch_mi::B, false_label);
    dynamic_cast<branch_mi*>(true_branch)->set_block(if_inst->getBranch()->getNo());
    dynamic_cast<branch_mi*>(false_branch)->set_block(if_inst->getFalseBranch()->getNo());
    m_bb->insert_inst(true_branch);
    m_bb->insert_inst(false_branch);
}

void gen_machine::convert_addr_inst(m_block*& m_bb, AddrInstruction* inst)
{
    if(inst->getDef() == nullptr)
        exit(12);
    if(inst->getUse().size() < 2)
        exit(13);
    m_operand* Rd = gen_operand(inst->getDef(), m_bb);
    m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
    m_operand* addr_Rd = nullptr;
    if(inst->getUse()[0]->isVar())
    {
        if(Rn->islabel())
        {
            addr_Rd = gen_reg_operand(vreg_num++);
            m_bb->insert_inst(gen_access_mi(access_mi::LDR, addr_Rd, Rn));
        }
        else
        {
            Variable* tmp = dynamic_cast<Variable*>(inst->getUse()[0]);
            Rn = gen_imm_operand(- tmp->getSymPtr()->offset, m_bb);
            if(!Rn->isimm())  m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());
            addr_Rd = gen_reg_operand(vreg_num++);
            m_operand* sp = gen_reg_operand(13, m_operand::REG);
            m_bb->insert_inst(gen_binary_mi(binary_mi::ADD, addr_Rd, sp, Rn));
            if(Rn->isimm())  m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());
        }
    }
    else
    {
        addr_Rd = Rn;
    }
    m_operand* Rm = nullptr;
    if(inst->getUse()[1]->isConstant())
    {
        Constant* co = dynamic_cast<Constant*>(inst->getUse()[1]);
        Rm = gen_imm_operand(co->getValue() * 4, m_bb);
    }
    else
    {
        Rm = gen_operand(inst->getUse()[1], m_bb, shift_type::LSL, 2);
    }
    m_operand* addr_Rd2 = new m_operand(*addr_Rd);
    m_bb->insert_inst(gen_binary_mi(binary_mi::ADD, Rd, addr_Rd2, Rm));
}

void gen_machine::convert_alloca_inst(m_block*& m_bb, AllocaInstruction* inst)
{
    if(inst == nullptr)
        exit(14);
    if(inst->getDef() == nullptr)
        exit(20);
    Variable* tmp = dynamic_cast<Variable*>(inst->getDef());
    m_bb->get_parent()->set_stack_size(tmp->getSymPtr()->tp->size);
    tmp->getSymPtr()->offset = m_bb->get_parent()->get_stack_size();
}

void gen_machine::convert_ret_inst(m_block*& m_bb, RetInstruction* inst)
{
    if(inst == nullptr)
        exit(15);
    m_instruction *i;
    m_operand* sp1 = gen_reg_operand(13, m_operand::REG);
    m_operand* sp2 = gen_reg_operand(13, m_operand::REG);
    m_operand* offset = gen_imm_operand(-1, m_bb);
    if(!inst->getUse().empty())
    {
        m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
        m_operand* Rd = gen_reg_operand(0, m_operand::REG);
        m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
    }
    i = gen_binary_mi(binary_mi::ADD, sp1, sp2, offset);
    m_bb->insert_inst(i);
    m_bb->get_parent()->add_sp_backpatch(i);
    m_operand* lr = gen_reg_operand(14, m_operand::REG);
    i = gen_stack_mi(stack_mi::POP, std::vector<m_operand*>());
    m_bb->insert_inst(i);
    m_bb->get_parent()->add_reg_back_patch(i);
    m_bb->insert_inst(gen_branch_mi(branch_mi::BX, lr));
}

void gen_machine::convert_call_inst(m_block*& m_bb, CallInstruction* inst)
{
    // save r0-r3
    // push param
    if(inst == nullptr)
        exit(16);
    vector<Operand*> params = inst->getUse();
    int num = params.size();
    auto iter = params.begin();
    for(int i=0; i < num; i++)
    {
        if(i < 4)
        {
            m_operand* Rd = gen_reg_operand(i, m_operand::REG);
            m_operand* Rn = gen_operand((*iter), m_bb);
            m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
            iter++;
        }
        else
        {
            m_operand* Rn = gen_operand(params.back(), m_bb);
            params.pop_back(); // vector params has been changed!!!!!
            if(Rn->isimm())
            {
                Rn = load_imm(m_bb, Rn);
            }
            m_bb->insert_inst(gen_stack_mi(stack_mi::PUSH, Rn));
        }
    }

    // call function
    m_operand* addr_label = gen_label_operand(inst->getSymPtr()->name);
    m_bb->insert_inst(gen_branch_mi(branch_mi::BL, addr_label));

    // add sp
    if(num > 4)
    {
        m_operand* offset = gen_imm_operand((num-4)*4, m_bb);
        m_operand* Rd = gen_reg_operand(13, m_operand::REG);
        m_operand* Rn = gen_reg_operand(13, m_operand::REG);
        m_bb->insert_inst(gen_binary_mi(binary_mi::ADD, Rd, Rn, offset));
    }

    // save return val
    if(inst->getDef() != nullptr)
    {
        m_operand* Rd = gen_operand(inst->getDef(), m_bb);
        m_operand* Rn = gen_reg_operand(0, m_operand::REG);
        m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, Rd, Rn));
    }

    // restore r0-r3 !there's no need to do this 7.27
}

void gen_machine::convert_store_inst(m_block*& m_bb, StoreInstruction* inst)
{
    if(inst == nullptr)
        exit(17);
    m_operand* Rd = gen_operand(inst->getUse()[0], m_bb);
    m_operand* Rn = gen_operand(inst->getUse()[1], m_bb);
    if(Rd->isimm())
    {
        Rd = load_imm(m_bb, Rd);
    }
    if(inst->getUse()[1]->isTemp())
    {
        // temp variable
        m_bb->insert_inst(gen_access_mi(access_mi::STR, Rd, Rn));
    }
    else if(Rn->islabel())
    {
        // global variable
        m_operand* addr_reg1 = gen_reg_operand(vreg_num);
        m_operand* addr_reg2 = gen_reg_operand(vreg_num++);
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, addr_reg1, Rn));
        m_bb->insert_inst(gen_access_mi(access_mi::STR, Rd, addr_reg2));
    }
    else
    {
        // local variable
        // Variable* src_addr = dynamic_cast<Variable*>(inst->getUse()[1]);
        // m_operand* imm_Rd = gen_imm_operand(-src_addr->getSymPtr()->offset, m_bb);
        // if(!imm_Rd->isimm())  m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());
        // m_operand* sp = gen_reg_operand(13, m_operand::REG);
        // m_bb->insert_inst(gen_access_mi(access_mi::STR, Rd, sp, imm_Rd));
        // if(imm_Rd->isimm()) m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());

        Variable* src_addr = dynamic_cast<Variable*>(inst->getUse()[1]);
        m_operand* imm_Rd = gen_imm_operand(0, m_bb);
        imm_Rd->setval(-src_addr->getSymPtr()->offset);
        m_operand* reg1 = gen_reg_operand(vreg_num);
        m_operand* reg2 = gen_reg_operand(vreg_num++);;
        m_operand* sp = gen_reg_operand(13, m_operand::REG);
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, reg1, imm_Rd));
        m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());
        m_bb->insert_inst(gen_access_mi(access_mi::STR, Rd, sp, reg2));
    }
}

void gen_machine::convert_load_inst(m_block*& m_bb, LoadInstruction* inst)
{
    if(inst == nullptr)
        exit(18);
    m_operand* Rd = gen_operand(inst->getDef(), m_bb);
    m_operand* Rn = gen_operand(inst->getUse()[0], m_bb);
    if(inst->getUse()[0]->isTemp())
    {
        // temp variable
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, Rd, Rn));
    }
    else if(Rn->islabel())
    {
        // global variable
        m_operand* addr_reg1 = gen_reg_operand(vreg_num);
        m_operand* addr_reg2 = gen_reg_operand(vreg_num++);
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, addr_reg1, Rn));
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, Rd, addr_reg2)); 
    }
    else
    {
        // local  variable
        Variable* src_addr = dynamic_cast<Variable*>(inst->getUse()[0]);
        m_operand* imm_Rd = gen_imm_operand(-src_addr->getSymPtr()->offset, m_bb);
        if(!imm_Rd->isimm())  m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());
        m_operand* sp = gen_reg_operand(13, m_operand::REG);
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, Rd, sp, imm_Rd));
        if(imm_Rd->isimm())  m_bb->get_parent()->add_local_backpatch(m_bb->get_inst_list().back());
    }
}

m_instruction* gen_machine::gen_mov_mi(int op, m_operand* Rd, m_operand* Rm)
{
    mov_mi* inst = new mov_mi(op, Rd, Rm);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_binary_mi(int op, m_operand* Rd, m_operand* Rn, m_operand* Rm)
{
    binary_mi* inst = new binary_mi(op, Rd, Rn, Rm);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_cmp_mi(m_operand* Rn, m_operand* Rm)
{
    cmp_mi* inst = new cmp_mi(Rn, Rm);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_branch_mi(int op, m_operand* addr_label)
{
    branch_mi* inst = new branch_mi(op, addr_label);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_access_mi(int op, m_operand* Rd, m_operand* Rn, m_operand* offset)
{
    access_mi* inst = new access_mi(op, Rd, Rn, offset);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_stack_mi(int op, m_operand* Rn)
{
    stack_mi* inst = new stack_mi(op, Rn);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_stack_mi(int op, vector<m_operand*> reg_list)
{
    stack_mi* inst = new stack_mi(op, reg_list);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_test_mi(m_operand* Rn, m_operand* Rm) 
{
    test_mi* inst = new test_mi(Rn, Rm);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_mrs_mi(m_operand* Rd) 
{
    mrs_mi* inst = new mrs_mi(Rd);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_it_mi(int cond) 
{
    it_mi* inst = new it_mi(cond);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_vec_access(int op, vector<m_operand*> list, m_operand* base) 
{
    vaccess* inst = new vaccess(op, list, base);
    return (m_instruction*)inst;
}

m_instruction* gen_machine::gen_vec_mov(m_operand* Rd, m_operand* Rn) 
{
    vmov* inst = new vmov(Rd, Rn);
    return (m_instruction*)inst;
}

m_operand* gen_machine::load_imm(m_block*& m_bb, m_operand* ope)
{
    m_operand* ret_reg = gen_reg_operand(vreg_num);
    m_operand* imm_Rd = gen_reg_operand(vreg_num++);
    if(!ope->isbigimm())
        m_bb->insert_inst(gen_mov_mi(mov_mi::MOV, imm_Rd, ope));
    else
        m_bb->insert_inst(gen_access_mi(access_mi::LDR, imm_Rd, ope));
    return ret_reg;
}

m_operand* gen_machine::gen_reg_operand(int reg_no, int type, int s_type, int s_val)
{
    m_operand* m_ope = new m_operand(type, reg_no, "", s_type, s_val);
    return m_ope;
}

m_operand* gen_machine::gen_imm_operand(int val, m_block* m_bb)
{
    m_operand* m_ope = new m_operand(m_operand::IMM, val);
    if(m_ope->isbigimm())
    {
        m_ope = load_imm(m_bb, m_ope);
    }
    return m_ope;
}

m_operand* gen_machine::gen_label_operand(string label)
{
    m_operand* m_ope = new m_operand(m_operand::LABEL, -1, label);
    return m_ope;
}

m_operand* gen_machine::gen_operand(Operand* operand, m_block* m_bb, int s_type, int s_val)
{
    // cerr << "here ope" << endl;
    if(operand == nullptr)
        exit(19);
    int ope_type = 0;
    int val = 0;
    string label = "";
    if(operand->isVar())
    {
        Variable* vo = dynamic_cast<Variable*>(operand);
        if(vo->getSymPtr()->scope == GLOBAL || isconst(vo->getSymPtr()->tp))
        {
            ope_type = m_operand::LABEL;
            std::ostringstream buf;
            buf << "addr_" << vo->getName().substr(1);
            label = buf.str();
        }
        // else if(vo->getSymPtr()->scope == PARAM)
        // {
        //     // type = m_operand::REG;
        //     // val = 0;
        // }
        else
        {
            ope_type = m_operand::VREG;
            if(vreg_map.find(operand->getName()) == vreg_map.end())
            {
                val = vreg_num++;
                vreg_map[operand->getName()] = val;
            }
            else
            {
                val = vreg_map[operand->getName()];
            }
        }
    }
    else if(operand->isConstant())
    {
        Constant* co = dynamic_cast<Constant*>(operand);
        ope_type = m_operand::IMM;
        val = co->getValue();
    }
    else if(operand->isTemp())
    {
        Temporary* to = dynamic_cast<Temporary*>(operand);
        if(to->getSymPtr()->scope == PARAM)
        {
            // get the no. of the param
            int no = -1, num = 0;
            type* tp = cur_func->getSymPtr()->tp;
            while (tp->func_params[num] != nullptr)
            {
                if(tp->func_params[num]->label == to->getNo())
                {
                    no = num;
                }
                num++;
            }
            if(no < 4)
            {
                ope_type = m_operand::REG;
                val = no;
            }
            else
            {
                // insert load inst
                ope_type = m_operand::VREG;
                val = vreg_num;
                m_operand* Rd = gen_reg_operand(vreg_num++);
                m_operand* Rn = gen_reg_operand(13, m_operand::REG);
                m_operand* offset = gen_imm_operand((no - 4)*4, m_bb);
                if(!offset->isimm())  m_bb->get_parent()->add_param_backpatch(m_bb->get_inst_list().back());
                m_bb->insert_inst(gen_access_mi(access_mi::LDR, Rd, Rn, offset));
                if(offset->isimm())  m_bb->get_parent()->add_param_backpatch(m_bb->get_inst_list().back());
            }
        }
        else
        {
            ope_type = m_operand::VREG;
            if(vreg_map.find(operand->getName()) == vreg_map.end())
            {
                val = vreg_num ++;
                vreg_map[operand->getName()] = val;
            }
            else
            {
                val = vreg_map[operand->getName()];
            }
        }
    }
    else
    {
        Exception("gen_machine::gen_operand");
    }
    m_operand* m_ope = new m_operand(ope_type, val, label, s_type, s_val);
    if(m_ope->isbigimm())
    {
        m_ope = load_imm(m_bb, m_ope);
    }
    return m_ope;
}