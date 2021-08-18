#ifndef __STRENGTHREDUCTION_H__
#define __STRENGTHREDUCTION_H__

#include "machine.h"
class strength_reduction
{
private:
    m_unit* unit;
public:
    strength_reduction(m_unit* u);
    void pass();
};
#endif