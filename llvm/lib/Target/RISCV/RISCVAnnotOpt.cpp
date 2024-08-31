#include "RISCV.h"
#include "RISCVMachineFunctionInfo.h"
#include "RISCVSubtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

#define RISCV_ANNOT_OPT_NAME "RISC-V annotation opt pass"

namespace {

class RISCVAnnotOpt : public MachineFunctionPass {
public:
  static char ID;

  RISCVAnnotOpt() : MachineFunctionPass(ID) {
    initializeRISCVAnnotOptPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return RISCV_ANNOT_OPT_NAME;
  }
};

char RISCVAnnotOpt::ID = 0;

} // end of annonymous name space

bool RISCVAnnotOpt::runOnMachineFunction(MachineFunction &MF) {
  errs() << "Machine Function Opt Pass\n";
  const Function &F = MF.getFunction();
  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

  for(auto &MBB : MF) {
    for(auto MII = MBB.begin(); MII != MBB.end();) {
      MachineInstr &MI = *MII;
      ++ MII;
      if(MI.getOpcode() == RISCV::HLV_B || MI.getOpcode() == RISCV::HLV_H || MI.getOpcode() == RISCV::HLV_W || MI.getOpcode() == RISCV::HLV_D ||
	 MI.getOpcode() == RISCV::HSV_B || MI.getOpcode() == RISCV::HSV_H || MI.getOpcode() == RISCV::HSV_W || MI.getOpcode() == RISCV::HSV_D) {
	MI.print(errs());
	BuildMI(MBB, MII, MI.getDebugLoc(), TII->get(RISCV::ADDI))
	  .addReg(MI.getOperand(1).getReg())
	  .addReg(MI.getOperand(1).getReg())
	  .addImm(MI.getOperand(2).getImm());

	switch(MI.getOpcode()) {
	case RISCV::HLV_B:
	case RISCV::HLV_H:
	case RISCV::HLV_W:
	case RISCV::HLV_D:
	  BuildMI(MBB, MII, MI.getDebugLoc(), TII->get(MI.getOpcode()))
	    .addReg(MI.getOperand(0).getReg())
	    .addReg(MI.getOperand(1).getReg());
	  break;
	case RISCV::HSV_B:
	case RISCV::HSV_H:
	case RISCV::HSV_W:
	case RISCV::HSV_D:
	   BuildMI(MBB, MII, MI.getDebugLoc(), TII->get(MI.getOpcode()))
	    .addReg(MI.getOperand(1).getReg())
	    .addReg(MI.getOperand(0).getReg());
	  break;
	}

	BuildMI(MBB, MII, MI.getDebugLoc(), TII->get(RISCV::ADDI))
	  .addReg(MI.getOperand(1).getReg())
	  .addReg(MI.getOperand(1).getReg())
	  .addImm(-(MI.getOperand(2).getImm()));
	
	MI.eraseFromParent();

      }
    }
  }

  return false;
}

INITIALIZE_PASS(RISCVAnnotOpt, "RISCV Annot Opt", RISCV_ANNOT_OPT_NAME, false, false)  

FunctionPass *llvm::createRISCVAnnotOptPass() {
  return new RISCVAnnotOpt();
}
