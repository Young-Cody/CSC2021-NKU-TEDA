#ifndef __PASS_H__
#define __PASS_H__

class Unit;

class Pass
{
protected:
    Unit *unit;
public:
    Pass(Unit*);
    virtual void pass() = 0;
};


#endif