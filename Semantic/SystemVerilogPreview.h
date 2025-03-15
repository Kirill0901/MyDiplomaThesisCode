/*
MIT License

Copyright (c) 2022 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#ifndef simodo_sbl_HostPreview
#define simodo_sbl_HostPreview

/*! \file HostPreview.h
    \brief Интерпретация операторов SystemVeriolog внутренней машиной SIMODO
*/

#include "SystemVerilogSemantics_abstract.h"

namespace sv
{
    class SystemVerilogPreview: public SystemVerilogSemantics_abstract
    {
    public:
        SystemVerilogPreview(simodo::interpret::ModuleManagement_interface & module_management);

    public:
        virtual bool    checkInterpretType(simodo::interpret::InterpretType interpret_type) const override;
    };

}

#endif // SystemVeriolog_HostPreview
