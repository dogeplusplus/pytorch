#include "lazy_tensor_core/csrc/ops/all_reduce.h"

#include <torch/csrc/lazy/core/internal_ops/ltc_ops.h>
#include <torch/csrc/lazy/core/util.h>

#include "lazy_tensor_core/csrc/ts_backend/ts_shape_inference.h"

namespace torch_lazy_tensors {
namespace ir {
namespace ops {
namespace {

std::vector<torch::lazy::Value> GetOperandList(
    torch::lazy::OpList operands, const torch::lazy::Value& token) {
  std::vector<torch::lazy::Value> operand_list(operands.begin(), operands.end());
  operand_list.push_back(token);
  return operand_list;
}

}  // namespace

AllReduce::AllReduce(AllReduceType reduce_type, torch::lazy::OpList operands,
                     const torch::lazy::Value& token, double scale,
                     std::vector<std::vector<int64_t>> groups)
    : torch::lazy::TsNode(
          torch::lazy::ltc_cross_replica_sum, GetOperandList(operands, token),
          {compiler::InferShape(this)},
          /*num_outputs=*/operands.size() + 1,
          torch::lazy::MHash(torch::lazy::GetEnumValue(reduce_type), scale,
                             groups)),
      reduce_type_(reduce_type),
      scale_(scale),
      groups_(std::move(groups)) {}

std::string AllReduce::ToString() const {
  std::stringstream ss;
  ss << torch::lazy::TsNode::ToString()
     << ", reduce_type=" << torch::lazy::GetEnumValue(reduce_type_)
     << ", scale=" << scale_ << ", groups=(";
  for (size_t i = 0; i < groups_.size(); ++i) {
    ss << (i == 0 ? "(" : ",(");
    ss << c10::Join(", ", groups_[i]) << ")";
  }
  ss << ")";
  return ss.str();
}

}  // namespace ops
}  // namespace ir
}  // namespace torch_lazy_tensors
