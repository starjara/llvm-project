// RUN: mlir-opt -split-input-file -transform-interpreter -cse  %s | FileCheck %s

func.func @decompose_dynamic_concat(%arg0 : tensor<8x4xf32>, %arg1 : tensor<?x?xf32>) -> tensor<?x?xf32> {
  %0 = tensor.concat dim(1) %arg0, %arg1 : (tensor<8x4xf32>, tensor<?x?xf32>) -> tensor<?x?xf32>
  return %0 : tensor<?x?xf32>
}
//   CHECK-DAG: #[[$MAP:.+]] = affine_map<()[s0] -> (s0 + 4)>
// CHECK-LABEL: func @decompose_dynamic_concat(
//  CHECK-SAME:     %[[ARG0:.+]]: tensor<8x4xf32>
//  CHECK-SAME:     %[[ARG1:[a-zA-Z0-9]+]]: tensor<?x?xf32>

//   CHECK-DAG:     %[[C8:.+]] = arith.constant 8 : index
//   CHECK-DAG:     %[[C1:.+]] = arith.constant 1 : index
//   CHECK-DAG:     %[[C0:.+]] = arith.constant 0 : index
//       CHECK:     %[[DIM:.+]] = tensor.dim %[[ARG1]], %[[C1]] : tensor<?x?xf32>
//       CHECK:     %[[CONCAT_SIZE:.+]] = affine.apply #[[$MAP]]()[%[[DIM]]]
//       CHECK:     %[[EMPTY:.+]] = tensor.empty(%[[C8]], %[[CONCAT_SIZE]]) : tensor<?x?xf32>
//       CHECK:     %[[SLICE0:.+]] = tensor.insert_slice %[[ARG0]] into %[[EMPTY]][0, 0] [8, 4] [1, 1] : tensor<8x4xf32> into tensor<?x?xf32>
//       CHECK:     %[[OFFSET:.+]] = tensor.dim %[[ARG1]], %[[C0]] : tensor<?x?xf32>
//       CHECK:     %[[CONCAT:.+]] = tensor.insert_slice %[[ARG1]] into %[[SLICE0]][0, 4] [%[[OFFSET]], %[[DIM]]] [1, 1] : tensor<?x?xf32> into tensor<?x?xf32>
//       CHECK:     return %[[CONCAT]] : tensor<?x?xf32>

func.func @decompose_1d_concat(%arg0 : tensor<1xf32>,
                            %arg1 : tensor<2xf32>,
                            %arg2 : tensor<3xf32>,
                            %arg3: tensor<4xf32>) -> tensor<10xf32> {
  %0 = tensor.concat dim(0) %arg0, %arg1, %arg2, %arg3
             : (tensor<1xf32>, tensor<2xf32>, tensor<3xf32>, tensor<4xf32>) -> tensor<10xf32>
  return %0 : tensor<10xf32>
}
// CHECK-LABEL: func @decompose_1d_concat
//       CHECK:    tensor.empty() : tensor<10xf32>
//       CHECK:    tensor.insert_slice %{{.*}}[0] [1] [1] : tensor<1xf32> into tensor<10xf32>
//       CHECK:    tensor.insert_slice %{{.*}}[1] [2] [1] : tensor<2xf32> into tensor<10xf32>
//       CHECK:    tensor.insert_slice %{{.*}}[3] [3] [1] : tensor<3xf32> into tensor<10xf32>
//       CHECK:    %[[CONCAT:.+]] = tensor.insert_slice %{{.*}}[6] [4] [1] : tensor<4xf32> into tensor<10xf32>
//       CHECK:    return %[[CONCAT]] : tensor<10xf32>

func.func @decompose_static_concat_dim(%arg0 : tensor<1x?x64xf32>,
                               %arg1: tensor<1x?x64xf32>) -> tensor<1x?x128xf32> {
  %0 = tensor.concat dim(2) %arg0, %arg1
             : (tensor<1x?x64xf32>, tensor<1x?x64xf32>) -> tensor<1x?x128xf32>
  return %0 : tensor<1x?x128xf32>
}
// CHECK-LABEL: func @decompose_static_concat_dim
//   CHECK-DAG:     %[[C1:.+]] = arith.constant 1 : index
//       CHECK:     %[[DIM:.+]] = tensor.dim %{{.*}}, %[[C1]] : tensor<1x?x64xf32>
//       CHECK:    tensor.empty(%[[DIM]]) : tensor<1x?x128xf32>
//       CHECK:    tensor.insert_slice %{{.*}}[0, 0, 0] [1, %[[DIM]], 64] [1, 1, 1] : tensor<1x?x64xf32> into tensor<1x?x128xf32>
//       CHECK:     %[[DIM1:.+]] = tensor.dim %{{.*}}, %[[C1]] : tensor<1x?x64xf32>
//       CHECK:    %[[CONCAT:.+]] = tensor.insert_slice %{{.*}}[0, 0, 64] [1, %[[DIM1]], 64] [1, 1, 1] : tensor<1x?x64xf32> into tensor<1x?x128xf32>
//       CHECK:    return %[[CONCAT]] : tensor<1x?x128xf32>

module attributes {transform.with_named_sequence} {
  transform.named_sequence @__transform_main(%func_op: !transform.op<"func.func"> {transform.readonly}) {
    transform.apply_patterns to %func_op {
      transform.apply_patterns.tensor.decompose_concat
    } : !transform.op<"func.func">
    transform.yield
  }
}
