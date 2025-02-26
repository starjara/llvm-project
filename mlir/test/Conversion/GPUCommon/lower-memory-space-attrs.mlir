// RUN: mlir-opt %s -split-input-file -convert-gpu-to-rocdl | FileCheck %s --check-prefixes=CHECK,ROCDL
// RUN: mlir-opt %s -split-input-file -convert-gpu-to-nvvm | FileCheck %s --check-prefixes=CHECK,NVVM

gpu.module @kernel {
  gpu.func @private(%arg0: f32) private(%arg1: memref<4xf32, #gpu.address_space<private>>) {
    %c0 = arith.constant 0 : index
    memref.store %arg0, %arg1[%c0] : memref<4xf32, #gpu.address_space<private>>
    gpu.return
  }
}

// CHECK-LABEL:  llvm.func @private
//      CHECK:  llvm.store
// ROCDL-SAME:   : f32, !llvm.ptr<5>
//  NVVM-SAME:   : f32, !llvm.ptr


// -----

gpu.module @kernel {
  gpu.func @workgroup(%arg0: f32) workgroup(%arg1: memref<4xf32, #gpu.address_space<workgroup>>) {
    %c0 = arith.constant 0 : index
    memref.store %arg0, %arg1[%c0] : memref<4xf32, #gpu.address_space<workgroup>>
    gpu.return
  }
}

// CHECK-LABEL:  llvm.func @workgroup
//       CHECK:  llvm.store
//  CHECK-SAME:   : f32, !llvm.ptr<3>

// -----

gpu.module @kernel {
  gpu.func @nested_memref(%arg0: memref<4xmemref<4xf32, #gpu.address_space<global>>, #gpu.address_space<global>>) -> f32 {
    %c0 = arith.constant 0 : index
    %inner = memref.load %arg0[%c0] : memref<4xmemref<4xf32, #gpu.address_space<global>>, #gpu.address_space<global>>
    %value = memref.load %inner[%c0] : memref<4xf32, #gpu.address_space<global>>
    gpu.return %value : f32
  }
}

// CHECK-LABEL:  llvm.func @nested_memref
//       CHECK:  llvm.load
//  CHECK-SAME:   : !llvm.ptr<1>
//       CHECK: [[value:%.+]] = llvm.load
//  CHECK-SAME:   : !llvm.ptr<1> -> f32
//       CHECK: llvm.return [[value]]

// -----

gpu.module @kernel {
  gpu.func @dynamic_shmem(%arg0: f32)  {
    %0 = arith.constant 0 : index
    %1 = gpu.dynamic_shared_memory : memref<?xi8, #gpu.address_space<workgroup>>
    %2 = memref.view %1[%0][] : memref<?xi8, #gpu.address_space<workgroup>> to memref<4xf32, #gpu.address_space<workgroup>>
    memref.store %arg0, %2[%0] : memref<4xf32, #gpu.address_space<workgroup>>
    gpu.return
  }
}

// CHECK-LABEL:  llvm.func @dynamic_shmem
//       CHECK:  llvm.store
//  CHECK-SAME:   : f32, !llvm.ptr<3>

