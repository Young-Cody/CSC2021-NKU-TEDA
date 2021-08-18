#include <iostream>
#include <fstream>
#include "ast.h"
#include "Mem2Reg.h"
#include "IRSemanticAnalysis.h"
#include "DeadCodeElimination.h"
#include "LoopUnrolling.h"
#include "AutomaticInlining.h"
#include "SSCP.h"
#include "GenMachine.h"
#include "PartialRedundancyElimination.h"
#include "RegisterAllocation.h"
#include "SSADestruction.h"
#include "StrengthReduction.h"
#include "MachineStrenthReduction.h"
#include "PeepholeOptimization.h"
#include "ArrayOptimization.h"
#include "mdce.h"
#include "CopyPropagation.h"
#include "ArrayOptimization.h"
#include "Vectorization.h"
#include "InstructionScheduling.h"
using namespace std;

Ast *ast;
extern FILE *yyin;

int yyparse();

char output_file[256] = "a.out";
char input_file[256];
bool gen_IR;
bool gen_arm = true;
bool gen_ast;
bool optimize;

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        cerr << "no input file\n";
        exit(-1);
    }
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--ast") == 0)
        {
            gen_arm = false;
            gen_ast = true;
        }  
        else if(strcmp(argv[i], "--arm") == 0)
            gen_arm = true;
        else if(strcmp(argv[i], "--IR") == 0)
        {
            gen_IR = true;
            gen_arm = false;
        }
        else if(strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            sprintf(output_file, "%s", argv[i + 1]);
            i++;
        }   
        else if(strcmp(argv[i], "-S") == 0)
            ;
        else if (strcmp(argv[i], "-O2") == 0)
            optimize = true;
        else
            sprintf(input_file, "%s", argv[i]);
    }

    if(!(yyin = fopen(input_file, "r")))
    {
        cerr<<input_file<<": No such file or directory\nno input file\n";
        exit(-1);
    }

    ofstream fout(output_file, ios_base::out);
    if (fout.fail())
    {
        cerr << "fail to open output file\n";
        exit(-1);
    }
    ast = new Ast(fout);
    yyparse();
    if (gen_ast)
        ast->printAst();
    if (gen_IR)
    {
        Unit *unit = ast->gen_code();
        IRSemAnalysis isa(unit);
        Mem2Reg ssa(unit);
        DeadCodeElimination dce(unit);
        SSCP sscp(unit);
        SSADestruction ssad(unit);
        isa.pass();
        if(optimize)
        {
            ssa.pass();
            sscp.pass();
            dce.pass();
        }
        // ssad.pass();
        unit->print();
    }
    if(gen_arm)
    {
        Unit *unit = ast->gen_code();
        IRSemAnalysis isa(unit);
        isa.pass();
        if(optimize)
        {
            Mem2Reg ssa(unit);
            SSADestruction ssad(unit);
            SSCP sscp(unit);
            DeadCodeElimination dce(unit);
            StrengthReduction sr(unit);
            LoopUnrolling lun(unit);
            Vectorization vec(unit);
            AutomaticInlining ami(unit);
            sr.pass();
            ssa.pass();
            sscp.pass();
            dce.pass();
            ssad.pass();
            vec.pass();
        }
        gen_machine g_machine(unit);
        m_unit* u = g_machine.convert_unit(fout);
        PartialRedundancyElimination pre(u);
        mdce md(u);
        copy_propagation cp(u);
        RegisterAllocation ra(u);
        strength_reduction sr(u);
        peephole_optimization po(u);
        array_optimization ao(u);
        InstructionScheduling inst_sched(u);
        if(optimize)
        {
            ao.pass();
            sr.pass();
            cp.pass();
            md.pass();
            po.pass();
            pre.pass();
            cp.pass();
            md.pass();
        }
        ra.allocateRegisters();
        u->back_patch();
        if(optimize)
        {
            ao.pass();
            sr.pass();
            po.pass();
            cp.pass();
            md.pass();
            inst_sched.pass();
        }
        u->gen_machine_code();
    }
    fout.close();
    return 0;
}
