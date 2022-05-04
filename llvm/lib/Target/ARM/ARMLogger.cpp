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
	const TargetInstrInfo *XII= MF.getSubtarget().getInstrInfo();// target instruction info
	DebugLoc DL;

	errs() << "ARM Logger\n";
	errs() << "======" << MF.getName() << "=====\n" ;

	for(MachineBasicBlock &MBB : MF) {
		for(MachineInstr &MI : MBB) {
			if(MI.isCall()) {
				DL = MI.getDebugLoc();
				errs() << "\tFound a call Instruction\n";
				BuildMI(MBB, MI, DL, XII->get(ARM::SVC)).addImm(77U);
			}
			else if(MI.isReturn()) {
				errs() << "\tFound a return Instruction\n";
			}
			else if(MI.isBranch()) {
				errs() << "\tFound a branch Instruction\n";
			}
		}
	}
	

	errs() << "End ARM Logger\n";

	return true;
}
