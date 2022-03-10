#include "ARM.h"
#include "ARMMachineFunctionInfo.h"
#include "ARMSubtarget.h"
#include "ARMInstrInfo.h"

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#include "llvm/IR/IRBuilder.h"

#include "llvm/Support/Debug.h"

using namespace llvm;

#define ARM_LOGGER_NAME "ARM Logger"

namespace {
	class ARMLogger : public MachineFunctionPass {
		public:
			static char ID;
			ARMLogger() : MachineFunctionPass(ID) {
				initializeARMLoggerPass(*PassRegistry::getPassRegistry());
			}
			bool runOnMachineFunction(MachineFunction &MF) override; 
			StringRef getPassName() const override {
				return ARM_LOGGER_NAME; 
			}
	};

} // end anonymous namespace
char ARMLogger::ID = 0;

INITIALIZE_PASS (ARMLogger, "arm-logger", ARM_LOGGER_NAME,
				false, false)


FunctionPass *llvm::createARMLoggerPass() {
	return new ARMLogger();
}

bool ARMLogger::runOnMachineFunction(MachineFunction &MF) {

	errs() << "ARM Logger\n";
	errs() << "======" << MF.getName() << "=====\n" ;

	MF.print(errs());

	Function &F = MF.getFunction();
	BasicBlock &BB = F.getEntryBlock();
	Instruction *I = &(BB.front());

	while (I!=NULL) {
		if(const CallBase *Call = dyn_cast<CallBase>(I)) {
			errs() << "Found a call instruction\n";

			Instruction *SysI = I->clone();
			errs() << SysI->getOpcodeName() << "\n";
			//SysI->insertBefore(I);
			//IRBuilder<>(I).CreateRetVoid();
		}
		if(I->isExceptionalTerminator()) {
			errs() << "ExceptionalTerminator()\n";
		}
		
		I = (I->getNextNonDebugInstruction());
		
	}

	for(MachineBasicBlock &MBB : MF) {
		MBB.print(errs());
		errs() << "MBB Loop\n";
		for(MachineInstr &MI : MBB) {
			errs() << "MI Loop\n";
			MI.print(errs());
		}
	}
	

	errs() << "End ARM Logger\n";

	return true;
}
