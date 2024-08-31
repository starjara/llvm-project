#include "RISCV.h"
#include "RISCVTargetMachine.h"
#include "RISCVSubtarget.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

#define RISCV_ANNOT_FUNC_NAME "RISC-V annotation function pass"

namespace {

class RISCVAnnotFunc : public FunctionPass {
public:
  static char ID;

  RISCVAnnotFunc() : FunctionPass(ID) {
    initializeRISCVAnnotFuncPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction (Function &F) override;

  StringRef getPassName() const override {
    return RISCV_ANNOT_FUNC_NAME;
  }
};


char RISCVAnnotFunc::ID = 0;



} // end of annonymous name space

bool RISCVAnnotFunc::runOnFunction(Function &F) {
  errs() << "Function pass\n";

  for (auto &BB : F) {
    for (auto &I : BB) {
      if (auto *AI = dyn_cast<AllocaInst>(&I)) {
	for (auto &U : AI->uses()) {
	  if (auto *CI = dyn_cast<CallInst>(U.getUser())) {
	    if (CI->getCalledFunction()->getName().startswith("llvm.var.annotation")) {
	      errs() << "Annotated variable found: " << *AI << "\n";
	      MDNode *N = MDNode::get(AI->getContext(), MDString::get(AI->getContext(), "hvmi"));
	      AI->setMetadata("hvmi", N);
	      errs() << "Annotated variable with custom tag: " << *AI << "\n";
	    }
	  }
	}
      }
      
    }
  }

  for (auto &BB : F) {
    for (auto &I : BB) {
      if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
	if (Instruction *PtrOp = dyn_cast<Instruction>(LI->getPointerOperand())) {
	  if (PtrOp->getMetadata("hvmi")) {
	    errs() << "Found load instruction with annotated variable: " << *LI << "\n";
	    MDNode *N = MDNode::get(LI->getContext(), MDString::get(LI->getContext(), "hvmi"));
	    LI->setMetadata("hvmi", N);
	  }
	}
      }

      if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
	if (Instruction *PtrOp = dyn_cast<Instruction>(SI->getPointerOperand())) {
	  if (PtrOp->getMetadata("hvmi")) {
	    errs() << "Found store instruction with annotated variable: " << *SI << "\n";
	    MDNode *N = MDNode::get(SI->getContext(), MDString::get(SI->getContext(), "hvmi"));
	    SI->setMetadata("hvmi", N);
	  }
	}
      }
      if(auto *GE = dyn_cast<GetElementPtrInst>(&I)) {
	if (Instruction *PtrOp = dyn_cast<Instruction>(GE->getPointerOperand())) {
	  I.print(errs());
	  errs() << "\n";
	  if(PtrOp->getMetadata("hvmi")) {
	    errs() << "Found GE Metadata\n";
	    MDNode *N = MDNode::get(GE->getContext(), MDString::get(GE->getContext(), "hsv"));
	    GE->setMetadata("hsv", N);
	  }
	}
      }
    }
  }

  for (auto &BB : F) {
    for (auto &I : BB) {
      if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
	if (Instruction *PtrOp = dyn_cast<Instruction>(LI->getPointerOperand())) {
	  if (PtrOp->getMetadata("hsv")) {
	    errs() << "Found load instruction with annotated variable: " << *LI << "\n";
	    MDNode *N = MDNode::get(LI->getContext(), MDString::get(LI->getContext(), "hsv"));
	    LI->setMetadata("hsv", N);
	  }
	}
      }

      if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
	if (Instruction *PtrOp = dyn_cast<Instruction>(SI->getPointerOperand())) {
	  if (PtrOp->getMetadata("hsv")) {
	    errs() << "Found store instruction with annotated variable: " << *SI << "\n";
	    MDNode *N = MDNode::get(SI->getContext(), MDString::get(SI->getContext(), "hsv"));
	    SI->setMetadata("hsv", N);
	  }
	}
      }
    }
  }
  return false;
}

INITIALIZE_PASS(RISCVAnnotFunc, "RISCV Annot Func", RISCV_ANNOT_FUNC_NAME, false, false)  

FunctionPass *llvm::createRISCVAnnotFuncPass() {
  return new RISCVAnnotFunc();
}
