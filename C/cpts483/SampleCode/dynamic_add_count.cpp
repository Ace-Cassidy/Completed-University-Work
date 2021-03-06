/*
 * dynamic_add_count.cpp
 * Copyright (C) 2020 Aravind SUKUMARAN RAJAM (asr) <aravind_sr@outlook.com>
 *
 * Distributed under terms of the GNU LGPL3 license.
 */

#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/GlobalVariable.h"

#include <iostream>
using namespace llvm;

namespace {
struct Hello3 : public ModulePass {

  static char ID;
  Hello3() : ModulePass(ID) {}

  Constant *getConstUi64(int Val, LLVMContext &Context) {
    IntegerType *Ui64Type = IntegerType::getInt64Ty(Context);
    return ConstantInt::get(Ui64Type, Val);
  }

  AllocaInst *allocateCounter(Function &F, Module &M, LLVMContext &Context) {
    Instruction *FirstInst = F.getEntryBlock().getFirstNonPHI();
    IntegerType *Ui64Type = IntegerType::getInt64Ty(Context);
    const DataLayout &DL = M.getDataLayout();

    AllocaInst *CntrAllocaInst =
        new AllocaInst(Ui64Type, DL.getAllocaAddrSpace(), "counter", FirstInst);

    return CntrAllocaInst;
  }

  StoreInst *initCounter(AllocaInst *CntrAllocaInst, LLVMContext &Context) {
    Instruction *NextInst = CntrAllocaInst->getNextNonDebugInstruction();
    assert(NextInst != nullptr && "this cant be the end of a BB");
    StoreInst *StoreZeroInst =
        new StoreInst(getConstUi64(0, Context), CntrAllocaInst, NextInst);
    return StoreZeroInst;
  }

  CastInst *createConstGlobalString(const char *GlobString,
                                    LLVMContext &Context, Function &F) {
    Constant *PrintStringConstant =
        ConstantDataArray::getString(Context, GlobString);
    GlobalVariable *GPrintVal = new GlobalVariable(
        *F.getParent(), PrintStringConstant->getType(), true,
        GlobalValue::PrivateLinkage, PrintStringConstant, ".strprint");

    PointerType *I8PType = IntegerType::getInt8PtrTy(Context);
    Instruction *FirstInst = F.getEntryBlock().getFirstNonPHI();
    CastInst *CastToPtr =
        CastInst::CreatePointerCast(GPrintVal, I8PType, "tmp", FirstInst);
    return CastToPtr;
  }

  bool runOnModule(Module &M) override {
    for (Function &F : M) {

      if (F.isDeclaration()) {
        errs() << "Continue\n";
        continue;
      }

      LLVMContext &Context = F.getContext();

      AllocaInst *CntrAllocaInst = allocateCounter(F, M, Context);
      initCounter(CntrAllocaInst, Context);

      IntegerType *Ui64Type = IntegerType::getInt64Ty(Context);
      // IntegerType* I32Type = IntegerType::getInt32Ty(Context);
      FunctionCallee Printfun = M.getFunction("printf");
      // getOrInsertFunction("printf",
      //                 FunctionType::get(I32Type, I8p, true));
      CastInst *PrintfStr = createConstGlobalString(
          "Total number of add operations  is %d\n", Context, F);


      for (BasicBlock &B : F) {
        for (Instruction &I : B) {
          if (I.getOpcode() == Instruction::Add) {
            LoadInst *LoadInstTmp =
                new LoadInst(Ui64Type, CntrAllocaInst, "ctrload", &I);
            BinaryOperator *AddInstTmp = BinaryOperator::Create(
                Instruction::Add, getConstUi64(1, Context), LoadInstTmp,
                "counterinc", &I);
            new StoreInst(AddInstTmp, CntrAllocaInst, &I);
          } else if (isa<ReturnInst>(I)) {

            LoadInst *LoadInstTmp =
                new LoadInst(Ui64Type, CntrAllocaInst, "ctrload", &I);
            ArrayRef<Value *> Args{PrintfStr,  LoadInstTmp};

            CallInst::Create(Printfun, Args, "call483", &I);
          }
        }
      }
    }
    return true;
  }
};
} // namespace

char Hello3::ID = 0;
static RegisterPass<Hello3> X("dyn_add_count", "3rd 483 pass",
                              false /* Only looks at CFG */,
                              false /* Analysis Pass */);
