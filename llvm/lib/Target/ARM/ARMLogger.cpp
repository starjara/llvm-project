#include "ARM.h"
#include "ARMMachineFunctionInfo.h"
#include "ARMSubtarget.h"
#include "ARMInstrInfo.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"

#include "llvm/Support/Debug.h"

using namespace llvm;

#define ARM_LOGGER_NAME "ARM Logger"

namespace {
	class ARMLogger : public MachineFunctionPass {
		public:
			static char ID;
			ARMLogger() : MachineFunctionPass(ID) {}
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

	for (MachineBasicBlock &MBB : MF) {
		if(&MBB == &*MF.begin()) {
			errs() << "Begin of the Machine Function\n";

			const ARMInstrInfo *TII = static_cast<const ARMInstrInfo *>(
				MBB.getParent()->getSubtarget().getInstrInfo());

			BuildMI(MBB, MBB.begin(), MBB.findDebugLoc(MBB.begin()), 
				TII->get(ARM::HINT)).addImm(0);
				
		}
	}

	return true;
}
