#include "PartialRedundancyElimination.h"
#include <algorithm>
#include <sstream>

void PartialRedundancyElimination::pass()
{
    for (auto &f : unit->get_func_list())
    {
        func = f;
        init();
        insertEmptyBlock();
        computeOprdExpr();
        computeUseKill();
        computeAnticipated();
        computeAvailable();
        computeEarliest();
        computePostponable();
        computeLatest();
        computeUsed();
        modifyCode();
    }
}

void PartialRedundancyElimination::insertEmptyBlock()
{
    std::vector<std::pair<m_block *, m_block *>> worklist;
    for (auto &bb : func->get_block_list())
    {
        for (auto &succ : bb->get_succ())
        {
            if (succ->get_pred().size() > 1)
                worklist.push_back({bb, succ});
        }
    }
    while (!worklist.empty())
    {
        m_block *u = worklist.back().first;
        m_block *v = worklist.back().second;
        worklist.pop_back();
        m_block *insert_bb = new m_block(symbol_table::gen_label(1), func);
        func->insert_blk(insert_bb);
        u->get_succ().push_back(insert_bb);
        insert_bb->get_pred().push_back(u);
        v->get_pred().push_back(insert_bb);
        insert_bb->get_succ().push_back(v);
        u->get_succ().erase(std::find(u->get_succ().begin(), u->get_succ().end(), v));
        v->get_pred().erase(std::find(v->get_pred().begin(), v->get_pred().end(), u));
        std::ostringstream buf;
        buf << ".L" << v->get_no();
        m_operand *addr_label = new m_operand(m_operand::LABEL, -1, buf.str());
        branch_mi *br = new branch_mi(branch_mi::B, addr_label);
        br->set_block(v->get_no());
        insert_bb->insert_inst(br);
        if (u->get_inst_list().empty())
            continue;
        auto inst = *u->get_inst_list().rbegin();
        if (!inst->isBranch())
            continue;
        br = dynamic_cast<branch_mi *>(inst);
        std::ostringstream buf2;
        buf2 << ".L" << insert_bb->get_no();
        addr_label = new m_operand(m_operand::LABEL, -1, buf2.str());
        if (br->get_block() == v->get_no())
        {
            br->set_block(insert_bb->get_no());
            br->set_label(addr_label);
        }
        int l = u->get_inst_list().size();
        if (l < 2)
            continue;
        auto it = u->get_inst_list().rbegin();
        it++;
        inst = *it;
        if (!inst->isBranch())
            continue;
        br = dynamic_cast<branch_mi *>(inst);
        if (br->get_block() == v->get_no())
        {
            br->set_block(insert_bb->get_no());
            br->set_label(addr_label);
        }
    }
}

void PartialRedundancyElimination::computeOprdExpr()
{
    for (auto &bb : func->get_block_list())
    {
        for (auto &inst : bb->get_inst_list())
        {
            if (!inst->isBinary() && !inst->isMemAccess())
                continue;
            if(inst->isMemAccess() && !dynamic_cast<access_mi*>(inst)->is_load())
                continue;
            auto rhs = inst->get_rhs();
            Expression expr;
            std::get<0>(expr) = inst->getType();
            std::get<1>(expr) = inst->getOP();
            for (auto &t : rhs)
                std::get<2>(expr).push_back(*t);
            if (rhs.back()->get_shift() == nullptr)
                std::get<3>(expr) = shift_type();
            else
                std::get<3>(expr) = *rhs.back()->get_shift();
            worklist.push_back(inst);
            expr2inst[expr] = inst;
            expressions.insert(expr);
            for (auto &t : rhs)
            {
                m_operand oprd(*t);
                oprd.set_shift(nullptr);
                oprd_expr[oprd].insert(expr);
            }
        }
    }
}

void PartialRedundancyElimination::computeUseKill()
{
    for (auto &bb : func->get_block_list())
    {
        auto &il = bb->get_inst_list();
        for (auto &inst : il)
        {
            if (inst->isBinary() || (inst->isMemAccess() && dynamic_cast<access_mi*>(inst)->is_load()))
            {
                auto rhs = inst->get_rhs();
                Expression expr;
                std::get<0>(expr) = inst->getType();
                std::get<1>(expr) = inst->getOP();
                for (auto &t : rhs)
                    std::get<2>(expr).push_back(*t);
                if (rhs.back()->get_shift() == nullptr)
                    std::get<3>(expr) = shift_type();
                else
                    std::get<3>(expr) = *rhs.back()->get_shift();
                if (kill[bb].find(expr) == kill[bb].end())
                    use[bb].insert(expr);
            }
            for (auto &def : inst->getDef())
                kill[bb].insert(oprd_expr[*def].begin(), oprd_expr[*def].end());
        }
    }
}

void PartialRedundancyElimination::computeAnticipated()
{
    for (auto &bb : func->get_block_list())
        anticipated_in[bb] = expressions;
    anticipated_in[func->get_exit()].clear();
    bool change;
    change = true;
    while (change)
    {
        change = false;
        for (auto &bb : func->get_block_list())
        {
            if (bb == func->get_exit())
                continue;
            anticipated_out[bb] = expressions;
            auto old = anticipated_in[bb];
            for (auto &succ : bb->get_succ())
            {
                std::set<Expression> t;
                set_intersection(anticipated_in[succ].begin(), anticipated_in[succ].end(), anticipated_out[bb].begin(), anticipated_out[bb].end(), inserter(t, t.end()));
                anticipated_out[bb] = t;
            }
            anticipated_in[bb] = use[bb];
            set_difference(anticipated_out[bb].begin(), anticipated_out[bb].end(), kill[bb].begin(), kill[bb].end(), inserter(anticipated_in[bb], anticipated_in[bb].end()));
            if (anticipated_in[bb] != old)
                change = true;
        }
    }
}

void PartialRedundancyElimination::computeAvailable()
{
    for (auto &bb : func->get_block_list())
        available_out[bb] = expressions;
    available_out[func->get_entry()].clear();
    bool change;
    change = true;
    while (change)
    {
        change = false;
        for (auto &bb : func->get_block_list())
        {
            if (bb == func->get_entry())
                continue;
            available_in[bb] = expressions;
            auto old = available_out[bb];
            for (auto &pred : bb->get_pred())
            {
                std::set<Expression> t;
                set_intersection(available_out[pred].begin(), available_out[pred].end(), available_in[bb].begin(), available_in[bb].end(), inserter(t, t.end()));
                available_in[bb] = t;
            }
            available_out[bb] = anticipated_in[bb];
            available_out[bb].insert(available_in[bb].begin(), available_in[bb].end());
            for (auto &k : kill[bb])
            {
                if (available_out[bb].find(k) != available_out[bb].end())
                    available_out[bb].erase(k);
            }
            if (old != available_out[bb])
                change = true;
        }
    }
}

void PartialRedundancyElimination::computeEarliest()
{
    for (auto &bb : func->get_block_list())
        set_difference(anticipated_in[bb].begin(), anticipated_in[bb].end(), available_in[bb].begin(), available_in[bb].end(), inserter(earliest[bb], earliest[bb].end()));
}

void PartialRedundancyElimination::computePostponable()
{
    for (auto &bb : func->get_block_list())
        postponable_out[bb] = expressions;
    postponable_out[func->get_entry()].clear();
    bool change;
    change = true;
    while (change)
    {
        change = false;
        for (auto &bb : func->get_block_list())
        {
            if (bb == func->get_entry())
                continue;
            postponable_in[bb] = expressions;
            auto old = postponable_out[bb];
            for (auto &pred : bb->get_pred())
            {
                std::set<Expression> t;
                set_intersection(postponable_out[pred].begin(), postponable_out[pred].end(), postponable_in[bb].begin(), postponable_in[bb].end(), inserter(t, t.end()));
                postponable_in[bb] = t;
            }
            postponable_out[bb] = earliest[bb];
            postponable_out[bb].insert(postponable_in[bb].begin(), postponable_in[bb].end());
            for (auto &k : use[bb])
            {
                if (postponable_out[bb].find(k) != postponable_out[bb].end())
                    postponable_out[bb].erase(k);
            }
            if (old != postponable_out[bb])
                change = true;
        }
    }
}

void PartialRedundancyElimination::computeLatest()
{
    for (auto &bb : func->get_block_list())
    {
        auto t1 = earliest[bb];
        t1.insert(postponable_in[bb].begin(), postponable_in[bb].end());
        auto t2 = use[bb];
        std::set<Expression> t3 = expressions;
        for (auto &succ : bb->get_succ())
        {
            auto t4 = earliest[succ];
            t4.insert(postponable_in[succ].begin(), postponable_in[succ].end());
            std::set<Expression> t5;
            set_intersection(t3.begin(), t3.end(), t4.begin(), t4.end(), inserter(t5, t5.end()));
            t3 = t5;
        }
        set_difference(expressions.begin(), expressions.end(), t3.begin(), t3.end(), inserter(t2, t2.end()));
        set_intersection(t1.begin(), t1.end(), t2.begin(), t2.end(), inserter(latest[bb], latest[bb].end()));
    }
}

void PartialRedundancyElimination::computeUsed()
{
    bool change;
    change = true;
    while (change)
    {
        change = false;
        for (auto &bb : func->get_block_list())
        {
            if (bb == func->get_exit())
                continue;
            used_out[bb].clear();
            auto old = used_in[bb];
            for (auto &succ : bb->get_succ())
                used_out[bb].insert(used_in[succ].begin(), used_in[succ].end());
            used_in[bb] = use[bb];
            used_in[bb].insert(used_out[bb].begin(), used_out[bb].end());
            for (auto &e : latest[bb])
                if (used_in[bb].find(e) != used_in[bb].end())
                    used_in[bb].erase(e);
            if (used_in[bb] != old)
                change = true;
        }
    }
}

void PartialRedundancyElimination::modifyCode()
{
    std::map<Expression, m_operand *> e2o;
    for (auto &bb : func->get_block_list())
    {
        std::set<Expression> insert_exprs;
        set_intersection(latest[bb].begin(), latest[bb].end(), used_out[bb].begin(), used_out[bb].end(), inserter(insert_exprs, insert_exprs.end()));
        for (auto &expr : insert_exprs)
        {
            m_instruction *inst = nullptr;
            m_operand *Rd, *Rn, *Rm;
            if (e2o.find(expr) == e2o.end())
            {
                m_operand *o = new m_operand(m_operand::VREG, unit->assign_vreg());
                e2o[expr] = o;
            }
            Rd = new m_operand(*e2o[expr]);
            int type = std::get<0>(expr);
            if (type == m_instruction::MOV)
            {
                Rn = new m_operand(*std::get<2>(expr).begin());
                inst = new mov_mi(std::get<1>(expr), Rd, Rn);
            }
            else if (type == m_instruction::BINARY)
            {
                Rn = new m_operand(std::get<2>(expr)[0]);
                Rm = new m_operand(std::get<2>(expr)[1]);
                inst = new binary_mi(std::get<1>(expr), Rd, Rn, Rm);
            }
            else if (type == m_instruction::ACCESS)
            {
                Rn = new m_operand(std::get<2>(expr)[0]);
                Rm = nullptr;
                if (std::get<2>(expr).size() > 1)
                    Rm = new m_operand(std::get<2>(expr)[1]);
                inst = new access_mi(std::get<1>(expr), Rd, Rn, Rm);
            }
            bb->insert_front(inst);
            std::set<m_instruction *> &local = func->get_local_backpatch();
            std::set<m_instruction *> &param = func->get_param_backpatch();
            if (local.find(expr2inst[expr]) != local.end())
                func->add_local_backpatch(inst);
            if (param.find(expr2inst[expr]) != param.end())
                func->add_param_backpatch(inst);
        }
    }
    for (auto &inst : worklist)
    {
        auto rhs = inst->get_rhs();
        m_block *bb = inst->get_parent();
        if (func->get_exit() == bb)
            continue;
        Expression expr;
        std::get<0>(expr) = inst->getType();
        std::get<1>(expr) = inst->getOP();
        for (auto &t : rhs)
            std::get<2>(expr).push_back(*t);
        if (rhs.back()->get_shift() == nullptr)
            std::get<3>(expr) = shift_type();
        else
            std::get<3>(expr) = *rhs.back()->get_shift();
        if (use[bb].find(expr) == use[bb].end())
            continue;
        bool f1, f2;
        f1 = latest[bb].find(expr) == latest[bb].end();
        f2 = used_out[bb].find(expr) == used_out[bb].end();
        if (!f1 && f2)
            continue;
        m_operand *Rd, *Rn;
        mov_mi *copy;
        Rd = new m_operand(**inst->getDef().begin());
        Rn = new m_operand(*e2o[expr]);
        copy = new mov_mi(mov_mi::MOV, Rd, Rn);
        bb->insert_before(copy, inst);
        bb->remove(inst);
    }
}

void PartialRedundancyElimination::init()
{
    worklist.clear();
    expressions.clear();
    expr2inst.clear();
    oprd_expr.clear();
    anticipated_in.clear();
    anticipated_out.clear();
    available_in.clear();
    available_out.clear();
    postponable_in.clear();
    postponable_out.clear();
    used_in.clear();
    used_out.clear();
    earliest.clear();
    latest.clear();
    use.clear();
    kill.clear();
}