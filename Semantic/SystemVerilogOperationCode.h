/*
MIT License

Copyright (c) 2019 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo
*/

#ifndef simodo_sbl_SystemVerilogOperationCode
#define simodo_sbl_SystemVerilogOperationCode

/*! \file SystemVerilogOperationCode.h
    \brief Мнемокоды операций языка SystemVerilog
*/

#include <string>
#include <cstdint>

namespace sv
{
    inline const std::u16string SYSTEMVERILOG_HOST_NAME = u"sv";

    enum class SystemVerilogOperationCode : uint16_t
    {
        #include "sv.sv"

        // Последняя операция (определяет кол-во операций)
        LastOperation,

        #include "sv_.sv"
    };

    /*!
     * \brief Преобразовывает мнемокод операции в строку
     * \param op Мнемокод операции
     * \return   Строковое представление мнемокода операции
     */
    std::u16string getSblOperationCodeName(SystemVerilogOperationCode op) noexcept;

}

#endif // SystemVeriolog_OperationCode
