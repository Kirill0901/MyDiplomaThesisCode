/*
MIT License

Copyright (c) 2019 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo
*/

#include "SystemVerilogOperationCode.h"

namespace sv
{
    std::u16string getSblOperationCodeName(SystemVerilogOperationCode op) noexcept
    {
        std::u16string str;

        switch(op)
        {
        case SystemVerilogOperationCode::None:
            str = u"None";
            break;
        case SystemVerilogOperationCode::PushConst:
            str = u"PushConst";
            break;
        case SystemVerilogOperationCode::Equal:
            str = u"Equal";
            break;
        case SystemVerilogOperationCode::NotEqual:
            str = u"NotEqual";
            break;
        case SystemVerilogOperationCode::Less:
            str = u"Less";
            break;
        case SystemVerilogOperationCode::LessOrEqual:
            str = u"LessOrEqual";
            break;
        case SystemVerilogOperationCode::More:
            str = u"More";
            break;
        case SystemVerilogOperationCode::MoreOrEqual:
            str = u"MoreOrEqual";
            break;
        default:
            str = u"***";
            break;
        }

        return str;
    }
}