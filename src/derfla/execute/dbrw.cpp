#include "stdafx.h"
#include "dbrw.h"

DBRW *DBRW::instance_ = nullptr;

DBRW* DBRW::instance()
{
    if (!instance_)
        instance_ = new DBRW;
    return instance_;
}

void DBRW::destroy()
{
    delete instance_;
    instance_ = nullptr;
}

DBRW::DBRW()
{

}

DBRW::~DBRW()
{

}
