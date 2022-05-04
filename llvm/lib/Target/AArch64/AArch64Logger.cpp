#include "AArch64MachineFunctionInfo.h"
#include "AArch64Subtarget.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"

#include "llvm/Support/Debug.h"

using namespace llvm;

#define AARCH64_LOGGER_NAME "AArch64 Logger"

namespace {
	class AArch64Logger : public MachineFunctionPass {
		public:
			static char ID;
			AArch64Logger() : MachineFunctionPass(ID) {}
			bool runOnMachineFunction(MachineFunction &MF) override;
			StringRef getPassName() const override {
				return AARCH64_LOGGER_NAME; 
			}
	};
} // end anonymous namespace

char AArch64Logger::ID = 0;

INITIALIZE_PASS (AArch64Logger, "aarch64-logger", AARCH64_LOGGER_NAME,
				false, false)


FunctionPass *llvm::createAArch64LoggerPass() {
	return new AArch64Logger();
}

bool AArch64Logger::runOnMachineFunction(MachineFunction &MF) {

	const TargetLibraryInfo *TLI;
	const AArch64InstrInfo *TII = static_cast<const AArch64InstrInfo *>(MF.getSubtarget().getInstrInfo());

	errs() << "========" << MF.getName().str() << "========\n";
//	Need to find proper places to insert the syscall
	for (MachineBasicBlock &MBB : MF) {
		for(MachineInstr &MI : MBB) {
			if(MI.isCall()) {
				errs() << "\tFound a call Instruction\n";
				//MachineOperand MO = MI.getOperand(0);
				//BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(AArch64::ADR), AArch64::X8).addGlobalAddress(MO.getGlobal());
				//BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(AArch64::BL)).addExternalSymbol("TEE_BtsCall");
				//BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(AArch64::BL)).addExternalSymbol("_utee_bts_call");
				BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(AArch64::SVC)).addImm(71);
			}
			else if(MI.isReturn()) {
				BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(AArch64::SVC)).addImm(72);
			}
		}// End of MI
	}// End of MBB

	return true;
}
