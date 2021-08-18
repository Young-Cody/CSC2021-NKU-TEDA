#include "mdce.h"
#include "LiveVariableAnalysis.h"
#include <algorithm>

void mdce::pass()
{
    for (auto &f : unit->get_func_list())
    {
        func = f;
        run();
    }
}

void mdce::run()
{
    LiveVariableAnalysis lva;
    lva.pass(func);
    std::vector<m_instruction *> worklist;
    for (auto &bb : func->get_block_list())
    {
        std::map<m_operand, std::set<m_operand *>> livenow;
        for (auto &t : bb->get_live_out())
            livenow[*t].insert(t);
        auto &l = bb->get_inst_list();
        for (auto inst = l.rbegin(); inst != l.rend(); inst++)
        {
            if ((*inst)->isMov() && dynamic_cast<mov_mi *>(*inst)->is_pure_mov())
            {
                if (!(*inst)->getUse().empty())
                {
                    m_operand *Rd = (*inst)->getDef()[0];
                    m_operand *Rn = (*inst)->getUse()[0];
                    if (Rd->get_reg() == Rn->get_reg())
                        worklist.push_back(*inst);
                }
            }
            auto defs = (*inst)->getDef();
            if (!defs.empty())
            {
                m_operand *def;
                def = *defs.begin();
                if (livenow[*def].empty())
                    worklist.push_back(*inst);
            }
            for (auto &def : defs)
            {
                for (auto &t : lva.getUsePos()[*def])
                {
                    if (livenow[*def].find(t) != livenow[*def].end())
                        livenow[*def].erase(t);
                }
            }
            auto users = (*inst)->getUse();
            for (auto &user : users)
                livenow[*user].insert(user);
        }
    }
    for (auto &list : worklist)
    {
        if (!list->isBranch() && list->getType() != m_instruction::STACK)
            list->get_parent()->remove(list);
    }
    std::vector<m_block *> delete_list;
    for (auto &bb : func->get_block_list())
    {
        if (bb->get_inst_list().size() != 1)
            continue;
        m_instruction *inst;
        inst = *bb->get_inst_list().begin();
        if (!inst->isBranch() || inst->getOP() == branch_mi::BX)
            continue;
        branch_mi *br = dynamic_cast<branch_mi *>(inst);
        delete_list.push_back(bb);
        for (auto &pred : bb->get_pred())
        {
            pred->add_succ(bb->get_succ()[0]);
            bb->get_succ()[0]->add_pred(pred);
            if (pred->get_inst_list().empty())
                continue;
            m_instruction *inst1 = pred->get_inst_list().back();
            if (!inst1->isBranch())
                continue;
            branch_mi *br1 = dynamic_cast<branch_mi *>(inst1);
            if (br1->get_block() == bb->get_no())
            {
                br1->set_block(br->get_block());
                br1->set_label(br->get_label());
            }
            if (pred->get_inst_list().size() <= 1)
                continue;
            auto it = pred->get_inst_list().rbegin();
            it++;
            inst1 = *it;
            if (!inst1->isBranch())
                continue;
            br1 = dynamic_cast<branch_mi *>(inst1);
            if (br1->get_block() == bb->get_no())
            {
                br1->set_block(br->get_block());
                br1->set_label(br->get_label());
            }
        }
    }

    for (auto bb : delete_list)
    {
        for(auto &pred:bb->get_pred())
            pred->get_succ().erase(std::find(pred->get_succ().begin(), pred->get_succ().end(), bb));
        for(auto &succ:bb->get_succ())
            succ->get_pred().erase(std::find(succ->get_pred().begin(), succ->get_pred().end(), bb));
        func->erase_blk(bb);
    }
}
