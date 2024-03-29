#include <iostream>
#include <vector>

#include "codegen.h"
using namespace std;

/* Public Class Method */

/**
 * @brief find a Value* of a var by its name
 *
 * @param name
 * @return llvm::Value*
 */
llvm::Value *ContextOfCodeCreate::VALUERETGET(std::string name) {
    cout << "Start get value for `" << name << "`" << endl;

    llvm::Value *RETC;
    if ((RETC = this->pModule->getGlobalVariable(name)) != nullptr) {  // global
        cout << "[Success] Found global variable: " << name << endl;
        return RETC;
    } else if ((RETC = this->CURRENCTFUNCION->getValueSymbolTable()->lookup(name)) != nullptr) {  // local
        cout << "[Success] Found local variable: " << name << endl;
        return RETC;
    } else {  // Not found
        cout << "[Error] Undeclared variable: " << name << endl;
        exit(0);
    }
}

/**
 * @brief return a LLVM type by type in tree
 *
 * @param type
 * @return llvm::Type*
 */
llvm::Type *ContextOfCodeCreate::LLVMTYPERET(tree::Type *type) {
    if (type == nullptr) {
        return llvm::Type::getVoidTy(globalContext);
    }

    if (type->baseType == TY_INT) {
        return llvm::Type::getInt32Ty(globalContext);
    } else if (type->baseType == TY_REAL) {
        return llvm::Type::getFloatTy(globalContext);
    } else if (type->baseType == TY_CHAR) {
        return llvm::Type::getInt8Ty(globalContext);
    } else if (type->baseType == TY_BOOL) {
        return llvm::Type::getInt1Ty(globalContext);
    } else if (type->baseType == TY_ARRAY) {
        return llvm::ArrayType::get(this->LLVMTYPERET(type->childType[0]), type->indexEnd - type->indexStart + 1);
    } else if (type->baseType == TY_RECORD) {
        vector<llvm::Type *> MEMBOFPASC;
        // string of name
        auto const rcd = llvm::StructType::create(globalContext, type->name);

        for (auto &child : type->childType)
            MEMBOFPASC.push_back(this->LLVMTYPERET(child));
        rcd->setBody(MEMBOFPASC);
        return rcd;
    } else {
        auto BlkOFCUR = this->BASECODEBLKcurRETURN();
        while (BlkOFCUR) {
            auto &typeMap = BlkOFCUR->typeMap;
            if (typeMap.find(type->name) != typeMap.end())
                return this->LLVMTYPERET(typeMap[type->name]);
            BlkOFCUR = BlkOFCUR->parent;
        }
        exit(0);
    }
}

void ContextOfCodeCreate::CODEGENER(tree::Program &root, std::string file) {
    cout << "Generating code..." << endl;

    vector<llvm::Type *> TOPAGROFG;  // Type genc
    // main
    llvm::FunctionType *mainType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(globalContext), llvm::makeArrayRef(TOPAGROFG), false);  // array lise size
    // use the thing
    this->MAINOFFUNCTION = llvm::Function::Create(
        mainType, llvm::GlobalValue::InternalLinkage, llvm::Twine("main"), this->pModule);  // give out the funciton
    // main entry
    llvm::BasicBlock *BlkBSE =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("entry"), this->MAINOFFUNCTION, nullptr);  // context use

    this->printf = PRINTFBUILD();

    this->BLKpush(BlkBSE);  // push
    this->CURRENCTFUNCION = this->MAINOFFUNCTION;
    root.PascalCodeCreate(this);
    llvm::ReturnInst::Create(globalContext, this->BASEBLKcurRETURN());  // the use
    this->BLKpop();                                                     // out

    cout << "Code is generated." << endl;

    // generate module pass
    llvm::legacy::PassManager MESGDELI;
    MESGDELI.add(llvm::createPrintModulePass(llvm::outs()));
    MESGDELI.run(*this->pModule);

    // generate .bc
    std::error_code ERRORC;
    llvm::raw_fd_ostream os(file, ERRORC, llvm::sys::fs::F_None);
    llvm::WriteBitcodeToFile(*this->pModule, os);
    os.flush();
}

void ContextOfCodeCreate::runCode() {
    cout << "Running code..." << endl;
    auto exeEngine = llvm::EngineBuilder(std::unique_ptr<llvm::Module>(this->pModule)).create();
    exeEngine->finalizeObject();  // withe the use of obj
    llvm::ArrayRef<llvm::GenericValue> ARGSNO;
    exeEngine->runFunction(this->MAINOFFUNCTION,  //  FUC use
        ARGSNO);                                  //  just value
    cout << "Code run." << endl;
}

int ContextOfCodeCreate::INDEXOFRECRETURN(tree::Type *PSACLTYR, std::string PASCALOFN) {
    int k = 0;
    while (k < PSACLTYR->childType.size()) {
        if (PASCALOFN == PSACLTYR->childType[k]->name)
            return k;
        k++;
    }

    cout << "[Error] we do not find the name: `" << PASCALOFN << "` (in record: " << PSACLTYR->name << ")" << endl;
    exit(0);
}

llvm::Value *ContextOfCodeCreate::ARRAYADDRRETURN(tree::BinaryExp *exp, ContextOfCodeCreate *context) {
    auto PASCALAR = static_cast<tree::VariableExp *>(exp->OPRFIRST)->name;

    if (exp->OPRFIRST->nodeType == ND_VARIABLE_EXP) {
        vector<llvm::Value *> ARIPSACLDE(2);
        ARIPSACLDE[0] = llvm::ConstantInt::get(globalContext, llvm::APInt(32, 0, true));
        ARIPSACLDE[1] = exp->OPRSECOND->PascalCodeCreate(context);

        return llvm::GetElementPtrInst::CreateInBounds(context->VALUERETGET(PASCALAR),
            llvm::ArrayRef<llvm::Value *>(ARIPSACLDE), llvm::Twine("tempname"), context->BASEBLKcurRETURN());
    }

    else {
        cout << "[Error] Type of array is false: " << PASCALAR << endl;
        exit(0);
    }
}

/* Private Class Method */

llvm::Function *ContextOfCodeCreate::PRINTFBUILD() {
    vector<llvm::Type *> PSCALTYPE_PARAM;
    PSCALTYPE_PARAM.push_back(llvm::Type::getInt8PtrTy(globalContext));  // type of push para

    llvm::FunctionType *PSCALTYPE_PRT_TO_SEAM = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(globalContext), llvm::makeArrayRef(PSCALTYPE_PARAM), true);  // give out the diffe
    llvm::Function *OUTPUTFUNC = llvm::Function::Create(PSCALTYPE_PRT_TO_SEAM, llvm::Function::ExternalLinkage,
        llvm::Twine("printf"), this->pModule);         // with the printf thing
    OUTPUTFUNC->setCallingConv(llvm::CallingConv::C);  // module type
    return OUTPUTFUNC;
}

/* Code Generation of tree node */

llvm::Value *tree::Body::PascalCodeCreate(ContextOfCodeCreate *context) {
    for (auto &HSTE : this->stms) {
        HSTE->PascalCodeCreate(context);
        cout << "[Success] We create a statement" << endl;
    }
    return nullptr;
}

llvm::Value *tree::Program::PascalCodeCreate(ContextOfCodeCreate *context) {
    llvm::Value *lastDef = nullptr;

    if (this->define != nullptr) {
        for (tree::ConstDef *&constDef : this->define->constDef) {
            cout << "[Success] a const with the program : " << constDef->name << endl;
            lastDef = constDef->PascalCodeCreate(context);
        }
        for (tree::VarDef *&varDef : this->define->varDef) {
            cout << "[Success] a gloable variable with the program: " << varDef->name << endl;
            varDef->isGlobal = true;
            lastDef          = varDef->PascalCodeCreate(context);
        }
        for (tree::FuncDef *&funcDef : this->define->funcDef) {
            cout << "[Success] a function with the program: " << funcDef->name << endl;
            lastDef = funcDef->PascalCodeCreate(context);
        }
    }

    cout << "[Success] Program created" << endl;
    this->body->PascalCodeCreate(context);

    return lastDef;
}

llvm::Value *tree::Define::PascalCodeCreate(ContextOfCodeCreate *context) {
    return nullptr;
}

llvm::Value *tree::Situation::PascalCodeCreate(ContextOfCodeCreate *context) {
    return this->solution->PascalCodeCreate(context);
}

llvm::Value *tree::LabelDef::PascalCodeCreate(ContextOfCodeCreate *context) {
    return nullptr;
}

llvm::Value *tree::ConstDef::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Const defining: " << this->name << endl;
    if (this->value->nodeType == NX_CONST_EXP) {                                       // if value is const
        tree::EXPRESSIONConst *ZUOOPER = static_cast<EXPRESSIONConst *>(this->value);  //
        llvm::Value *ALOCT = new llvm::AllocaInst(context->LLVMTYPERET(ZUOOPER->returnType), 0, llvm::Twine(this->name),
            context->BASEBLKcurRETURN());  // with the blck out
        llvm::Value *TOSE  = new llvm::StoreInst(
            ZUOOPER->PascalCodeCreate(context), ALOCT, false, context->BASEBLKcurRETURN());  // give out the things

        context->CONSTPLUS(this->name, this->value);  // add the const
        cout << "[Success]  defined with const: " << this->name << endl;
        return TOSE;
    } else {
        cout << "[Error] left value was error: " << this->name << endl;
        exit(0);
    }
}

llvm::Value *tree::TypeDef::PascalCodeCreate(ContextOfCodeCreate *TFECOB) {
    TFECOB->BASECODEBLKcurRETURN()->typeMap[this->name] = this->type;  // define type
    return nullptr;
}

llvm::Value *tree::VarDef::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Variable defining: " << this->name << endl;

    llvm::Value *ALOCT;
    if (this->isGlobal) {
        cout << ">> Global variable" << endl;

        if (this->type->baseType == TY_ARRAY) {  // if array
            cout << ">>>> Array variable" << endl;

            auto vec = vector<llvm::Constant *>();
            llvm::Constant *LISTOFATA;

            if (this->type->childType[0]->baseType == TY_INT) {
                LISTOFATA = llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true);
            } else if (this->type->childType[0]->baseType == TY_REAL) {
                LISTOFATA = llvm::ConstantFP ::get(llvm::Type::getFloatTy(globalContext), 0);
            } else if (this->type->childType[0]->baseType == TY_CHAR) {
                LISTOFATA = llvm::ConstantInt::get(llvm::Type::getInt8Ty(globalContext), 0, true);
            } else if (this->type->childType[0]->baseType == TY_CHAR) {
                LISTOFATA = llvm::ConstantInt::get(llvm::Type::getInt1Ty(globalContext), 0, true);
            } else {
                cout << "[Warning] global array not have label fully" << endl;
                exit(0);
            }

            int k = 0;
            while (k < this->type->indexEnd - this->type->indexStart + 1) {
                vec.push_back(LISTOFATA);
                k++;
            }

            llvm::ArrayType *TYPEA     = static_cast<llvm::ArrayType *>(context->LLVMTYPERET(this->type));
            llvm::Constant *arrOFConst = llvm::ConstantArray::get(TYPEA, vec);
            ALOCT = new llvm::GlobalVariable(*context->pModule, context->LLVMTYPERET(this->type), false,
                llvm::GlobalValue::ExternalLinkage, arrOFConst, llvm::Twine(this->name));

        } else if (this->type->baseType == TY_RECORD) {
            cout << "[Warning] Uncomplete feature for gloable record" << endl;
            exit(0);
        } else {  // Not Array
            llvm::Constant *ATPCOST;

            if (this->type->baseType == TY_INT) {
                ATPCOST = llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true);
            } else if (this->type->baseType == TY_REAL) {
                ATPCOST = llvm::ConstantFP ::get(llvm::Type::getFloatTy(globalContext), 0);
            } else if (this->type->baseType == TY_CHAR) {
                ATPCOST = llvm::ConstantInt::get(llvm::Type::getInt1Ty(globalContext), 0, true);
            } else if (this->type->baseType == TY_CHAR) {
                ATPCOST = llvm::ConstantInt::get(llvm::Type::getInt8Ty(globalContext), 0, true);
            }

            ALOCT = new llvm::GlobalVariable(*context->pModule, context->LLVMTYPERET(this->type), false,
                llvm::GlobalValue::ExternalLinkage, ATPCOST, llvm::Twine(this->name));
        }
    } else {
        ALOCT = new llvm::AllocaInst(
            context->LLVMTYPERET(this->type), 0, llvm::Twine(this->name), context->BASEBLKcurRETURN());  // fuc out of C
    }
    cout << "[Success] we just give variable name of: " << this->name << endl;
    return ALOCT;
}

llvm::Value *tree::FuncDef::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Define of Function : " << this->name << endl;
    context->MAPOFDEFFUNC[this->name] = this;

    vector<llvm::Type *> WTAGT;
    int i = 0;
    while (i < this->TypeofVectorPara.size()) {
        if (this->FomalVectorPara[i])  // pass value
            WTAGT.push_back(llvm::Type::getInt32PtrTy(globalContext));
        else
            WTAGT.push_back(context->LLVMTYPERET(this->TypeofVectorPara[i]));
        i++;
    }

    auto TYPEOF = llvm::FunctionType::get(context->LLVMTYPERET(this->retType), llvm::makeArrayRef(WTAGT), false);
    auto FUCTI =
        llvm::Function::Create(TYPEOF, llvm::GlobalValue::InternalLinkage, llvm::Twine(this->name), context->pModule);

    auto BLCT =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("entry"), FUCTI, nullptr);  // BasicBlock with interface

    auto ELDFUNC = context->CURRECENTFUNCRETURN();
    context->CURRECENTFUNCSET(FUCTI);
    auto BLCOD                  = context->BASEBLKcurRETURN();
    context->MAPOFPARENT[FUCTI] = ELDFUNC;

    context->BLKpush(BLCT);

    llvm::Value *VALUEOFPARA;
    llvm::Argument *ITOROFPARA = FUCTI->arg_begin();
    int k                      = 0;
    while (k < WTAGT.size()) {
        llvm::Type *OPTYE;
        if (this->FomalVectorPara[k]) {
            OPTYE = llvm::Type::getInt32PtrTy(globalContext);
            cout << ">> Define of Formal argument : " << this->VectorNamePara[k] << endl;
        } else {
            OPTYE = context->LLVMTYPERET(this->TypeofVectorPara[k]);
            cout << ">> Define of paramater: " << this->VectorNamePara[k] << endl;
        }
        llvm::Value *TALLOC =
            new llvm::AllocaInst(OPTYE, 0, llvm::Twine(this->VectorNamePara[k]), context->BASEBLKcurRETURN());
        VALUEOFPARA = ITOROFPARA++;
        VALUEOFPARA->setName(llvm::Twine(this->VectorNamePara[k]));
        new llvm::StoreInst(VALUEOFPARA, TALLOC, false, BLCT);
        k++;
    }

    if (this->retType) {
        new llvm::AllocaInst(  // allocte the addr
            context->LLVMTYPERET(this->retType), 0, llvm::Twine(this->name), context->BASEBLKcurRETURN());
        cout << ">> Function return value declaration" << endl;
    }

    cout << ">> [Success] Function header part generated success!" << endl;

    if (this->define) {
        cout << ">> Function variable define" << endl;
        for (auto &i : this->define->varDef)
            i->PascalCodeCreate(context);

        for (auto &i : this->define->funcDef)
            i->PascalCodeCreate(context);

        cout << ">> [Success] Function define part generated success!" << endl;
    }

    this->body->PascalCodeCreate(context);

    if (this->retType != nullptr) {
        cout << ">> Generating return value for function" << endl;
        llvm::Value *LOADING = new llvm::LoadInst(  // load the return addr
            context->VALUERETGET(this->name), llvm::Twine(""), false, context->BASEBLKcurRETURN());
        llvm::ReturnInst::Create(globalContext, LOADING, context->BASEBLKcurRETURN());
        cout << ">> Function returned" << endl;
    } else {
        cout << ">> Generating return void for procedure" << endl;
        llvm::ReturnInst::Create(globalContext, context->BASEBLKcurRETURN());
        cout << ">> Procedure returned" << endl;
    }

    while (context->BASEBLKcurRETURN() != BLCOD) {  // done the func
        context->BLKpop();
    }
    context->CURRECENTFUNCSET(ELDFUNC);

    cout << "[Success] Defined function of our project: " << this->name << endl;
    return FUCTI;
}

llvm::Value *tree::StatementAssign::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Creating assignment statment..." << endl;

    if (this->leftVal->nodeType == ND_BINARY_EXP) {  // left value of two expr
        tree::BinaryExp *OPRENDFIRST = static_cast<tree::BinaryExp *>(this->leftVal);

        switch (OPRENDFIRST->opcode) {
            case OP_INDEX: {
                llvm::Value *POINTELE = ContextOfCodeCreate::ARRAYADDRRETURN(OPRENDFIRST, context);  // get the addr
                cout << "[Success] Assignment statment generate" << endl;
                return new llvm::StoreInst(
                    this->rightVal->PascalCodeCreate(context), POINTELE, false, context->BASEBLKcurRETURN());
                break;
            }
            case OP_DOT: break;
            default:
                cout << "[Error] Wrong left value type" << endl;
                exit(0);
                break;
        }

    } else if (this->leftVal->nodeType == ND_VARIABLE_EXP) {  // left  value is vary
        tree::VariableExp *OPRENDFIRST = static_cast<tree::VariableExp *>(this->leftVal);
        if (OPRENDFIRST->PascalCodeCreate(context)->getType()->isArrayTy()) {  // value is array
            cout << "[Error] left value type error" << endl;
            exit(0);
        } else {  // if not
            llvm::Value *TMPTE = context->VALUERETGET(OPRENDFIRST->name);
            llvm::Value *LOUD;
            do {
                LOUD  = TMPTE;
                TMPTE = new llvm::LoadInst(TMPTE, llvm::Twine(""), false, context->BASEBLKcurRETURN());
            } while (TMPTE->getType()->isPointerTy());

            return new llvm::StoreInst(
                this->rightVal->PascalCodeCreate(context), LOUD, false, context->BASEBLKcurRETURN());
        }
    } else {  // no thing left
        cout << "[Error] Wrong left value type" << endl;
        exit(0);
    }
    return nullptr;
}

llvm::Value *tree::CallStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Calling function: " << this->name << endl;
    if (this->name == "write" || this->name == "writeln") {  // judge the wirte
        bool isWriteln = (this->name == "writeln");
        std::string FORMALPRTF;
        vector<llvm::Value *> PARAMPRTF;

        for (tree::Exp *arg : this->args) {
            llvm::Value *VALUEPARA = arg->PascalCodeCreate(context);  // get the value
            if (VALUEPARA->getType() == llvm::Type::getInt32Ty(globalContext)) {
                FORMALPRTF += "%d";
                cout << ">> give out the write variable name before" << VALUEPARA->getName().str() << endl;
                PARAMPRTF.push_back(VALUEPARA);
            } else if (VALUEPARA->getType()->isFloatTy()) {
                FORMALPRTF += "%f";
                PARAMPRTF.push_back(VALUEPARA);
            } else if (VALUEPARA->getType() == llvm::Type::getInt8PtrTy(globalContext)) {
                cout << "[Warning] we not support the print of str" << endl;
                exit(0);
            } else {
                cout << "[Error] Print type wrong" << endl;
                exit(0);
            }
        }

        if (isWriteln) {
            FORMALPRTF += "\n";
        }

        cout << "print normal: " << FORMALPRTF << endl;
        llvm::Constant *CONSTOFPRINTFOMAL =
            llvm::ConstantDataArray::getString(globalContext, FORMALPRTF.c_str());  // creat const
        llvm::GlobalVariable *formatStringVar = new llvm::GlobalVariable(*context->pModule,
            llvm::ArrayType::get(llvm::IntegerType::getInt8Ty(globalContext), strlen(FORMALPRTF.c_str()) + 1), true,
            llvm::GlobalValue::PrivateLinkage, CONSTOFPRINTFOMAL, llvm::Twine(".str"));  // a new gloabl var
        llvm::Constant *ZEO = llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(globalContext));
        vector<llvm::Constant *> INDEXOFA;
        INDEXOFA.push_back(ZEO);
        INDEXOFA.push_back(ZEO);
        llvm::Constant *REFERENCE_V = llvm::ConstantExpr::getGetElementPtr(
            llvm::ArrayType::get(llvm::IntegerType::getInt8Ty(globalContext), strlen(FORMALPRTF.c_str()) + 1),
            formatStringVar, INDEXOFA);  // from 0 to start
        PARAMPRTF.insert(PARAMPRTF.begin(), REFERENCE_V);
        cout << "[Success] Print call created" << endl;
        return llvm::CallInst::Create(
            context->printf, llvm::makeArrayRef(PARAMPRTF), llvm::Twine(""), context->BASEBLKcurRETURN());
    }

    // no write/writeln
    llvm::Function *OPWRITFUNC = context->pModule->getFunction(this->name.c_str());  // find func
    if (OPWRITFUNC == nullptr) {
        cout << "[Error] defined error of func" << endl;
        exit(0);
    }
    vector<llvm::Value *> VAL_PARA;
    auto ITERARGFUNC = OPWRITFUNC->arg_begin();
    for (tree::Exp *arg : this->args) {
        llvm::Value *funcArgValue = static_cast<llvm::Value *>(ITERARGFUNC++);
        if (funcArgValue->getType()->isPointerTy()) {  // if POINTER
            if (arg->nodeType == ND_VARIABLE_EXP) {    // if vary
                llvm::Value *POINTER =
                    context->VALUERETGET(static_cast<tree::VariableExp *>(arg)->name);  // the value of type
                while (POINTER->getType() != llvm::Type::getInt32PtrTy(globalContext)) {
                    POINTER = new llvm::LoadInst(POINTER, llvm::Twine(""), false, context->BASEBLKcurRETURN());
                }
                VAL_PARA.push_back(POINTER);
            } else if (arg->nodeType == ND_BINARY_EXP) {
                BinaryExp *ASTNODE = static_cast<BinaryExp *>(arg);
                switch (ASTNODE->opcode) {
                    case OP_DOT: {
                        if (ASTNODE->OPRSECOND->nodeType == ND_VARIABLE_EXP) {
                            VariableExp *OPRONDSECOND = static_cast<VariableExp *>(ASTNODE->OPRSECOND);
                            int INDEXOFREC            = ContextOfCodeCreate::INDEXOFRECRETURN(
                                ASTNODE->OPRFIRST->returnType, OPRONDSECOND->name);
                            vector<llvm::Value *> INDEXOFA(2);
                            INDEXOFA[0] = llvm::ConstantInt::get(globalContext, llvm::APInt(32, 0, true));
                            INDEXOFA[1] = llvm::ConstantInt::get(globalContext, llvm::APInt(32, INDEXOFREC, true));
                            llvm::Value *POINTER =
                                llvm::GetElementPtrInst::Create(context->LLVMTYPERET(ASTNODE->OPRFIRST->returnType),
                                    context->VALUERETGET(OPRONDSECOND->name), INDEXOFA, llvm::Twine(""),
                                    context->BASEBLKcurRETURN());
                            VAL_PARA.push_back(POINTER);

                        } else {
                            cout << "[Error] OPRSECOND of dot operation is not a variable exp type" << endl;
                            exit(0);
                        }
                        break;
                    }
                    case OP_INDEX: {
                        if (ASTNODE->OPRFIRST->nodeType == ND_VARIABLE_EXP) {
                            VariableExp *operandFIRST = static_cast<VariableExp *>(ASTNODE->OPRFIRST);
                            vector<llvm::Value *> INDEXOFA(2);
                            INDEXOFA[0]          = llvm::ConstantInt::get(globalContext, llvm::APInt(32, 0, true));
                            INDEXOFA[1]          = ASTNODE->OPRSECOND->PascalCodeCreate(context);
                            llvm::Value *POINTER = llvm::GetElementPtrInst::CreateInBounds(
                                context->VALUERETGET(operandFIRST->name), llvm::ArrayRef<llvm::Value *>(INDEXOFA),
                                llvm::Twine("tempname"), context->BASEBLKcurRETURN());
                            VAL_PARA.push_back(POINTER);
                        } else {
                            cout << "[Error] Array's Ref is not an array type variable" << endl;
                            exit(0);
                        }
                        break;
                    }
                    default: break;
                }

            } else {
                cout << "[Error] left value type error " << endl;
            }
        } else {
            VAL_PARA.push_back(arg->PascalCodeCreate(context));
        }
    }
    cout << "[Success] Function called." << endl;
    return llvm::CallInst::Create(
        OPWRITFUNC, llvm::makeArrayRef(VAL_PARA), llvm::Twine(""), context->BASEBLKcurRETURN());
}

llvm::Value *tree::LabelStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    return nullptr;
}

llvm::Value *tree::IfStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "building while statement" << endl;
    llvm::Value *TCODIN = condition->PascalCodeCreate(context);
    llvm::BasicBlock *BLK_YES =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("then"), context->CURRECENTFUNCRETURN());
    llvm::BasicBlock *BLK_NO =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("else"), context->CURRECENTFUNCRETURN());
    llvm::BasicBlock *MERGEBLK =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("merge"), context->CURRECENTFUNCRETURN());
    cout << ">> [Success] Condition generated" << endl;

    llvm::Value *RYUETE =
        llvm::BranchInst::Create(BLK_YES, BLK_NO, TCODIN, context->BASEBLKcurRETURN());  // bsaci of retievce

    context->BLKpush(BLK_YES);
    this->trueBody->PascalCodeCreate(context);
    cout << ">> [Success] generated True" << endl;
    llvm::BranchInst::Create(MERGEBLK, context->BASEBLKcurRETURN());  //  merge return while code build true

    context->BLKpop();
    context->BLKpush(BLK_NO);

    if (this->falseBody != nullptr) {
        this->falseBody->PascalCodeCreate(context);
        cout << ">> [Success] generated False" << endl;
    }
    llvm::BranchInst::Create(MERGEBLK, context->BASEBLKcurRETURN());  //  merge return while code build false
    context->BLKpop();
    context->BLKpush(MERGEBLK);
    cout << "[Success] If statment generated" << endl;
    return RYUETE;
}

llvm::Value *tree::CaseStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Creating case statment" << endl;
    llvm::BasicBlock *ESCBLK =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("exit"), context->CURRECENTFUNCRETURN());
    vector<llvm::BasicBlock *> TBLKS;
    cout << ">> Contains :" << situations.size() << "cases" << endl;

    llvm::Value *RETIV;
    int i = 0;
    while (i < this->situations.size()) {
        for (int j = 0; j < this->situations[i]->caseVec.size(); j++) {
            llvm::BasicBlock *BLKBASE =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("case"), context->CURRECENTFUNCRETURN());

            cout << ">> In case " << i << ":" << j << endl;
            tree::BinaryExp *CONDITION = new tree::BinaryExp(OP_EQUAL, this->object, this->situations[i]->caseVec[j]);

            if (i == this->situations.size() - 1 && j == this->situations[i]->caseVec.size() - 1) {  // last one
                RETIV = llvm::BranchInst::Create(BLKBASE, ESCBLK, CONDITION->PascalCodeCreate(context),
                    context->BASEBLKcurRETURN());  // insert the last one
            } else {
                llvm::BasicBlock *BLKNEXT =
                    llvm::BasicBlock::Create(globalContext, llvm::Twine("next"), context->CURRECENTFUNCRETURN());
                llvm::BranchInst::Create(BLKBASE, BLKNEXT, CONDITION->PascalCodeCreate(context),
                    context->BASEBLKcurRETURN());  // not true would be next
                context->BLKpush(BLKNEXT);
            }
        }
        i++;
    }

    i = 0;
    while (i < this->situations.size()) {
        for (int j = 0; j < this->situations[i]->caseVec.size(); j++) {  // situation with match
            llvm::BasicBlock *BLKBASE =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("caseStmt"), context->CURRECENTFUNCRETURN());
            TBLKS.push_back(BLKBASE);  // a block of  push
        }
        i++;
    }

    int p = 0;
    i     = 0;

    while (i < this->situations.size()) {
        for (int j = 0; j < this->situations[i]->caseVec.size(); j++, p++) {
            cout << "in case No." << i << endl;
            cout << "|__case's No." << j << endl;

            tree::BinaryExp *CONDITION = new tree::BinaryExp(OP_EQUAL, this->object, this->situations[i]->caseVec[j]);

            llvm::BasicBlock *BLKNEXT;
            if (p == TBLKS.size() - 1) {
                RETIV = llvm::BranchInst::Create(
                    TBLKS[p], ESCBLK, CONDITION->PascalCodeCreate(context), context->BASEBLKcurRETURN());  // END insret
            } else {
                BLKNEXT = llvm::BasicBlock::Create(globalContext, "next", context->CURRECENTFUNCRETURN());

                llvm::BranchInst::Create(TBLKS[p], BLKNEXT, CONDITION->PascalCodeCreate(context),
                    context->BASEBLKcurRETURN());  // basic create

                context->BLKpush(BLKNEXT);
            }
        }
        i++;
        p++;
    }

    p = 0;
    i = 0;
    while (i < this->situations.size()) {
        for (int j = 0; j < this->situations[i]->caseVec.size(); j++, p++) {
            context->BLKpush(TBLKS[p]);
            this->situations[i]->PascalCodeCreate(context);
            llvm::BranchInst::Create(ESCBLK, context->BASEBLKcurRETURN());
            cout << ">> [Success] In case " << i << ":" << j << endl;
            context->BLKpop();
        }
        i++;
        p++;
    }

    context->BLKpush(ESCBLK);

    return RETIV;
}

llvm::Value *tree::ForStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Creating for statement" << endl;
    llvm::BasicBlock *BLKSRT =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("start"), context->CURRECENTFUNCRETURN());
    llvm::BasicBlock *BLKLOP =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("loop"), context->CURRECENTFUNCRETURN());
    llvm::BasicBlock *BLKLESC =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("exit"), context->CURRECENTFUNCRETURN());
    // 定义循环变量
    tree::VariableExp *PARALOOP         = new tree::VariableExp(this->iter);
    PARALOOP->returnType                = tree::findVar(this->iter, this);
    PARALOOP->name                      = this->iter;
    tree::StatementAssign *LOOPINITPARA = new tree::StatementAssign(PARALOOP, this->start);
    LOOPINITPARA->PascalCodeCreate(context);
    llvm::BranchInst::Create(BLKSRT, context->BASEBLKcurRETURN());

    context->BLKpush(BLKSRT);
    tree::BinaryExp *COMPARE = new tree::BinaryExp(OP_EQUAL, PARALOOP, this->end);
    llvm::Instruction *RETC  = llvm::BranchInst::Create(
        BLKLESC, BLKLOP, COMPARE->PascalCodeCreate(context), context->BASEBLKcurRETURN());  // to the end or not
    context->BLKpop();
    context->BLKpush(BLKLOP);
    this->loop->PascalCodeCreate(context);

    // LOOPUPDATE
    tree::BinaryExp *LOOPUPDATE;
    tree::Value *TMP                     = new tree::Value();
    TMP->baseType                        = TY_INT;
    TMP->val.intVal                      = this->step;
    tree::EXPRESSIONConst *int1          = new tree::EXPRESSIONConst(TMP);
    LOOPUPDATE                           = new tree::BinaryExp(OP_ADD, PARALOOP, int1);
    tree::StatementAssign *updateLoopVar = new tree::StatementAssign(PARALOOP, LOOPUPDATE);
    updateLoopVar->PascalCodeCreate(context);
    llvm::BranchInst::Create(BLKSRT, context->BASEBLKcurRETURN());
    context->BLKpop();
    context->BLKpush(BLKLESC);
    this->loop->PascalCodeCreate(context);
    cout << "[Success] For loop generated" << endl;
    return RETC;
}

llvm::Value *tree::WhileStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Building WIHILE statement" << endl;
    llvm::BasicBlock *BLKST =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("start"), context->CURRECENTFUNCRETURN());
    llvm::BasicBlock *BLKLOP =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("loop"), context->CURRECENTFUNCRETURN());
    llvm::BasicBlock *BLKESC =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("exit"), context->CURRECENTFUNCRETURN());

    llvm::BranchInst::Create(BLKST, context->BASEBLKcurRETURN());
    context->BLKpush(BLKST);  // give the start
    llvm::Value *RECT = llvm::BranchInst::Create(
        BLKLOP, BLKESC, this->condition->PascalCodeCreate(context), context->BASEBLKcurRETURN());  // true jump or nit
    context->BLKpop();
    context->BLKpush(BLKLOP);
    this->loop->PascalCodeCreate(context);  // still cong
    llvm::BranchInst::Create(BLKST, context->BASEBLKcurRETURN());
    context->BLKpop();
    context->BLKpush(BLKESC);  // out boclk
    cout << "[Success] While loop create" << endl;
    return RECT;
}

llvm::Value *tree::StatementRepeat::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Building repeat statement" << endl;
    llvm::BasicBlock *BLKLOP =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("loop"), context->CURRECENTFUNCRETURN());
    llvm::BranchInst::Create(BLKLOP, context->BASEBLKcurRETURN());
    context->BLKpush(BLKLOP);               // loop
    this->loop->PascalCodeCreate(context);  // build blk

    llvm::Value *CONDITION = this->condition->PascalCodeCreate(context);  // cond

    llvm::BasicBlock *BLKESC =
        llvm::BasicBlock::Create(globalContext, llvm::Twine("exit"), context->CURRECENTFUNCRETURN());

    llvm::Instruction *RECT =
        llvm::BranchInst::Create(BLKESC, BLKLOP, CONDITION, context->BASEBLKcurRETURN());  // true jump or not
    context->BLKpop();
    context->BLKpush(BLKESC);  // BLKLESC
    cout << "[Success] Repeat loop created" << endl;
    return RECT;
}

llvm::Value *tree::GotoStm::PascalCodeCreate(ContextOfCodeCreate *context) {
    return nullptr;
}

llvm::Value *tree::UnaryExp::PascalCodeCreate(ContextOfCodeCreate *context) {
    if (this->opcode == OP_NOT) {
        return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGT,
            this->operand->PascalCodeCreate(context),
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true), llvm::Twine(""),
            context->BASEBLKcurRETURN());
    } else if (this->opcode == OP_OPPO) {
        if (this->operand->returnType->baseType == TY_INT) {  // define n= 0- n
            return llvm::BinaryOperator::Create(llvm::Instruction::Sub,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true),
                this->operand->PascalCodeCreate(context), llvm::Twine(""), context->BASEBLKcurRETURN());
        } else if (this->operand->returnType->baseType == TY_REAL) {
            return llvm::BinaryOperator::Create(llvm::Instruction::FSub,
                llvm::ConstantFP::get(globalContext, llvm::APFloat(0.)), this->operand->PascalCodeCreate(context),
                llvm::Twine(""), context->BASEBLKcurRETURN());
        }
    } else if (this->opcode == OP_PRED) {
        return llvm::BinaryOperator::Create(llvm::Instruction::Sub, this->operand->PascalCodeCreate(context),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(globalContext), 1, true), llvm::Twine(""),
            context->BASEBLKcurRETURN());
    } else if (this->opcode == OP_SUCC) {
        return llvm::BinaryOperator::Create(llvm::Instruction::Add, this->operand->PascalCodeCreate(context),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(globalContext), 1, true), llvm::Twine(""),
            context->BASEBLKcurRETURN());
    } else if (this->opcode == OP_ODD) {  // 0 & n
        return llvm::BinaryOperator::Create(llvm::Instruction::And,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true),
            this->operand->PascalCodeCreate(context), llvm::Twine(""), context->BASEBLKcurRETURN());
    } else if (this->opcode == OP_ORD) {
        return llvm::CastInst::CreateIntegerCast(this->operand->PascalCodeCreate(context),
            llvm::Type::getInt32Ty(globalContext), true, llvm::Twine(""), context->BASEBLKcurRETURN());

    } else if (this->opcode == OP_CHR) {
        return llvm::CastInst::CreateIntegerCast(this->operand->PascalCodeCreate(context),
            llvm::Type::getInt8Ty(globalContext), true, llvm::Twine(""), context->BASEBLKcurRETURN());

    } else if (this->opcode == OP_ABS) {
        if (this->operand->returnType->baseType == TY_INT) {
            llvm::Value *CONDITION = llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGT,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true),
                this->operand->PascalCodeCreate(context), llvm::Twine(""), context->BASEBLKcurRETURN());
            llvm::BasicBlock *BLKDOWN =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("neg"), context->CURRECENTFUNCRETURN());
            llvm::BasicBlock *BLKUP =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("pos"), context->CURRECENTFUNCRETURN());
            llvm::BasicBlock *BLKMERGE =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("merge"), context->CURRECENTFUNCRETURN());
            llvm::Value *RETC = llvm::BranchInst::Create(
                BLKDOWN, BLKUP, CONDITION, context->BASEBLKcurRETURN());  // positve or negative

            context->BLKpush(BLKDOWN);
            llvm::BinaryOperator::Create(llvm::Instruction::Sub,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), 0, true),
                this->operand->PascalCodeCreate(context), llvm::Twine(""), context->BASEBLKcurRETURN());
            llvm::BranchInst::Create(BLKMERGE, context->BASEBLKcurRETURN());  // jump BLKMERGE

            context->BLKpop();
            context->BLKpush(BLKUP);                                          // not true
            this->operand->PascalCodeCreate(context);                         // abs
            llvm::BranchInst::Create(BLKMERGE, context->BASEBLKcurRETURN());  // jump BLKMERGE
            context->BLKpop();
            context->BLKpush(BLKMERGE);  // BLKMERGE
            return RETC;
        } else if (this->operand->returnType->baseType == TY_REAL) {
            llvm::Value *CONDITION = llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGT,
                llvm::ConstantFP::get(globalContext, llvm::APFloat(0.)), this->operand->PascalCodeCreate(context),
                llvm::Twine(""), context->BASEBLKcurRETURN());
            llvm::BasicBlock *BLKDOWN =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("neg"), context->CURRECENTFUNCRETURN());
            llvm::BasicBlock *BLKUP =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("pos"), context->CURRECENTFUNCRETURN());
            llvm::BasicBlock *BLKMERGE =
                llvm::BasicBlock::Create(globalContext, llvm::Twine("merge"), context->CURRECENTFUNCRETURN());
            llvm::Value *RETC = llvm::BranchInst::Create(BLKDOWN, BLKUP, CONDITION, context->BASEBLKcurRETURN());
            context->BLKpush(BLKDOWN);

            llvm::BinaryOperator::Create(llvm::Instruction::FSub,
                llvm::ConstantFP::get(globalContext, llvm::APFloat(0.)), this->operand->PascalCodeCreate(context),
                llvm::Twine(""), context->BASEBLKcurRETURN());
            llvm::BranchInst::Create(BLKMERGE, context->BASEBLKcurRETURN());
            context->BLKpop();
            context->BLKpush(BLKUP);
            this->operand->PascalCodeCreate(context);
            llvm::BranchInst::Create(BLKMERGE, context->BASEBLKcurRETURN());
            context->BLKpop();
            context->BLKpush(BLKMERGE);
            return RETC;
        }
    }
    return nullptr;
}

llvm::Value *tree::BinaryExp::PascalCodeCreate(ContextOfCodeCreate *context) {

    switch (this->opcode) {
        case OP_DOT: {
            if (this->OPRSECOND->nodeType == ND_VARIABLE_EXP) {  // vary of op2
                tree::VariableExp *OPRONDSECOND = static_cast<tree::VariableExp *>(this->OPRSECOND);
                int INDEXOFREC =
                    ContextOfCodeCreate::INDEXOFRECRETURN(this->OPRFIRST->returnType, OPRONDSECOND->name);  // find pos
                vector<llvm::Value *> INDEXOFA(2);
                INDEXOFA[0]         = llvm::ConstantInt::get(  // 0
                    globalContext, llvm::APInt(32, 0, true));
                INDEXOFA[1]         = llvm::ConstantInt::get(                   // index
                    globalContext, llvm::APInt(32, INDEXOFREC, true));  // create member_index
                llvm::Value *memPtr = llvm::GetElementPtrInst::Create(context->LLVMTYPERET(this->OPRFIRST->returnType),
                    context->VALUERETGET(OPRONDSECOND->name), INDEXOFA, llvm::Twine(""), context->BASEBLKcurRETURN());
                return new llvm::LoadInst(memPtr, llvm::Twine(""), false, context->BASEBLKcurRETURN());
            } else {
                cout << "[Error] error member of record " << endl;
                exit(0);
            }
            break;
        }

        case OP_INDEX: {
            if (this->OPRFIRST->nodeType == ND_VARIABLE_EXP) {
                tree::VariableExp *OPRONDFIRST = static_cast<tree::VariableExp *>(this->OPRFIRST);
                vector<llvm::Value *> INDEXOFA(2);
                INDEXOFA[0]         = llvm::ConstantInt::get(globalContext, llvm::APInt(32, 0, true));
                INDEXOFA[1]         = this->OPRSECOND->PascalCodeCreate(context);
                llvm::Value *memPtr = llvm::GetElementPtrInst::CreateInBounds(context->VALUERETGET(OPRONDFIRST->name),
                    llvm::ArrayRef<llvm::Value *>(INDEXOFA), llvm::Twine("tempname"), context->BASEBLKcurRETURN());
                return new llvm::LoadInst(memPtr, llvm::Twine(""), false, context->BASEBLKcurRETURN());
            } else {
                cout << "[Error] List's reference not true with the type" << endl;
                exit(0);
            }
            break;
        }

        default: break;
    }

    llvm::Value *VAL_OPREANDFIRST_  = this->OPRFIRST->PascalCodeCreate(context);
    llvm::Value *VAL_OPREANDSECOND_ = this->OPRSECOND->PascalCodeCreate(context);

    if (VAL_OPREANDFIRST_->getType()->isFloatTy() || VAL_OPREANDSECOND_->getType()->isFloatTy()) {  // if float
        switch (this->opcode) {
            case OP_ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::FAdd, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_MINUS:
                return llvm::BinaryOperator::Create(llvm::Instruction::FSub, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_MULTI:
                return llvm::BinaryOperator::Create(llvm::Instruction::FMul, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_RDIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::FDiv, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_DDIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_MOD:
                return llvm::BinaryOperator::Create(llvm::Instruction::SRem, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            // logical
            case OP_SMALL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SLT, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_LARGE:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGT, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_SMALL_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGE, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_LARGE_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SLE, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_EQ, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_NOT_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_NE, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            default: cout << "[Error] error type of opreation:" << opcode << endl; exit(0);
        }
    } else {  // int
        switch (opcode) {
            case OP_ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_MINUS:
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_MULTI:
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_RDIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::UDiv, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_DDIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_MOD:
                return llvm::BinaryOperator::Create(llvm::Instruction::SRem, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, VAL_OPREANDFIRST_, VAL_OPREANDSECOND_,
                    llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_SMALL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SLT, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_LARGE:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGT, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_SMALL_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SGE, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_LARGE_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_SLE, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_EQ, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            case OP_NOT_EQUAL:
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::CmpInst::ICMP_NE, VAL_OPREANDFIRST_,
                    VAL_OPREANDSECOND_, llvm::Twine(""), context->BASEBLKcurRETURN());
            default: cout << "[Error] Unknown type of opcode:" << opcode << endl; exit(0);
        }
    }
}

llvm::Value *tree::CallExp::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "Building calling " << endl;
    llvm::Function *FUcTION = context->pModule->getFunction(this->name.c_str());
    if (FUcTION == nullptr) {
        cout << "[Error] Function: " << name << " didn't defined" << endl;
        exit(0);
    }

    vector<llvm::Value *> VALUEPARA;
    auto ITERFUNPARA = FUcTION->arg_begin();
    for (tree::Exp *arg : this->args) {
        llvm::Value *funcArgValue = static_cast<llvm::Value *>(ITERFUNPARA++);
        if (funcArgValue->getType()->isPointerTy()) {                                                      // if ptr
            if (arg->nodeType == ND_VARIABLE_EXP) {                                                        // if vary
                llvm::Value *POINTER = context->VALUERETGET(static_cast<tree::VariableExp *>(arg)->name);  // value
                while (POINTER->getType() != llvm::Type::getInt32PtrTy(globalContext)) {
                    POINTER = new llvm::LoadInst(POINTER, llvm::Twine(""), false, context->BASEBLKcurRETURN());
                }
                VALUEPARA.push_back(POINTER);
            } else if (arg->nodeType == ND_BINARY_EXP) {
                BinaryExp *ASTNODE = static_cast<BinaryExp *>(arg);

                switch (ASTNODE->opcode) {
                    case OP_DOT: {
                        if (ASTNODE->OPRSECOND->nodeType == ND_VARIABLE_EXP) {
                            VariableExp *OPRONDSECOND = static_cast<VariableExp *>(ASTNODE->OPRSECOND);
                            int INDEXOFREC            = ContextOfCodeCreate::INDEXOFRECRETURN(
                                ASTNODE->OPRFIRST->returnType, OPRONDSECOND->name);
                            vector<llvm::Value *> INDEXOFA(2);
                            INDEXOFA[0] = llvm::ConstantInt::get(globalContext, llvm::APInt(32, 0, true));  // zero
                            INDEXOFA[1] =
                                llvm::ConstantInt::get(globalContext, llvm::APInt(32, INDEXOFREC, true));  // index
                            llvm::Value *POINTER =
                                llvm::GetElementPtrInst::Create(context->LLVMTYPERET(ASTNODE->OPRFIRST->returnType),
                                    context->VALUERETGET(OPRONDSECOND->name), INDEXOFA, llvm::Twine(""),
                                    context->BASEBLKcurRETURN());  // record the name
                            VALUEPARA.push_back(POINTER);
                        } else {
                            cout << "[Error] OPRSECOND of dot operation would not match the exp type" << endl;
                            exit(0);
                        }
                        break;
                    }

                    case OP_INDEX: {
                        if (ASTNODE->OPRFIRST->nodeType == ND_VARIABLE_EXP) {
                            VariableExp *OPRONDFIRST = static_cast<VariableExp *>(ASTNODE->OPRFIRST);
                            vector<llvm::Value *> INDEXOFA(2);
                            INDEXOFA[0]          = llvm::ConstantInt::get(globalContext, llvm::APInt(32, 0, true));
                            INDEXOFA[1]          = ASTNODE->OPRSECOND->PascalCodeCreate(context);
                            llvm::Value *POINTER = llvm::GetElementPtrInst::CreateInBounds(
                                context->VALUERETGET(OPRONDFIRST->name), llvm::ArrayRef<llvm::Value *>(INDEXOFA),
                                llvm::Twine("tempname"), context->BASEBLKcurRETURN());
                            VALUEPARA.push_back(POINTER);
                        } else {
                            cout << "[Error] List's reference not true with the type" << endl;
                            exit(0);
                        }
                    }

                    default: break;
                }

            } else {
                cout << "[Error] left value type error" << endl;
            }
        } else {
            VALUEPARA.push_back(arg->PascalCodeCreate(context));
        }
    }
    return llvm::CallInst::Create(FUcTION, llvm::makeArrayRef(VALUEPARA), llvm::Twine(""), context->BASEBLKcurRETURN());
}

llvm::Value *tree::EXPRESSIONConst::PascalCodeCreate(ContextOfCodeCreate *context) {
    return value->PascalCodeCreate(context);
}

llvm::Value *tree::VariableExp::PascalCodeCreate(ContextOfCodeCreate *context) {
    cout << "variable load with: '" << this->name << "'" << endl;
    llvm::Value *PTRIONT = context->VALUERETGET(this->name);
    PTRIONT              = new llvm::LoadInst(PTRIONT, llvm::Twine(""), false, context->BASEBLKcurRETURN());
    if (PTRIONT->getType()->isPointerTy()) {
        PTRIONT = new llvm::LoadInst(PTRIONT, llvm::Twine(""), false, context->BASEBLKcurRETURN());
    }
    return PTRIONT;
}

llvm::Value *tree::Type::PascalCodeCreate(ContextOfCodeCreate *context) {
    return nullptr;
}

llvm::Value *tree::Value::PascalCodeCreate(ContextOfCodeCreate *context) {

    if (this->baseType == TY_INT) {
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalContext), this->val.intVal, true);
    } else if (this->baseType == TY_REAL) {
        return llvm::ConstantFP::get(globalContext, llvm::APFloat(this->val.realVal));
    } else if (this->baseType == TY_CHAR) {
        return llvm::ConstantInt::get(llvm::Type::getInt8Ty(globalContext), this->val.charVal, true);
    } else if (this->baseType == TY_BOOL) {
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(globalContext), this->val.boolVal, true);
    } else {
        return nullptr;
    }
}
