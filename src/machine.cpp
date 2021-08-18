#include "machine.h"
#include <algorithm>
static int literal_count = symbol_table::gen_label(1);
static void Exception(string info) { cerr << "Exception in " << info << endl; }

m_unit::m_unit(symbol_table *global_decl, ostream &out) : o(out)
{
    this->global_decl = global_decl;
}

void m_unit::insert_func(m_function *func)
{
    this->func_list.push_back(func);
}

void m_unit::back_patch() 
{
    for (auto iter = this->func_list.begin(); iter != this->func_list.end(); iter++)
    {
        (*iter)->back_patch();
    }
}

void m_unit::gen_global_decl_code()
{
    symbol_entry *se = this->global_decl->all;
    vector<symbol_entry *> data, rodata, bss;
    while (se)
    {
        if (se->str == 1)
            rodata.push_back(se);
        else if (isarray(se->tp))
        {
            if (se->array_value == NULL)
                bss.push_back(se);
            else
            {
                type *t = se->tp;
                while (isarray(t))
                    t = t->operand;
                if (isconst(t))
                    rodata.push_back(se);
                else
                {
                    int length = se->tp->size / se->tp->align;
                    int i;
                    for (i = 0; i < length; i++)
                    {
                        if (se->array_value[i])
                        {
                            data.push_back(se);
                            break;
                        }
                    }
                    if (i >= length)
                        bss.push_back(se);
                }
            }
        }
        else if (isconst(se->tp))
            rodata.push_back(se);
        else
        {
            if (se->constant == 0)
                bss.push_back(se);
            else
                data.push_back(se);
        }
        se = se->prev;
    }
    if (!data.empty())
        out() << "\t.data\n";

    while (!data.empty())
    {
        se = data.back();
        data.pop_back();
        out() << "\t.global " << se->name.c_str() << "\n";
        out() << "\t.align " << se->tp->align << "\n\t.size " << se->name.c_str() << ", " << se->tp->size << "\n";
        out() << se->name.c_str() << ":\n";
        if (isarray(se->tp))
        {
            int length = se->tp->size / se->tp->align;
            for (int i = 0; i < length; i++)
                out() << "\t.word " << se->array_value[i] << "\n";
        }
        else
            out() << "\t.word " << se->constant << "\n";
    }

    while (!bss.empty())
    {
        se = bss.back();
        bss.pop_back();
        out() << "\t.comm " << se->name.c_str() << ", " << se->tp->size << ", " << se->tp->align << "\n";
    }

    if (!rodata.empty())
        out() << "\t.section .rodata\n";
    while (!rodata.empty())
    {
        se = rodata.back();
        rodata.pop_back();
        if (se->str == 1)
        {
            se->name.pop_back();
            out() << "\t.align 2\n.STR" << se->label << ":\n\t.ascii " << se->name.c_str() << "\\000\"\n";
        }
        else
        {
            out() << "\t.global " << se->name.c_str() << "\n";
            out() << "\t.align " << se->tp->align << "\n";
            out() << "\t.size " << se->name.c_str() << ", " << se->tp->size << "\n";
            out() << se->name.c_str() << ":\n";
            if (isarray(se->tp))
            {
                int length = se->tp->size / se->tp->align;
                for (int i = 0; i < length; i++)
                    out() << "\t.word " << se->array_value[i] << "\n";
            }
            else
                out() << "\t.word " << se->constant << "\n";
        }
    }
}

void m_unit::gen_reloc()
{
    symbol_entry *se = this->global_decl->all;
    while (se)
    {
        if (se->str)
            out() << "addr_STR" << se->label << literal_count << ":\n\t.word .STR" << se->label << "\n";
        else
            out() << "addr_" << se->name.c_str() << literal_count << ":\n\t.word " << se->name.c_str() << "\n";
        se = se->prev;
    }
}

void m_unit::gen_literal_poll(int n)
{
    code_count += n;
    if (code_count >= max_count)
    {
        out() << "\tb .L" << literal_count << "\n";
        out() << ".LTORG\n";
        gen_reloc();
        out() << ".L" << literal_count << ":\n";
        code_count = 0;
        literal_count = symbol_table::gen_label(1);
    }
}

void m_unit::gen_machine_code()
{
    out() << "\t.arch armv8-a\n";
    out() << "\t.arch_extension crc\n";
    out() << "\t.arm\n";
    out() << "\t.fpu neon\n";
    gen_global_decl_code();
    for (auto iter = this->func_list.begin(); iter != this->func_list.end(); iter++)
    {
        (*iter)->gen_machine_code();
    }
    gen_reloc();
    out() << "\n\t.ident\t\"NKU-TEDA\"\n";
}

m_function::m_function(symbol_entry *sym_ptr, m_unit *p)
{
    this->sym_ptr = sym_ptr;
    this->parent = p;
}

void m_function::insert_blk(m_block *blk)
{
    this->block_list.push_back(blk);
}

void m_function::erase_blk(m_block* blk) 
{
    auto it = std::find(block_list.begin(), block_list.end(), blk);
    if(it != block_list.end())
        block_list.erase(it);
}

void m_function::gen_func_entry_code()
{
    out() << "\t.global " << this->sym_ptr->name.c_str() << "\n";
    out() << "\t.type " << this->sym_ptr->name.c_str() << " , %function\n";
    out() << this->sym_ptr->name.c_str() << ":\n";
}

void m_function::gen_machine_code()
{
    gen_func_entry_code();
    std::vector<m_block *> worklist;
    std::set<m_block *> v;
    std::vector<m_block *> q;
    q.push_back(entry_bb);
    v.insert(entry_bb);
    while (!q.empty())
    {
        auto p = q.back();
        q.pop_back();
        worklist.push_back(p);
        for (auto i : p->get_succ())
        {
            if (v.find(i) == v.end())
            {
                v.insert(i);
                q.push_back(i);
            }
        }
    }
    for (int i = 0; i < (int)worklist.size() - 1; i++)
    {
        if (worklist[i]->get_inst_list().empty())
            continue;
        m_instruction *inst = *worklist[i]->get_inst_list().rbegin();
        if (!inst->isBranch() || inst->getOP() == branch_mi::BX)
            continue;
        branch_mi *br = dynamic_cast<branch_mi *>(inst);
        if (br->get_block() == worklist[i + 1]->get_no())
            br->get_parent()->remove(br);
    }
    for (auto &bb : worklist)
        bb->gen_machine_code();
}

void m_function::add_saved_regs(int reg_no)
{
    saved_regs.insert(reg_no);
}

void m_function::back_patch()
{
    // batchpatch sub/add sp inst
    if (stack_size == 0)
    {
        for (auto &inst : sp_backpatch)
            inst->get_parent()->remove(inst);
    }
    else
    {
        for (auto &sb : sp_backpatch)
        {
            if (sb->get_imm_ope())
            {
                if (this->stack_size > 255)
                {
                    m_operand *reg = new m_operand(m_operand::REG, 4);
                    m_operand *imm = new m_operand(m_operand::IMM, this->stack_size);
                    access_mi *inst = new access_mi(access_mi::LDR, reg, imm);
                    sb->get_parent()->insert_before(inst, sb);
                    sb->replace_use(sb->get_imm_ope(), reg);
                    saved_regs.insert(4);
                }
                else
                    sb->get_imm_ope()->setval(this->stack_size);
            }
        }
    }

    for (auto &bb : block_list)
    {
        int ps = 0;
        auto t = bb->get_inst_list();
        for (auto &inst : t)
        {
            if (inst->isPush())
                ps += dynamic_cast<stack_mi *>(inst)->get_reg_size() * 4;
            else if (inst->isBranch())
                ps = 0;
            else if (local_backbatch.find(inst) != local_backbatch.end())
            {
                if (inst->get_imm_ope())
                {
                    int tmp = inst->get_imm_ope()->getval();
                    int offset = tmp + this->stack_size + ps;
                    if (inst->getType() == m_instruction::ACCESS)
                    {
                        if (offset < 4096)
                        {
                            inst->get_imm_ope()->setval(offset);
                        }
                        else if (dynamic_cast<access_mi *>(inst)->is_load_imm())
                        {
                            inst->get_imm_ope()->setval(offset);
                        }
                        else
                        {
                            int reg_no = inst->getDef()[0]->get_reg();
                            m_operand *reg = new m_operand(m_operand::REG, reg_no);
                            m_operand *imm = new m_operand(m_operand::IMM, offset);
                            access_mi *i = new access_mi(access_mi::LDR, reg, imm);
                            inst->replace_use(inst->get_imm_ope(), reg);
                            inst->get_parent()->insert_before(i, inst);
                        }
                    }
                    else if (inst->getType() == m_instruction::BINARY)
                    {
                        if (offset < 255)
                        {
                            inst->get_imm_ope()->setval(offset);
                        }
                        else
                        {
                            int reg_no = inst->getDef()[0]->get_reg();
                            m_operand *reg = new m_operand(m_operand::REG, reg_no);
                            m_operand *imm = new m_operand(m_operand::IMM, offset);
                            access_mi *i = new access_mi(access_mi::LDR, reg, imm);
                            inst->replace_use(inst->get_imm_ope(), reg);
                            inst->get_parent()->insert_before(i, inst);
                        }
                    }
                    else
                    {
                        Exception("BackPatch");
                    }
                }
            }
            else if (param_backbatch.find(inst) != param_backbatch.end())
            {
                if (inst->get_imm_ope())
                {
                    int tmp = inst->get_imm_ope()->getval();
                    int offset = tmp + this->stack_size + this->saved_regs.size() * 4 + ps;

                    if (offset < 4096)
                    {
                        inst->get_imm_ope()->setval(offset);
                    }
                    else if (dynamic_cast<access_mi *>(inst)->is_load_imm())
                    {
                        inst->get_imm_ope()->setval(offset);
                    }
                    else
                    {
                        int reg_no = inst->getDef()[0]->get_reg();
                        m_operand *reg = new m_operand(m_operand::REG, reg_no);
                        m_operand *imm = new m_operand(m_operand::IMM, offset);
                        access_mi *i = new access_mi(access_mi::LDR, reg, imm);
                        inst->replace_use(inst->get_imm_ope(), reg);
                        inst->get_parent()->insert_before(i, inst);
                    }
                }
            }
        }
    }
    // backpatch context switch
    if (saved_regs.empty())
        for (auto &inst : reg_back_patch)
            inst->get_parent()->remove(inst);
    else
    {
        std::vector<m_operand *> regs;
        for (auto &regno : saved_regs)
        {
            m_operand *o = new m_operand(m_operand::REG, regno);
            regs.push_back(o);
        }
        for (auto &inst : reg_back_patch)
            dynamic_cast<stack_mi *>(inst)->set_reg_list(regs);
    }
}

m_block::m_block(int no, m_function *p)
{
    this->no = no;
    this->parent = p;
}

void m_block::insert_front(m_instruction *inst)
{
    this->inst_list.insert(this->inst_list.begin(), inst);
    inst->set_parent(this);
}

void m_block::insert_inst(m_instruction *inst)
{
    this->inst_list.push_back(inst);
    inst->set_parent(this);
}

void m_block::insert_after(m_instruction *dst, m_instruction *src)
{
    auto it = std::find(inst_list.begin(), inst_list.end(), src);
    it++;
    inst_list.insert(it, dst);
    src->set_parent(this);
    dst->set_parent(this);
}

void m_block::insert_before(m_instruction *dst, m_instruction *src)
{
    auto it = std::find(inst_list.begin(), inst_list.end(), src);
    inst_list.insert(it, dst);
    src->set_parent(this);
    dst->set_parent(this);
}

void m_block::gen_machine_code()
{
    out() << ".L" << this->no << ":\n";
    for (auto iter = this->inst_list.begin(); iter != this->inst_list.end(); iter++)
    {
        (*iter)->gen_machine_code();
        this->parent->get_parent()->gen_literal_poll();
    }
}

void m_block::remove(m_instruction *inst)
{
    inst_list.erase(std::find(inst_list.begin(), inst_list.end(), inst));
}

std::list<m_instruction*>::iterator m_block::get_last_nonbranch_inst() 
{
    for(auto it = inst_list.rbegin(); it != inst_list.rend(); it++)
    {
        if((*it)->isBranch() || (*it)->isCmp())
            continue;
        return std::find(inst_list.begin(), inst_list.end(), *it);
    }
    return inst_list.end();
}

branch_mi::branch_mi(int op, m_operand *addr_label)
{
    this->type = m_instruction::BRANCH;
    this->op = op;
    this->addr_label = addr_label;
    addr_label->set_parent(this);
    if (op == BL)
    {
        m_operand *r0d = new m_operand(m_operand::REG, 0);
        m_operand *r0u = new m_operand(m_operand::REG, 0);
        m_operand *r1d = new m_operand(m_operand::REG, 1);
        m_operand *r1u = new m_operand(m_operand::REG, 1);
        m_operand *r2d = new m_operand(m_operand::REG, 2);
        m_operand *r2u = new m_operand(m_operand::REG, 2);
        m_operand *r3d = new m_operand(m_operand::REG, 3);
        m_operand *r3u = new m_operand(m_operand::REG, 3);
        r0d->set_parent(this);
        r0u->set_parent(this);
        r1d->set_parent(this);
        r1u->set_parent(this);
        r2d->set_parent(this);
        r2u->set_parent(this);
        r3d->set_parent(this);
        r3u->set_parent(this);
        def = {r0d, r1d, r2d, r3d};
        use = {r0u, r1u, r2u, r3u};
    }
    else if(op == BX)
    {
        m_operand *r0 = new m_operand(m_operand::REG, 0);
        m_operand *sp = new m_operand(m_operand::REG, 13);
        r0->set_parent(this);
        sp->set_parent(this);
        use = {r0, sp};
    }
}

void branch_mi::replace_use(m_operand *o, m_operand *n)
{
    if (addr_label == o)
    {
        addr_label = n;
        addr_label->set_parent(this);
    }
}

void branch_mi::gen_machine_code()
{
    switch (this->op)
    {
    case B:
        out() << "\tb";
        break;
    case BEQ:
        out() << "\tbeq";
        break;
    case BNE:
        out() << "\tbne";
        break;
    case BGT:
        out() << "\tbgt";
        break;
    case BLT:
        out() << "\tblt";
        break;
    case BGE:
        out() << "\tbge";
        break;
    case BLE:
        out() << "\tble";
        break;
    case BL:
        out() << "\tbl";
        break;
    case BX:
        out() << "\tbx";
        break;
    case BR:
        out() << "\tbr";
        break;
    default:
        out() << "\tnop";
        Exception("branch_mi::gen_machine_code()");
    }
    out() << "\t";
    this->addr_label->gen_machine_code();
    out() << "\n";
    // gen_literal_poll();
}

cmp_mi::cmp_mi(m_operand *Rd, m_operand *Rn)
{
    this->type = m_instruction::CMP;
    this->Rd = Rd;
    this->Rn = Rn;
    Rd->set_parent(this);
    Rn->set_parent(this);
}

std::vector<m_operand *> branch_mi::getDef()
{
    return def;
}

std::vector<m_operand *> branch_mi::getUse()
{
    return use;
}

void cmp_mi::gen_machine_code()
{
    out() << "\tcmp ";
    this->Rd->gen_machine_code();
    out() << ", ";
    this->Rn->gen_machine_code();
    out() << "\n";
    // gen_literal_poll();
}

void cmp_mi::replace_use(m_operand *o, m_operand *n)
{
    if (Rd == o)
    {
        n->set_parent(this);
        Rd = n;
    }
    if (Rn == o)
    {
        n->set_parent(this);
        Rn = n;
    }
}

std::vector<m_operand *> cmp_mi::getUse()
{
    std::vector<m_operand *> ret;
    ret.push_back(Rd);
    if (Rn->isreg() || Rn->isvreg())
        ret.push_back(Rn);
    return ret;
}

m_operand* cmp_mi::get_imm_ope() 
{
    if(Rn->isimm())
    {
        return Rn;
    }
    return nullptr;
}

int cmp_mi::get_exec_time() const
{
    if(Rn->get_shift() == nullptr)
        return 1;
    return 2;
}

binary_mi::binary_mi(int op, m_operand *Rd, m_operand *Rn, m_operand *Rm)
{
    this->type = m_instruction::BINARY;
    this->op = op;
    this->Rd = Rd;
    this->Rn = Rn;
    this->Rm = Rm;
    Rd->set_parent(this);
    Rn->set_parent(this);
    Rm->set_parent(this);
}

void binary_mi::replace_def(m_operand *o, m_operand *n)
{
    if (Rd == o)
    {
        n->set_parent(this);
        Rd = n;
    }
}

void binary_mi::replace_use(m_operand *o, m_operand *n)
{
    if (Rm == o)
    {
        n->set_parent(this);
        Rm = n;
    }
    if (Rn == o)
    {
        n->set_parent(this);
        Rn = n;
    }
}

void binary_mi::gen_machine_code()
{
    switch (this->op)
    {
    case ADD:
        out() << "\tadd";
        break;
    case SUB:
        out() << "\tsub";
        break;
    case MUL:
        out() << "\tmul";
        break;
    case DIV:
        out() << "\tsdiv";
        break;
    case RSB:
        out() << "\trsb";
        break;
    case AND:
        out() << "\tand";
        break;
    case OR:
        out() << "\tor";
        break;
    case ASR:
        out() << "\tasr";
        break;
    case LSR:
        out() << "\tlsr";
        break;
    case LSL:
        out() << "\tlsl";
        break;
    default:
        Exception("binary_mi::gen_machine_code()");
    }
    out() << " ";
    this->Rd->gen_machine_code();
    out() << ", ";
    this->Rn->gen_machine_code();
    out() << ", ";
    this->Rm->gen_machine_code();
    out() << "\n";
    // gen_literal_poll();
}

m_operand *binary_mi::get_imm_ope()
{
    switch (this->op)
    {
    case ADD:
    case SUB:
        if (Rm->isimm())
            return Rm;
        break;
    default:
        break;
    }
    return nullptr;
}

int binary_mi::get_exec_time() const
{
    if(op == DIV)
        return 8;
    if(op == MUL)
        return 3;
    if(Rn->get_shift() == nullptr)
        return 1;
    return 2;
}

mov_mi ::mov_mi(int op, m_operand *Rd, m_operand *Rn)
{
    this->type = m_instruction ::MOV;
    this->op = op;
    this->Rd = Rd;
    this->Rn = Rn;
    this->cond = NONE;
    Rd->set_parent(this);
    Rn->set_parent(this);
}

void mov_mi::replace_def(m_operand *o, m_operand *n)
{
    if (Rd == o)
    {
        n->set_parent(this);
        Rd = n;
    }
}

void mov_mi::replace_use(m_operand *o, m_operand *n)
{
    if (Rn == o)
    {
        n->set_parent(this);
        Rn = n;
    }
}

std::vector<m_operand *> binary_mi::getUse()
{
    std::vector<m_operand *> ret;
    if (Rm->isreg() || Rm->isvreg())
        ret.push_back(Rm);
    ret.push_back(Rn);
    return ret;
}

void mov_mi ::gen_machine_code()
{
    if (this->op == MOV)
        out() << "\tmov";
    else
        out() << "\tmvn";
    gen_cond();
    out() << " ";
    this->Rd->gen_machine_code();
    out() << ", ";
    this->Rn->gen_machine_code();
    out() << "\n";
    if (cond != NONE)
    {
        if (this->op == MOV)
            out() << "\tmov";
        else
            out() << "\tmvn";
        gen_cond(true);
        out() << " ";
        this->Rd->gen_machine_code();
        out() << ", ";
        Rn->setval(1 - Rn->getval());
        this->Rn->gen_machine_code();
        out() << "\n";
    }
    // gen_literal_poll();
}

bool mov_mi::is_pure_mov() const
{
    shift_type *shift = Rn->get_shift();
    if (shift == nullptr)
        return true;
    if (shift->get_imm() == 0)
        return true;
    return false;
}

std::vector<m_operand *> mov_mi::getUse()
{
    std::vector<m_operand *> ret;
    if (Rn->isreg() || Rn->isvreg())
        ret.push_back(Rn);
    return ret;
}

access_mi ::access_mi(int op, m_operand *Rd, m_operand *Rn, m_operand *offset)
{
    this->type = m_instruction ::ACCESS;
    this->op = op;
    this->Rd = Rd;
    this->Rn = Rn;
    this->offset = offset;
    Rd->set_parent(this);
    Rn->set_parent(this);
    if (offset)
        offset->set_parent(this);
}

void access_mi::replace_def(m_operand *o, m_operand *n)
{
    if (op == STR)
        return;
    if (Rd == o)
    {
        n->set_parent(this);
        Rd = n;
    }
}

void access_mi::replace_use(m_operand *o, m_operand *n)
{
    if (offset == o)
    {
        n->set_parent(this);
        offset = n;
    }
    if (Rn == o)
    {
        n->set_parent(this);
        Rn = n;
    }
    if (op == STR && Rd == o)
    {
        n->set_parent(this);
        Rd = n;
    }
}

void access_mi ::gen_machine_code()
{
    if (this->op == LDR)
        out() << "\tldr ";
    else
        out() << "\tstr ";
    this->Rd->gen_machine_code();
    out() << ", ";

    // ldr out of range imm;
    if (this->Rn->isimm())
    {
        out() << "=" << this->Rn->getval() << "\n";
        // gen_literal_poll();
        return;
    }

    // ldr addr
    if (this->Rn->isreg())
        out() << "[";
    this->Rn->gen_machine_code();
    if (this->offset)
    {
        out() << ", ";
        this->offset->gen_machine_code();
    }
    if (this->Rn->isreg())
        out() << "]";
    out() << "\n";
    // gen_literal_poll();
}

void access_mi::add_offset(m_operand* offset) 
{
    this->offset = offset;
    offset->set_parent(this);
}

m_operand *access_mi::get_imm_ope()
{
    if (Rn->isimm())
        return Rn;
    if (offset && offset->isimm())
        return offset;
    return nullptr;
}

int access_mi::get_exec_time() const
{
    if(op == LDR)
        return 4;
    return 1;
}

std::vector<m_operand *> access_mi::getDef()
{
    std::vector<m_operand *> ret;
    if (op == LDR)
        ret.push_back(Rd);
    return ret;
}

std::vector<m_operand *> access_mi::getUse()
{
    std::vector<m_operand *> ret;
    if (Rn->isreg() || Rn->isvreg())
        ret.push_back(Rn);
    if (offset && (offset->isreg() || offset->isvreg()))
        ret.push_back(offset);
    if (op == STR)
        ret.push_back(Rd);
    return ret;
}

stack_mi ::stack_mi(int op, m_operand *Rn)
{
    this->type = STACK;
    this->op = op;
    this->reg_list.push_back(Rn);
    Rn->set_parent(this);
}

stack_mi ::stack_mi(int op, vector<m_operand *> list)
{
    this->type = STACK;
    this->op = op;
    this->reg_list.insert(this->reg_list.end(), list.begin(), list.end());
    for (auto &reg : list)
        reg->set_parent(this);
}

void stack_mi::replace_use(m_operand *o, m_operand *n)
{
    while (true)
    {
        auto it = std::find(reg_list.begin(), reg_list.end(), o);
        if (it == reg_list.end())
            break;
        reg_list.erase(it);
        reg_list.push_back(n);
        n->set_parent(this);
    }
}

void stack_mi ::insert_reg(m_operand *reg)
{
    reg->set_parent(this);
    this->reg_list.push_back(reg);
}

void stack_mi::set_reg_list(vector<m_operand *> a)
{
    for (auto &op : a)
        op->set_parent(this);
    reg_list = a;
}

void stack_mi ::gen_machine_code()
{
    if (this->op == PUSH)
        out() << "\tpush ";
    else if (this->op == POP)
        out() << "\tpop ";
    else
        Exception("stack_mi :: gen_machine_code");
    out() << "{";
    auto iter = this->reg_list.begin();
    (*iter)->gen_machine_code();
    iter++;
    while (iter != reg_list.end())
    {
        out() << ", ";
        (*iter)->gen_machine_code();
        iter++;
    }
    out() << "}\n";
}

std::vector<m_operand *> stack_mi::getDef()
{
    std::vector<m_operand *> ret;
    if (op == POP)
    {
        ret.insert(ret.end(), reg_list.begin(), reg_list.end());
        return ret;
    }
    return ret;
}

std::vector<m_operand *> stack_mi::getUse()
{
    std::vector<m_operand *> ret;
    if (op == PUSH)
    {
        ret.insert(ret.end(), reg_list.begin(), reg_list.end());
        return ret;
    }
    return ret;
}

int stack_mi::get_exec_time() const
{
    if(op == POP)
        return 3 + (int)reg_list.size();
    return (int)reg_list.size();
}

test_mi::test_mi(m_operand *Rn, m_operand *Rm)
{
    this->type = TST;
    this->Rn = Rn;
    this->Rm = Rm;
    Rn->set_parent(this);
    Rm->set_parent(this);
}

void test_mi::gen_machine_code()
{
    out() << "\ttst ";
    this->Rn->gen_machine_code();
    out() << ", ";
    this->Rm->gen_machine_code();
    out() << "\n";
}

void test_mi::replace_use(m_operand *o, m_operand *n)
{
    if (Rn == o)
    {
        this->Rn = n;
        n->set_parent(this);
    }
    if (Rm == o)
    {
        this->Rm = n;
        n->set_parent(this);
    }
}

int test_mi::get_exec_time() const
{
    if(Rn->get_shift() == nullptr)
        return 1;
    return 2;
}

mrs_mi::mrs_mi(m_operand *Rd)
{
    this->Rd = Rd;
    Rd->set_parent(this);
    this->type = MRS;
}

void mrs_mi::gen_machine_code()
{
    out() << "\tmrs ";
    this->Rd->gen_machine_code();
    out() << ", ";
    out() << "cpsr\n";
}

it_mi::it_mi(int flag)
{
    this->cond = flag;
    this->type = IT;
}

void it_mi::gen_machine_code()
{
    out() << "\tite ";
    gen_cond();
    out() << "\n";
}

fused_binary::fused_binary(int op, m_operand* Rd, m_operand* Rm, m_operand* Rn, m_operand* Ra) 
{
    this->type = FUSED_BINARY;
    this->op = op;
    this->Rd = Rd;
    this->Rm = Rm;
    this->Rn = Rn;
    this->Ra = Ra;
    Rd->set_parent(this);
    Rm->set_parent(this);
    Rn->set_parent(this);
    Ra->set_parent(this);
}

void fused_binary::gen_machine_code() 
{
    if(this->op == MLA)
        out() << "\tmla ";
    else
        out() << "\tmls ";
    this->Rd->gen_machine_code();
    out() << ", ";
    this->Rm->gen_machine_code();
    out() << ", ";
    this->Rn->gen_machine_code();
    out() << ", ";
    this->Ra->gen_machine_code();
    out() << "\n";
}

void fused_binary::replace_use(m_operand*o, m_operand*n) 
{
    if(this->Rm == o)
    {
        this->Rm = n;
        n->set_parent(this);
    }
    if(this->Rn == o)
    {
        this->Rn = n;
        n->set_parent(this);
    }
    if(this->Ra == o)
    {
        this->Ra = n;
        n->set_parent(this);
    }
}

fused_cmp::fused_cmp(int op, m_operand* Rn, m_operand* addr_label) 
{
    this->type = FUSED_CMP;
    this->op = op;
    this->Rn = Rn;
    this->addr_label = addr_label;
    Rn->set_parent(this);
    addr_label->set_parent(this);
}

void fused_cmp::gen_machine_code() 
{
    if(this->op == CBZ)
        out() << "\tcbz ";
    else 
        out() << "\tcbnz ";
    this->Rn->gen_machine_code();
    out() << ", ";
    this->addr_label->gen_machine_code();
    out() << "\n";
}

void fused_cmp::replace_use(m_operand* o, m_operand* n) 
{
    if(this->Rn == o)
    {
        this->Rn = n;
        n->set_parent(this);
    }
    if(this->addr_label == o)
    {
        this->addr_label = n;
        n->set_parent(this);
    }

}

vaccess::vaccess(int op, vector<m_operand*> list, m_operand* addr) 
{
    this->type = VACCESS;
    this->op = op;
    this->addr = addr;
    this->reg_list.insert(this->reg_list.end(), list.begin(), list.end());
    this->addr->set_parent(this);
    for (auto &reg : list)
        reg->set_parent(this);
}

void vaccess::gen_machine_code() 
{
    if(this->op == VLDR)    
        out() << "\tvld4.i32 {" ;
    else 
        out() << "\tvst4.i32 {" ;
    for (auto i = reg_list.begin(); i != reg_list.end(); i++)
    {
        (*i)->gen_machine_code();
        if(*i != reg_list.back())
            out() << ", ";
    }
    out() << "}, [";
    this->addr->gen_machine_code();
    out() << "]\n";
}

void vaccess::replace_use(m_operand* o, m_operand* n)
{
    if(this->addr == o)
    {
        this->addr = n;
        n->set_parent(this);
    }
}

int vaccess::get_exec_time() const
{
    if(op == VLDR)
    {
        return 8;
    }
    else
    {
        return 4;
    }
}

vmov::vmov(m_operand* Rd, m_operand* Rn) 
{
    this->type = VMOV;
    this->Rd = Rd;
    this->Rn = Rn;
    Rd->set_parent(this);
    Rn->set_parent(this);
}

void vmov::gen_machine_code() 
{
    out() << "\tvmov.i32 ";
    this->Rd->gen_machine_code();
    out() << ", ";
    this->Rn->gen_machine_code();
    out() << "\n";
}

shift_type ::shift_type(int type, int imm)
{
    this->type = type;
    this->imm = imm;
}

void shift_type ::gen_machine_code()
{
    if (this->isnone())
    {
        return;
    }
    switch (this->type)
    {
    case ASR:
        out() << "asr ";
        break;
    case LSL:
        out() << "lsl ";
        break;
    case LSR:
        out() << "lsr ";
        break;
    case ROR:
        out() << "ror ";
        break;
    case RRX:
        out() << "rrx ";
        break;
    default:
        Exception("shift_type :: gen_machine_code()");
    }
    out() << "#" << this->imm;
}

m_operand ::m_operand(int type, int val, string label, int s_type, int imm, bool minus)
{
    this->type = type;
    if (type == m_operand::LABEL)
    {
        this->label = label;
    }
    else if (type == m_operand::IMM)
    {
        this->imm_val = val;
    }
    else
    {
        this->reg_no = val;
    }
    if (s_type == shift_type::NONE)
        this->shift = nullptr;
    else
    {
        this->shift = new shift_type(s_type, imm);
        this->shift->parent = this;
    }
    this->minus = minus;
    parent = nullptr;
}

bool m_operand::need_color()
{
    return type == VREG || (type == REG && reg_no < 13);
}

void m_operand ::gen_reg_code()
{
    if(this->type == DREG)
    {
        out() << "d" << this->reg_no;
        return;
    }
    if(this->type == QREG)
    {
        out() << "q" << this->reg_no;
        return;
    }
    switch (this->reg_no)
    {
    case 13:
        out() << "sp";
        break;
    case 14:
        out() << "lr";
        break;
    case 15:
        out() << "pc";
        break;
    default:
        out() << "r" << this->reg_no;
    }
}

bool m_operand::operator<(const m_operand &a) const
{
    if (type != a.type)
        return type < a.type;
    if (type == IMM)
        return imm_val < a.imm_val;
    if (type == REG || type == VREG)
    {
        // if (shift == nullptr && a.shift == nullptr)
        //     return reg_no < a.reg_no;
        // if (shift == nullptr)
        //     return true;
        // if (a.shift == nullptr)
        //     return false;
        // if (*shift == *a.shift)
            return reg_no < a.reg_no;
        // return *shift < *a.shift;
    }
    if (type == LABEL)
        return label < a.label;
    return false;
}

bool m_operand::operator==(const m_operand &a) const
{
    if (type != a.type)
        return false;
    if (type == IMM)
        return imm_val == a.imm_val;
    if (type == REG || type == VREG)
    {
        // if(shift == nullptr && a.shift == nullptr)
        //     return reg_no == a.reg_no;
        // if(shift == nullptr)
        //     return false;
        // if(a.shift == nullptr)
        //     return false;
        // return reg_no == a.reg_no && *shift == *a.shift;
        return reg_no == a.reg_no;
    }
    if (type == LABEL)
        return label == a.label;
    return false;
}

void m_operand ::gen_machine_code()
{
    switch (this->type)
    {
    case IMM:
        out() << "#" << this->imm_val;
        break;
    case REG:
    case DREG:
    case QREG:
        gen_reg_code();
        break;
    case LABEL:
        if (this->label.substr(0, 5) == "addr_")
            out() << this->label.c_str() << literal_count;
        else
            out() << this->label.c_str();
        break;
    case VREG:
        // should be del after the register assign algo is
        gen_reg_code();
        break;
    default:
        Exception("m_operand :: gen_machine_code()");
    }
    if (this->shift)
    {
        out() << ", ";
        if (this->minus)
            out() << "-";
        this->shift->gen_machine_code();
    }
}

// insert inst after this
void m_instruction::insert_after(m_instruction *inst)
{
    parent->insert_after(inst, this);
}

// insert inst before this
void m_instruction::insert_before(m_instruction *inst)
{
    parent->insert_before(inst, this);
}

void m_instruction::gen_cond(bool flag)
{
    if (cond == NONE)
        return;
    int new_cond = flag ? 1 ^ cond : cond;
    switch (new_cond)
    {
    case E:
        out() << "eq";
        break;
    case NE:
        out() << "ne";
        break;
    case LE:
        out() << "le";
        break;
    case L:
        out() << "lt";
        break;
    case GE:
        out() << "ge";
        break;
    case G:
        out() << "gt";
        break;
    default:
        break;
    }
}

ostream &shift_type::out()
{
    return parent->out();
}

ostream &m_operand::out()
{
    return parent->out();
}

ostream &m_instruction::out()
{
    return parent->out();
}

ostream &m_block::out()
{
    return parent->out();
}

ostream &m_function::out()
{
    return parent->out();
}

ostream &m_unit::out()
{
    return o;
}

bool shift_type::operator<(const shift_type &a) const
{
    if (imm < a.imm)
        return true;
    return type < a.type;
}

bool shift_type::operator==(const shift_type &a) const
{
    return imm == a.imm && type == a.type;
}
