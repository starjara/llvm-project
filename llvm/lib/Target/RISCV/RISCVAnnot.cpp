#include "RISCV.h"
#include "RISCVMachineFunctionInfo.h"
#include "RISCVSubtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#include "llvm/Support/Debug.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h" 

using namespace llvm;

#define RISCV_ANNOT_NAME "RISC-V annotation pass"

namespace {

class RISCVAnnot : public MachineFunctionPass {
public:
  static char ID;

  RISCVAnnot() : MachineFunctionPass(ID) {
    initializeRISCVAnnotPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return RISCV_ANNOT_NAME;
  }
};

char RISCVAnnot::ID = 0;

} // end of annonymous name space

bool RISCVAnnot::runOnMachineFunction(MachineFunction &MF) {
  errs() << "Machine Function Pass\n";
  const Function &F = MF.getFunction();
  const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

  for (auto &MBB : MF) {
    for (auto MII = MBB.begin(); MII != MBB.end();) {
      MachineInstr &MI = *MII;
      ++ MII;
     
      /*
      if (const DebugLoc &DL = MI.getDebugLoc()) {
	if (DILocation *DIL = DL.get()) {
	  errs() << "DebugLoc found: "
		 << DIL->getFilename() << ":"
		 << DIL->getLine() << ":"
		 << DIL->getColumn() << "\n";

	  // Process additional debug info or metadata if needed
	  // For example, check for specific metadata node
	  MDNode *MD = DIL->getScope()->getMetadata("annotation_key");
	  if (MD) {
	    errs() << "Found annotation metadata: " << *MD << "\n";
	  }
	}
      }
      */
      
     if (MI.mayLoadOrStore()) {
	for (unsigned i = 0; i < MI.getNumOperands(); ++i) {
	  MachineOperand &MO = MI.getOperand(i);
	  if (MO.isFI()) {
	    const AllocaInst *AI = MF.getFrameInfo().getObjectAllocation(MO.getIndex());
	    MI.print(errs());
	    if (AI && AI->getMetadata("hvmi")) {
	      errs() << "get Annotate\n";
	      if (MI.mayLoad()) {
		errs() << "\tLoad\n";
		MI.print(errs());

		BuildMI(MBB, MII, MI.getDebugLoc(), TII->get(RISCV::HLV_W))
		  .add(MI.getOperand(0))
		  .add(MI.getOperand(1))
		  .add(MI.getOperand(2));

		errs() << "HLV Set\n";
		//		MI.eraseFromParent();
		break;
	      } else if (MI.mayStore()) {
		errs() << "Store\n";

		MI.print(errs());

		unsigned int opcode;

		  switch (MI.getOpcode()) {
		  case RISCV::SB:
		    opcode = RISCV::HSV_B;
		    break;
		  case RISCV::SH:
		    opcode = RISCV::HSV_H;
		    break;
		  case RISCV::SW:
		    opcode = RISCV::HSV_W;
		    break;
		  case RISCV::SD:
		    opcode = RISCV::HSV_D;
		    break;
		  default:
		    errs() << "Unknown\n";
		    opcode = RISCV::HSV_D;
		  }

		
		BuildMI(MBB, MII, MI.getDebugLoc(), TII->get(opcode))
		  .add(MI.getOperand(0))
		  .add(MI.getOperand(1))
		  .add(MI.getOperand(2));

		errs() << "HSV Set\n";
		//MI.eraseFromParent();
		break;
	      }
	    }
	  }
	}
      }
    }
  }
  /*
  for(auto &MBB : MF) {
    for(auto MII = MBB.begin(); MII != MBB.end();) {
      MachineInstr &MI = *MII;
      ++ MII;
      MI.print(errs());
    }
  }
  */

 Found:
  return false;
}

INITIALIZE_PASS(RISCVAnnot, "RISCV Annot", RISCV_ANNOT_NAME, false, false)  

FunctionPass *llvm::createRISCVAnnotPass() {
  return new RISCVAnnot();
}
