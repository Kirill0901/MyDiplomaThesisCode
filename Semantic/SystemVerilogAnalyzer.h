/*
MIT License

Copyright (c) 2022 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#ifndef simodo_sbl_SystemVerilogAnalyzer
#define simodo_sbl_SystemVerilogAnalyzer

/*! \file SystemVerilogAnalyzer.h
    \brief Анализ операторов SystemVerilog с использованием внутренней машины SIMODO
*/

#include "SystemVerilogSemantics_abstract.h"
#include "simodo/interpret/SemanticDataCollector_interface.h"

#include <memory>

namespace sv
{
    inline const size_t MAX_NUMBER_OF_MISTAKES = 5;

    class SystemVerilogAnalyzer: public SystemVerilogSemantics_abstract
    {
        simodo::interpret::SemanticDataCollector_interface & 
                                _collector;
        size_t                  _number_of_mistakes = 0;

    public:
        SystemVerilogAnalyzer(simodo::interpret::ModuleManagement_interface & module_management);
        SystemVerilogAnalyzer(simodo::interpret::ModuleManagement_interface & module_management, 
                    simodo::interpret::SemanticDataCollector_interface & collector);

    // OperationalSemantics_interface
    public:
        virtual bool    checkInterpretType      (simodo::interpret::InterpretType interpret_type) const override;
        virtual simodo::interpret::InterpretState   performOperation        (const simodo::ast::Node & op) override;

    // SemanticsHost_abstract
    public:
        virtual simodo::interpret::InterpretState   before_start            () override;
        virtual simodo::interpret::InterpretState   before_finish           (simodo::interpret::InterpretState state) override;

        // virtual simodo::interpret::InterpretState   executePushConst   () override;
    };
}

#endif // SystemVeriolog_Analyzer
