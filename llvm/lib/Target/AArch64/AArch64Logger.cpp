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

	for (MachineBasicBlock &MBB : MF) {
		if(&MBB == &*MF.begin()) {
			errs() << "Begin of the Machine Function\n";

			const AArch64InstrInfo *TII = static_cast<const AArch64InstrInfo *>(
				MBB.getParent()->getSubtarget().getInstrInfo());

			BuildMI(MBB, MBB.begin(), MBB.findDebugLoc(MBB.begin()), 
				TII->get(AArch64::HINT)).addImm(0);
				
		}
	}

	return true;
}
