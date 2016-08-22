#include "stdafx.h"
#include "dbrw.h"

DBRW *DBRW::instance_ = nullptr;

DBRW* DBRW::instance()
{
    if (!instance_)
        new DBRW;
    return instance();
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