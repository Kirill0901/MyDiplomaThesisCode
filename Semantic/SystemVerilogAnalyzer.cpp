/*
MIT License

Copyright (c) 2022 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#include "SystemVerilogAnalyzer.h"
#include "simodo/interpret/AnalyzeException.h"
#include "simodo/inout/convert/functions.h"
#include "simodo/inout/format/fmt.h"

using namespace simodo;

namespace sv
{
    namespace 
    {
        interpret::SemanticDataCollector_null null_collector;
    }

    SystemVerilogAnalyzer::SystemVerilogAnalyzer(interpret::ModuleManagement_interface & module_management)
        : SystemVerilogSemantics_abstract(module_management)
        , _collector(null_collector)
    {
    }

    SystemVerilogAnalyzer::SystemVerilogAnalyzer(interpret::ModuleManagement_interface & module_management, 
                            interpret::SemanticDataCollector_interface & collector)
        : SystemVerilogSemantics_abstract(module_management)
        , _collector(collector)
    {
    }

    bool SystemVerilogAnalyzer::checkInterpretType(interpret::InterpretType interpret_type) const
    {
        return interpret_type == interpret::InterpretType::Analyzer;
    }

    interpret::InterpretState SystemVerilogAnalyzer::performOperation(const ast::Node & op)
    {
        try
        {
            return SystemVerilogSemantics_abstract::performOperation(op);
        }
        catch(const interpret::AnalyzeException & e)
        {
            inter().reporter().reportError(e.location(), e.what());
            _number_of_mistakes ++;
        }

        if (_number_of_mistakes >= MAX_NUMBER_OF_MISTAKES) 
            throw interpret::AnalyzeException("ScriptAnalyzer::performOperation", 
                                    op.token().makeLocation(inter().files()), 
                                    inout::fmt("The number of errors has exceeded the allowable limit"));
        
        return interpret::InterpretState::Flow;
    }

    interpret::InterpretState SystemVerilogAnalyzer::before_start()
    {
        return SystemVerilogSemantics_abstract::before_start();
    }

    interpret::InterpretState SystemVerilogAnalyzer::before_finish(interpret::InterpretState state)
    {
        return SystemVerilogSemantics_abstract::before_finish(state);
    }

    // interpret::InterpretState SystemVerilogAnalyzer::executePushConst()
    // {
    //     inter().stack().pop();

    //     return interpret::InterpretState::Flow;
    // }

}