/*
MIT License

Copyright (c) 2022 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#ifndef simodo_sbl_SemanticsHost_abstract
#define simodo_sbl_SemanticsHost_abstract

/*! \file SemanticsHost_abstract.h
    \brief Класс разбора SystemVeriolog операции, заданной в виде ноды семантического дерева (ast::Node)
*/

#include "SystemVerilogOperationCode.h"

#include "simodo/interpret/ModuleManagement_interface.h"
#include "simodo/interpret/Interpret_interface.h"


namespace sv
{
    class SystemVerilogSemantics_abstract: public simodo::interpret::SemanticModule_interface
    {
        simodo::interpret::ModuleManagement_interface &    _module_management;
        simodo::interpret::Interpret_interface *           _interpret = nullptr;

    public:
        SystemVerilogSemantics_abstract() = delete;
        SystemVerilogSemantics_abstract(simodo::interpret::ModuleManagement_interface & module_management);

        virtual ~SystemVerilogSemantics_abstract();

    public:
        const simodo::interpret::Interpret_interface &inter()         const { return *_interpret; }
        simodo::interpret::Interpret_interface &   inter()                  { return *_interpret; }

        const simodo::interpret::StackOfNames_interface & stack()     const { return _interpret->stack(); }
        simodo::interpret::StackOfNames_interface & stack()                 { return _interpret->stack(); }

    public:
        virtual void            setInterpret(simodo::interpret::Interpret_interface * inter)       override { _interpret = inter; }
        virtual void            reset()                                                            override;
        virtual simodo::interpret::InterpretState  before_start()                                  override;
        virtual simodo::interpret::InterpretState  before_finish(simodo::interpret::InterpretState state) override;
        virtual bool            isOperationExists(simodo::ast::OperationCode operation_code) const override;
        virtual simodo::interpret::InterpretState  performOperation(const simodo::ast::Node & op)  override;
        virtual bool            checkSemanticName(const std::u16string & semantic_name)      const override
                                { return semantic_name == SYSTEMVERILOG_HOST_NAME; }
        /// \note Это определение требуется только для того, чтобы успокоить clang-diagnostic
        virtual bool            checkInterpretType(simodo::interpret::InterpretType )        const override
                                { return false; }

    public:
        virtual simodo::interpret::InterpretState   executeNone        ();
        virtual simodo::interpret::InterpretState   executePushConst   (const simodo::ast::Node & op);
        virtual simodo::interpret::InterpretState   executeAssign      (const simodo::ast::Node & op);
        virtual simodo::interpret::InterpretState   executeCompare     (const simodo::ast::Node & op);

        void compare(SystemVerilogOperationCode opcode, const simodo::inout::TokenLocation & location);
        simodo::variable::Value         performCompareEqual(const simodo::variable::Value & op1, const simodo::variable::Value & op2) const;
        simodo::variable::Value         performCompareLess(const simodo::variable::Value & op1, const simodo::variable::Value & op2) const;
        simodo::variable::Value         performCompareLessOrEqual(const simodo::variable::Value & op1, const simodo::variable::Value & op2) const;
        simodo::variable::ValueType     getType4TypeConversion(SystemVerilogOperationCode opcode, simodo::variable::ValueType type1, simodo::variable::ValueType type2) const;
    };
}

#endif // SystemVeriolog_SemanticsHost_abstract
