#ifndef __PEEPHOLEOPTIMIZATION_H__
#define __PEEPHOLEOPTIMIZATION_H__

#include "machine.h"
#include <sstream>
class peephole_optimization
{
private:
    m_unit* unit;
public:
    peephole_optimization(m_unit*);
    void pass();

}; 
#endif
