/*
MIT License

Copyright (c) 2021 МГТУ им. Н.Э. Баумана, кафедра ИУ-6, Михаил Фетисов,

https://bmstu.codes/lsx/simodo/loom
*/

#include "simodo/interpret/SemanticModuleFactory_interface.h"
#include "SystemVerilogAnalyzer.h"
#include "SystemVerilogPreview.h"

#include <memory>
#include <cassert>

#ifdef CROSS_WIN
// MinGW related workaround
#define BOOST_DLL_FORCE_ALIAS_INSTANTIATION
#endif

#include <boost/dll/alias.hpp>

using namespace simodo;

namespace sv
{
    class SystemVerilogInterpret : public interpret::SemanticModuleFactory_interface
    {
        interpret::ModuleManagement_interface & _mm;

    public:
        SystemVerilogInterpret(interpret::ModuleManagement_interface & module_management)
            : _mm(module_management)
        {
        }

        virtual version_t version() const override { return lib_version(); }

        virtual interpret::SemanticModule_interface *
                                        create(interpret::Interpret_interface & ) const override 
        {
            switch(_mm.interpret_type())
            {
            case interpret::InterpretType::Analyzer:
                return new SystemVerilogAnalyzer(_mm, _mm.semantic_data());
            case interpret::InterpretType::Preview:
                return new SystemVerilogPreview(_mm);
            default:
                return nullptr;
            }
        }

        // Self factory method
        static std::shared_ptr<SemanticModuleFactory_interface> self_create(interpret::ModuleManagement_interface & module_management) {
            return std::make_shared<SystemVerilogInterpret>(module_management);
        }
    };

}

BOOST_DLL_ALIAS(
    sv::SystemVerilogInterpret::self_create,   // <-- this function is exported with...
    create_simodo_interpret         // <-- ...this alias name
)

