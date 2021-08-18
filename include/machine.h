#ifndef __MACHINE_H__
#define __MACHINE_H__
#include <vector>
#include <set>
#include <list>
#include "symtab.h"

class m_operand;
class m_instruction;
class m_block;
class m_function;
class m_unit;
class shift_type
{
private:
    int imm;
    int type;
public:
    m_operand* parent;
    bool operator<(const shift_type&)const;
    bool operator==(const shift_type&)const;
    enum{NONE, ASR, LSL, LSR, ROR, RRX};
    shift_type(int type = 0, int imm = 0);
    bool isnone(){return type == NONE;}
    void gen_machine_code();
    int get_imm() {return imm;};
    ostream& out();
};
class m_operand
{
private:
    int type;
    int reg_no;
    int imm_val;
    string label;
    
    shift_type* shift;
    bool minus; // for access inst
    m_instruction *parent;
protected:
    void gen_reg_code();
public:
    enum {IMM, REG, VREG, LABEL, DREG, QREG};
    bool operator < (const m_operand&) const;
    bool operator == (const m_operand&) const;
    m_operand(int type, int val = -1, string label = "", int s_type = 0, int imm = 0, bool minus = false);
    m_instruction* get_parent() {return parent;};
    void set_parent(m_instruction*p) {this->parent = p;};
    bool isimm() {return this->type == IMM;}
    bool isbigimm() {return this->type == IMM && (this->imm_val > 255 || this->imm_val < -255);}
    bool isreg() {return this->type == REG;}
    bool islabel() {return this->type == LABEL;}
    bool isvreg() {return this->type == VREG;}
    void set_reg(int reg) {reg_no = reg;type = REG;}
    void setval(int val) {imm_val = val;}
    int get_reg() {return reg_no;}
    int getval() {return this->imm_val;}
    int getno() {return this->reg_no;}
    shift_type* get_shift() const {return shift;};
    void set_shift(shift_type*s) {shift = s; if(s) s->parent = this;};
    bool need_color();
    void gen_machine_code();
    ostream& out();
};

class m_instruction
{
protected:
    int cond;
    int op;
    int type;
    m_block *parent;
public:
    enum{BINARY, ACCESS, MOV, BRANCH, CMP, STACK, TST, MRS, IT, FUSED_BINARY, FUSED_CMP, VACCESS, VMOV};
    bool isMov() const {return type == MOV;};
    bool isPush() const {return type == STACK && op == 0;};
    bool isBranch() const {return type == BRANCH;};
    bool isBinary() const {return type == BINARY;};
    bool isCmp() const {return type == CMP;};
    bool isMemAccess() const {return type == ACCESS;};
    // bool isLdr() const {return type == ACCESS && op == 0;};
    m_block* get_parent() {return parent;};
    void set_parent(m_block*bb) {parent = bb;};
    void insert_after(m_instruction*);
    void insert_before(m_instruction*);
    int getType() {return type;};
    int getOP(){return op;};
    void setCond(int c) {cond = c;};
    void gen_cond(bool flag = false);
    virtual void gen_machine_code() = 0;
    virtual std::vector<m_operand*> getDef() = 0;
    virtual std::vector<m_operand*> getUse() = 0;
    virtual std::vector<m_operand*> get_rhs() {return {};};
    virtual m_operand* get_imm_ope(){return nullptr;};
    virtual void replace_def(m_operand*o, m_operand*n){};
    virtual void replace_use(m_operand*o, m_operand*n) = 0;
    virtual int get_exec_time() const = 0;
    ostream& out();
    enum {E, NE, L, GE, G, LE, NONE};
};

class branch_mi : public m_instruction
{
private:
    m_operand* addr_label;
    int to;
    std::vector<m_operand*> def;
    std::vector<m_operand*> use;
public:
    enum {BEQ, BNE, BLT, BGE, BGT, BLE, B, BL, BX, BR};
    branch_mi(int op, m_operand* addr_label);
    void replace_use(m_operand*o, m_operand*n);
    void gen_machine_code();
    void set_block(int bb){to = bb;};
    int get_block(){return to;};
    void set_label(m_operand*lb){addr_label = lb;lb->set_parent(this);};
    m_operand* get_label() {return addr_label;};
    std::vector<m_operand*> getDef();
    std::vector<m_operand*> getUse();
    int get_exec_time() const {return 1;};
};

class cmp_mi : public m_instruction
{
private:
    m_operand* Rd;
    m_operand* Rn;
public:
    cmp_mi(m_operand* Rd, m_operand* Rn);
    void gen_machine_code();
    void replace_use(m_operand*o, m_operand*n);
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>();};
    std::vector<m_operand*> getUse();
    m_operand* get_imm_ope();
    int get_exec_time() const;
};

class binary_mi : public m_instruction
{
private:
    m_operand* Rd;
    m_operand* Rn;
    m_operand* Rm;
public:
    enum {SUB, ADD, MUL, DIV, MOD, AND, OR, LSL, LSR, ASR, RSB};
    binary_mi(int op, m_operand* Rd, m_operand* Rn, m_operand* Rm);
    void set_ope(m_operand* ope) {this->Rm = ope;};
    void replace_def(m_operand*o, m_operand*n);
    void replace_use(m_operand*o, m_operand*n);
    void gen_machine_code();
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({Rd});};
    std::vector<m_operand*> getUse();
    std::vector<m_operand*> get_rhs() {return {Rn, Rm};};
    m_operand* get_imm_ope();
    int get_exec_time() const;
};

class access_mi : public m_instruction
{
private:
    m_operand* Rd;
    m_operand* Rn;
    m_operand* offset;
public:
    enum {LDR, STR};
    access_mi(int op, m_operand* Rd, m_operand* Rn, m_operand* offset = nullptr);
    void replace_def(m_operand*o, m_operand*n);
    void replace_use(m_operand*o, m_operand*n);
    bool is_load() const {return op == LDR;};
    bool is_store() const {return op == STR;};
    void gen_machine_code();
    void add_offset(m_operand*);
    m_operand* get_offset() {return this->offset;};
    std::vector<m_operand*> getDef();
    std::vector<m_operand*> getUse();
    std::vector<m_operand*> get_rhs() { if(offset) return {Rn, offset}; return{Rn};};
    m_operand* get_imm_ope();
    bool is_load_imm() {return op==LDR && Rn->isimm();}
    int get_exec_time() const;
};

class mov_mi : public m_instruction
{
private:
    m_operand* Rd;
    m_operand* Rn;
public:
    enum {MOV, MVN};
    mov_mi(int op, m_operand* Rd, m_operand* Rn);
    void replace_def(m_operand*o, m_operand*n);
    void replace_use(m_operand*o, m_operand*n);
    void gen_machine_code();
    bool is_pure_mov() const;
    std::vector<m_operand*> get_rhs() {return {Rn};};
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({Rd});};
    std::vector<m_operand*> getUse();
    int get_exec_time() const {return 1;};
};

class stack_mi : public m_instruction
{
private:
    vector<m_operand*> reg_list;
public:
    enum {PUSH, POP};
    stack_mi(int op, m_operand* Rn);
    stack_mi(int op, vector<m_operand*> list);
    void replace_use(m_operand*o, m_operand*n);
    void insert_reg(m_operand* reg);
    void set_reg_list(vector<m_operand*> a);
    int get_reg_size() const {return reg_list.size();};
    void gen_machine_code();
    std::vector<m_operand*> getDef();
    std::vector<m_operand*> getUse();
    int get_exec_time() const;
};

class test_mi : public m_instruction
{
private:
    m_operand* Rn;
    m_operand* Rm;
    
public:
    test_mi(m_operand* , m_operand*);
    void gen_machine_code();
    void replace_use(m_operand*o, m_operand*n);
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({Rn, Rm});};
    int get_exec_time() const;
};

class mrs_mi : public m_instruction
{
private:
    m_operand* Rd;
public:
    mrs_mi(m_operand*);
    void gen_machine_code();
    void replace_use(m_operand*o, m_operand*n){};
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({Rd});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({});};
    int get_exec_time() const {return 1;};
};

class it_mi : public m_instruction
{
public:
    it_mi(int flag);
    void gen_machine_code();
    void replace_use(m_operand*o, m_operand*n){};
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({});};
    int get_exec_time() const {return 1;};
};

class fused_binary : public m_instruction
{
private:
    m_operand* Rd;
    m_operand* Rm;
    m_operand* Rn;
    m_operand* Ra;
public:
    fused_binary(int op, m_operand* Rd, m_operand* Rm, m_operand* Rn, m_operand* Ra);
    void gen_machine_code();
    void replace_use(m_operand*o, m_operand*n);
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({Rd});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({Rm, Rn, Ra});};
    int get_exec_time() const {return 3;};
    enum{MLA, MLS};
};

class fused_cmp : public m_instruction
{
private:
    m_operand* Rn;
    m_operand* addr_label;
public:
    fused_cmp(int op, m_operand* Rn, m_operand* addr_label);
    void gen_machine_code();
    void replace_use(m_operand* o, m_operand* n);
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({Rn});};
    int get_exec_time() const {return 1;};
    enum{CBZ, CBNZ};
};

class vaccess : public m_instruction
{
private:
    vector<m_operand*> reg_list;
    m_operand* addr;
public:
    vaccess(int op, vector<m_operand*> list, m_operand* addr);
    void gen_machine_code();
    void replace_use(m_operand* o, m_operand* n);
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({addr});};
    int get_exec_time() const;
    enum{VLDR, VSTR};
};

class vmov : public m_instruction
{
private:
    m_operand* Rd;
    m_operand* Rn;
public:
    vmov(m_operand* Rd, m_operand* Rn);
    void gen_machine_code();
    void replace_use(m_operand* o, m_operand* n){return;};
    std::vector<m_operand*> getDef() {return std::vector<m_operand*>({});};
    std::vector<m_operand*> getUse() {return std::vector<m_operand*>({});};
    int get_exec_time() const {return 1;};
};

class m_block
{
private:
    int no;
    std::vector<m_block *> pred, succ;
    std::list<m_instruction*> inst_list;
    std::set<m_operand*> live_in;
    std::set<m_operand*> live_out;
    std::set<m_operand*> def_in;
    std::set<m_operand*> def_out;
    m_function* parent;
public:
    void insert_front(m_instruction* inst);
    void insert_inst(m_instruction* inst);
    void insert_after(m_instruction*, m_instruction*);
    void insert_before(m_instruction*, m_instruction*);
    void gen_machine_code();
    void remove(m_instruction*);
    int get_no(){return no;};
    std::list<m_instruction*>::iterator get_last_nonbranch_inst();
    std::list<m_instruction*>& get_inst_list(){return inst_list;};
    void add_pred(m_block * p) {this->pred.push_back(p);};
    void add_succ(m_block* s) {this->succ.push_back(s);};
    std::vector<m_block *>& get_pred() {return pred;};
    std::vector<m_block*>& get_succ() {return succ;};
    m_block(int no, m_function*);
    std::set<m_operand*>& get_def_in(){return def_in;};
    std::set<m_operand*>& get_def_out(){return def_out;};
    std::set<m_operand*>& get_live_in(){return live_in;};
    std::set<m_operand*>& get_live_out(){return live_out;};
    m_function* get_parent(){return parent;};
    ostream& out();
};

class m_function
{
private:
    int stack_size = 0;
    std::vector<m_block*> block_list;
    m_block* entry_bb;
    m_block* exit_bb;
    m_unit* parent;
    std::set<int> saved_regs;
    std::vector<m_instruction*> reg_back_patch;
    std::set<m_instruction*> local_backbatch;
    std::set<m_instruction*> param_backbatch;
    std::vector<m_instruction*> sp_backpatch;
    symbol_entry *sym_ptr;
protected:
    void gen_func_entry_code();
public:
    void insert_blk(m_block* blk);
    void erase_blk(m_block* blk);
    void set_entry(m_block* entry){ this->entry_bb = entry;};
    void set_exit(m_block* exit) {this->exit_bb = exit;};
    m_block* get_entry() {return this->entry_bb;};
    m_block* get_exit() {return this->exit_bb;};
    symbol_entry* get_sym_ptr() {return sym_ptr;};
    void gen_machine_code();
    std::vector<m_block*> get_block_list(){return block_list;};
    std::set<int> get_saved_regs(){return saved_regs;};
    void add_saved_regs(int reg_no);
    void add_reg_back_patch(m_instruction*inst){reg_back_patch.push_back(inst);};
    void add_local_backpatch(m_instruction* inst){local_backbatch.insert(inst);};
    void add_param_backpatch(m_instruction* inst){param_backbatch.insert(inst);};
    void add_sp_backpatch(m_instruction* inst){sp_backpatch.push_back(inst);};
    void back_patch();
    std::set<m_instruction*> &get_local_backpatch(){return local_backbatch;};
    std::set<m_instruction*> &get_param_backpatch(){return param_backbatch;};
    m_unit* get_parent(){return parent;};
    int get_stack_size(){return stack_size;};
    void set_stack_size(int size){stack_size += size;};
    m_function(symbol_entry *sym_ptr, m_unit*);
    ostream& out();
};

class m_unit
{
private:
    std::vector<m_function*> func_list;
    symbol_table *global_decl;
    ostream& o;
    int code_count = 0;
    int max_count = 512;
    int vreg_num = 0;
protected:
    void gen_global_decl_code();
public:
    void set_vreg(int n){vreg_num = n;}
    int assign_vreg(){int val = vreg_num++; return val;}
    void gen_reloc();
    void gen_literal_poll(int n=1);
    void gen_machine_code();
    void insert_func(m_function* func);
    void back_patch();
    std::vector<m_function*> get_func_list(){return func_list;};
    m_unit(symbol_table *global_decl, ostream& o);
    ostream& out();
};

#endif