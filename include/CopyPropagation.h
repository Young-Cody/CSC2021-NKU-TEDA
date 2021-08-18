#ifndef __COPY_OPTIMIZATION_H
#define __COPY_OPTIMIZATION_H

#include "machine.h"

class copy_propagation
{
private:
    m_unit* unit;
    void pass(m_function*);
public:
    copy_propagation(m_unit* u) {unit = u;};
    void pass();
};

#endif