#ifndef _SSCP_H__
#define _SSCP_H__

#include "pass.h"
class Function;

class SSCP : public Pass
{
private:
    void constantPropagation(Function *);

public:
    SSCP(Unit *);
    void pass();
};

#endif