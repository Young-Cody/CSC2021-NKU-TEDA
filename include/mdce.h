#ifndef __MDCE_H__
#define __MDCE_H__

#include <set>
#include <map>
#include <vector>
#include "machine.h"

class mdce
{
public:
    mdce(m_unit *u){unit = u;};
    void pass();

private:
    m_function *func;
    m_unit *unit;
    void run();
};

#endif