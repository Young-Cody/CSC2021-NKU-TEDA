#ifndef __GENMACHINE_H__
#define __GENMACHINE_H__
#include <sstream>
#include <unordered_map>
#include "operand.h"
#include "instruction.h"
#include "basicblock.h"
#include "function.h"
#include "unit.h"
#include "machine.h"
#include "symtab.h"
#include "typesys.h"

class gen_machine
{
private:
    Function* cur_func = nullptr;
    int vreg_num = 16;
    unordered_map<string, int> vreg_map;
    Unit* unit;
    void convert_func(m_function*& m_func, Function* func, m_unit*);
    void convert_block(m_block*& m_bb, BasicBlock* bb, m_function*);

    void convert_binary_inst(m_block*& m_bb, BinaryInstruction* inst);
    void convert_unary_inst(m_block*& m_bb, UnaryInstruction* inst);
    void convert_copy_inst(m_block*& m_bb, CopyInstruction* inst);
    void convert_branch_inst(m_block*& m_bb, BranchInstruction* inst);
    void convert_cmp_if_inst(m_block*& m_bb, CmpInstruction* cmp_inst, IfInstruction* if_inst);
    void convert_cmp_inst(m_block*& m_bb, CmpInstruction* inst);
    void convert_ret_inst(m_block*& m_bb, RetInstruction* inst);
    void convert_call_inst(m_block*& m_bb, CallInstruction* inst);
    void convert_load_inst(m_block*& m_bb, LoadInstruction* inst);
    void convert_store_inst(m_block*& m_bb, StoreInstruction* inst);
    void convert_addr_inst(m_block*& m_bb, AddrInstruction* inst);
    void convert_alloca_inst(m_block*& m_bb, AllocaInstruction* inst);

    m_instruction* gen_mov_mi(int op, m_operand* Rd, m_operand* Rm);
    m_instruction* gen_binary_mi(int op, m_operand* Rd, m_operand* Rn, m_operand* Rm);
    m_instruction* gen_cmp_mi(m_operand* Rn, m_operand* Rm);
    m_instruction* gen_branch_mi(int op, m_operand* addr_label);
    m_instruction* gen_access_mi(int op, m_operand* Rd, m_operand* Rn, m_operand* offset = nullptr);
    m_instruction* gen_stack_mi(int op, m_operand* Rn);
    m_instruction* gen_stack_mi(int op, vector<m_operand*>);
    m_instruction* gen_test_mi(m_operand* Rn, m_operand* Rm);
    m_instruction* gen_mrs_mi(m_operand* Rd);
    m_instruction* gen_it_mi(int cond);
    m_instruction* gen_vec_access(int op, vector<m_operand*> list, m_operand* base);
    m_instruction* gen_vec_mov(m_operand* Rd, m_operand* Rn);
    m_operand* gen_operand(Operand* operand, m_block* m_bb, int s_type = shift_type::NONE, int s_val = 0);
    m_operand* gen_reg_operand(int reg_no, int type = m_operand::VREG, int s_type = shift_type::NONE, int s_val = 0);
    m_operand* gen_imm_operand(int val, m_block* m_bb);
    m_operand* gen_label_operand(string label);

    m_operand* load_imm(m_block*& m_bb, m_operand* ope);
    enum {BINARY, UNARY, COPY, BRANCH, IF, RET, CALL, LOAD, STORE, CMP, ADDR, ALLOCA, PHI};


public:
    gen_machine(Unit* unit);
    m_unit* convert_unit(ostream& out);

};

#endif