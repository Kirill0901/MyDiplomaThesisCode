/*
MIT License

Copyright (c) 2022 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#include "SystemVerilogSemantics_abstract.h"
#include "simodo/interpret/AnalyzeException.h"
#include "simodo/bormental/DrBormental.h"
#include "simodo/inout/convert/functions.h"
#include "simodo/inout/format/fmt.h"

#include "simodo/interpret/StackOfNames.h"

#include <limits>
#include <cassert>
#include <cmath>
#include <float.h>

using namespace simodo;

namespace sv
{
    SystemVerilogSemantics_abstract::SystemVerilogSemantics_abstract(interpret::ModuleManagement_interface & module_management)
        : _module_management(module_management)
    {
    }

    SystemVerilogSemantics_abstract::~SystemVerilogSemantics_abstract()
    {
    }

    void SystemVerilogSemantics_abstract::reset()
    {
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::before_start() 
    { 
        return interpret::InterpretState::Flow; 
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::before_finish(interpret::InterpretState state) 
    { 
        return state; 
    }

    bool SystemVerilogSemantics_abstract::isOperationExists(ast::OperationCode operation_code) const
    {
        SystemVerilogOperationCode operation = static_cast<SystemVerilogOperationCode>(operation_code);

        switch(operation)
        {
        case SystemVerilogOperationCode::None:
        case SystemVerilogOperationCode::PushConst:
        case SystemVerilogOperationCode::Assign:
        case SystemVerilogOperationCode::Equal:
        case SystemVerilogOperationCode::NotEqual:
        case SystemVerilogOperationCode::Less:
        case SystemVerilogOperationCode::LessOrEqual:
        case SystemVerilogOperationCode::More:
        case SystemVerilogOperationCode::MoreOrEqual:
            return true;
        default:
            return false;
        }
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::performOperation(const ast::Node & op) 
    {
        SystemVerilogOperationCode operation = static_cast<SystemVerilogOperationCode>(op.operation());

        switch(operation)
        {
        case SystemVerilogOperationCode::None:
            return executeNone();
        case SystemVerilogOperationCode::PushConst:
            return executePushConst(op);
        case SystemVerilogOperationCode::Assign:
            return executeAssign(op);
        case SystemVerilogOperationCode::Equal:
        case SystemVerilogOperationCode::NotEqual:
        case SystemVerilogOperationCode::Less:
        case SystemVerilogOperationCode::LessOrEqual:
        case SystemVerilogOperationCode::More:
        case SystemVerilogOperationCode::MoreOrEqual:
            return executeCompare(op);
        default:
            break;
        }

        throw bormental::DrBormental("SystemVerilogSemantics_abstract::performOperation", 
                            inout::fmt("Invalid operation index %1")
                                .arg(op.operation()));
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::executeNone()
    {
        return interpret::InterpretState::Flow;
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::executePushConst(const simodo::ast::Node & op)
    {
        assert(_interpret);

        variable::Variable   constant_variable;
        [[maybe_unused]] size_t index = 0;
        
        switch(op.token().type())
        {
        case inout::LexemeType::Annotation:
            constant_variable = { u"", op.token().lexeme(), op.token().location() };
            break;
        case inout::LexemeType::Number:
        {
            std::string number_string = inout::toU8(op.token().lexeme());
            if (number_string[0] != '\''){
                if (op.token().qualification() == inout::TokenQualification::Integer)
                    constant_variable = { u"", static_cast<int64_t>(std::stoll(number_string)), op.token().location() };
                else
                    constant_variable = { u"", std::stod(number_string), op.token().location() };
                break;
            }
            char type_of_number = number_string[1];
            number_string = number_string.substr(2);

            switch (type_of_number)
            {
            case 'b':
                constant_variable = { u"", 
                        static_cast<int64_t>(std::stoll(number_string,nullptr,2)), 
                        op.token().location() };
                break;
            case 'o':
                constant_variable = { u"", 
                        static_cast<int64_t>(std::stoll(inout::toU8(op.token().lexeme().substr(2)),nullptr,8)), 
                        op.token().location() };
                break;
            case 'h':
                constant_variable = { u"", 
                        static_cast<int64_t>(std::stoll(inout::toU8(op.token().lexeme().substr(2)),nullptr,16)), 
                        op.token().location() };
                break;
            
            default:
                throw bormental::DrBormental("SystemVerilogSemantics_abstract::executePushConstant", 
                                        inout::fmt("Invalid internal type of constant to convert (%1)")
                                            .arg(getLexemeTypeName(op.token().type())));
            }
            break;
        }
        case inout::LexemeType::Punctuation:
            if (op.token().lexeme() == u"true" || op.token().lexeme() == u"false")
                constant_variable = { u"", op.token().lexeme() == u"true", op.token().location() };
            else if (op.token().lexeme() == u"null") {
                constant_variable = variable::null_variable(op.token().location());
                index = constant_variable.value().variant().index();
            }
            break;
        default:
            throw bormental::DrBormental("SystemVerilogSemantics_abstract::executePushConstant", 
                                        inout::fmt("Invalid internal type of constant to convert (%1)")
                                            .arg(getLexemeTypeName(op.token().type())));
        }

        if (!op.branches().empty()) {
            std::vector<variable::Value> unit_parts;

            for(const ast::Node & n : op.branches())
                unit_parts.push_back(n.token().lexeme());

            constant_variable.spec().set(u"unit", unit_parts);
        }

        stack().push(constant_variable);

        return interpret::InterpretState::Flow;
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::executeAssign(const simodo::ast::Node & )
    {
        assert(_interpret);

        interpret::name_index_t left  = stack().top(1);
        interpret::name_index_t right = stack().top(0);

        variable::Variable & left_var        = stack().variable(left).origin();
        const variable::Variable & right_var = stack().variable(right).origin();

        if (left_var.value().isArray() && right_var.type() == variable::ValueType::Int) {
            std::shared_ptr<variable::Array> array_variable = left_var.value().getArray();
            if (array_variable->dimensions().size() != 1)
                throw bormental::DrBormental("SystemVerilogSemantics_abstract::executeAssign", 
                                            inout::fmt("Invalid internal type..."));
            variable::index_t            size = array_variable->dimensions()[0];
            std::vector<variable::Value> array;
            int64_t                      var = right_var.value().getInt();

            for(variable::index_t i=0; i < size; ++i ) {
                array.push_back(var & 1);
                var >>= 1;
            }
            left_var.value() = array;
        }
        else
            left_var.value() = right_var.value().copy();

        stack().pop(2);

        return interpret::InterpretState::Flow;
    }

    interpret::InterpretState SystemVerilogSemantics_abstract::executeCompare(const ast::Node & op)
    {
        interpret::name_index_t top_name_index = stack().top();

        try
        {
            compare(static_cast<SystemVerilogOperationCode>(op.operation()), op.bound().location());
        }
        catch(...)
        {
            stack().pop(stack().top()+1-top_name_index);
            stack().variable(stack().top()) = variable::error_variable().copyVariable();
            throw;
        }

        return interpret::InterpretState::Flow;
    }

    void SystemVerilogSemantics_abstract::compare(SystemVerilogOperationCode opcode, const inout::TokenLocation & location)
    {
        variable::Variable & op1_origin = stack().variable(stack().top(1)).origin();
        variable::Variable & op2_origin = stack().variable(stack().top(0)).origin();

        if (op1_origin.value().isError() || op2_origin.value().isError()) {
            stack().pop(2);
            stack().push(variable::error_variable().copyVariable());
            return;
        }

        variable::Variable op1,
                           op2;

        if ((op1_origin.type() == variable::ValueType::Array && op2_origin.type() == variable::ValueType::Int) ||
            (op1_origin.type() == variable::ValueType::Int && op2_origin.type() == variable::ValueType::Array)){
            
            variable::Variable &array = op1_origin.type() == variable::ValueType::Array ? op1_origin : op2_origin;

            std::shared_ptr<variable::Array> array_variable = array.value().getArray();
            if (array_variable->dimensions().size() != 1)
                throw bormental::DrBormental("SystemVerilogSemantics_abstract::executeAssign", 
                                            inout::fmt("Invalid internal type..."));
                                            
            variable::index_t            size = array_variable->dimensions()[0];
            int64_t                      integer = 0, pow2 = 1;

            for(variable::index_t i=0; i < size; ++i ) {
                integer += array_variable->getValueByIndex(i).getInt() * pow2;
                pow2 <<= 1;
            }
            
            if (op1_origin.value().isArray()){
                op1.value() = variable::Value(integer);
                op2.value() = op2_origin.value();
            }
            else{
                op1.value() = op1_origin.value();
                op2.value() = variable::Value(integer);
            }
        }
        else{
            variable::ValueType target_type = getType4TypeConversion(opcode, op1_origin.type(), op2_origin.type());

            if (target_type == variable::ValueType::Null)
            throw interpret::AnalyzeException("ScriptSemantics_abstract::compare", 
                                    location.makeLocation(inter().files()), 
                                    inout::fmt("For operation %1, the use of types %2 and %3 is not provided")
                                        .arg(getSblOperationCodeName(opcode))
                                        .arg(getValueTypeName(op1_origin.type()))
                                        .arg(getValueTypeName(op2_origin.type())));

            op1 = inter().expr().convertVariable(op1_origin, target_type);
            op2 = inter().expr().convertVariable(op2_origin, target_type);
        }
        
        variable::Value     res;

        switch(opcode)
        {
        case SystemVerilogOperationCode::Equal:
            res = performCompareEqual(op1.value(), op2.value());
            break;
        case SystemVerilogOperationCode::NotEqual:
            res = !performCompareEqual(op1.value(), op2.value()).getBool();
            break;
        case SystemVerilogOperationCode::Less:
            res = performCompareLess(op1.value(), op2.value());
            break;
        case SystemVerilogOperationCode::LessOrEqual:
            res = performCompareLessOrEqual(op1.value(), op2.value());
            break;
        case SystemVerilogOperationCode::More:
            res = !performCompareLessOrEqual(op1.value(), op2.value()).getBool();
            break;
        case SystemVerilogOperationCode::MoreOrEqual:
            res = !performCompareLess(op1.value(), op2.value()).getBool();
            break;
        default:
            break;
        }

        if (res.type() == variable::ValueType::Null)
            throw interpret::AnalyzeException("ScriptSemantics_abstract::compare", 
                                    location.makeLocation(inter().files()), 
                                    inout::fmt("For operation %1, the use of types %2 and %3 is not provided")
                                        .arg(getSblOperationCodeName(opcode))
                                        .arg(getValueTypeName(op1.type()))
                                        .arg(getValueTypeName(op2.type())));

        stack().pop(2);
        stack().push({{}, res, location, {}});
    }

    variable::Value SystemVerilogSemantics_abstract::performCompareEqual(const variable::Value & op1, const variable::Value & op2) const
    {
        switch(op1.type())
        {
        case variable::ValueType::Bool:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getBool() == op2.getBool();
            else
                return variable::ValueType::Bool;
        case variable::ValueType::Int:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getInt() == op2.getInt();
            else
                return variable::ValueType::Bool;
        case variable::ValueType::Float:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
            {
                double val1 = op1.getReal();
                double val2 = op2.getReal();

                // Учёт погрешности вычислений с плавающей точкой.
                // Машинный эпсилон (разница между 1.0 и следующим представимым значением для double)
                // должен быть масштабирован до величины используемых значений и умножен на желаемую
                // точность в ULP (единицы на последнем месте). ULP нужно подбирать в зависимсоти от
                // степени накапливаемой погрешности.
                /// \todo Подобрать оптимальное значение ULP
                return std::abs(val1 - val2) <= DBL_EPSILON * std::abs(val1+val2) * 2;
            }
            else
                return variable::ValueType::Bool;
        case variable::ValueType::String:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getString() == op2.getString();
            else
                return variable::ValueType::Bool;
        default:
            break;
        }

        return {};
    }

    variable::Value SystemVerilogSemantics_abstract::performCompareLess(const variable::Value & op1, const variable::Value & op2) const
    {
        switch(op1.type())
        {
        case variable::ValueType::Int:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getInt() < op2.getInt();
            else
                return variable::ValueType::Bool;
        case variable::ValueType::Float:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getReal() < op2.getReal();
            else
                return variable::ValueType::Bool;
        case variable::ValueType::String:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getString() < op2.getString();
            else
                return variable::ValueType::Bool;
        default:
            break;
        }

        return {};
    }

    variable::Value SystemVerilogSemantics_abstract::performCompareLessOrEqual(const variable::Value & op1, const variable::Value & op2) const
    {
        switch(op1.type())
        {
        case variable::ValueType::Int:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getInt() <= op2.getInt();
            else
                return variable::ValueType::Bool;
        case variable::ValueType::Float:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getReal() <= op2.getReal();
            else
                return variable::ValueType::Bool;
        case variable::ValueType::String:
            if (op1.variant().index() != std::variant_npos && op2.variant().index() != std::variant_npos)
                return op1.getString() <= op2.getString();
            else
                return variable::ValueType::Bool;
        default:
            break;
        }

        return {};
    }

    variable::ValueType SystemVerilogSemantics_abstract::getType4TypeConversion(SystemVerilogOperationCode /*opcode*/, variable::ValueType type1, variable::ValueType type2) const
    {
        if (type1 == type2)
            return type1;

        if ((type1 == variable::ValueType::Int && type2 == variable::ValueType::Float) ||
            (type1 == variable::ValueType::Float && type2 == variable::ValueType::Int))
            return variable::ValueType::Float;

        if (type1 == variable::ValueType::String || type2 == variable::ValueType::String)
            return variable::ValueType::String;

        return variable::ValueType::Null;
    }

}