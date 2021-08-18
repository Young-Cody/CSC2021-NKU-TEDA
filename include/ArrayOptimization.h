#ifndef __ARRAY_OPTIMIZATION_H
#define __ARRAY_OPTIMIZATION_H

#include "machine.h"

class array_optimization
{
private:
    m_unit* unit;
    void pass(m_function*);
public:
    array_optimization(m_unit* u);
    void pass();
};

#endif