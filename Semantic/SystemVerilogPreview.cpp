/*
MIT License

Copyright (c) 2022 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#include "SystemVerilogPreview.h"

using namespace simodo;

namespace sv
{
    SystemVerilogPreview::SystemVerilogPreview(interpret::ModuleManagement_interface & module_management)
        : SystemVerilogSemantics_abstract(module_management)
    {
    }

    bool SystemVerilogPreview::checkInterpretType(interpret::InterpretType interpret_type) const
    {
        return interpret_type == interpret::InterpretType::Preview;
    }

}