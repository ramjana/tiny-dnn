/*
    Copyright (c) 2016, Taiga Nomi
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "tiny_dnn/layers/layer.h"
#include "tiny_dnn/activations/activation_function.h"

namespace tiny_dnn {

/**
 * single-input, single-output network with activation function
 **/
template<typename Activation>
class activation_layer : public layer {
public:
    activation_layer(const shape3d& shape) : layer({vector_type::data}, { vector_type::data }), shape_(shape) {}

    std::pair<float_t, float_t> out_value_range() const override { return h_.scale(); }

    std::vector<shape3d> in_shape() const override {
        return shape_;
    }

    std::vector<shape3d> out_shape() const override {
        return shape_;
    }

    std::string layer_type() const override {
        return "activation";
    }

	void forward_propagation(const std::vector<tensor_t*>& in_data,
		                     std::vector<tensor_t*>& out_data) override {
		assert(in_data.size() == out_data.size());
        forward_activation(*in_data[0], *out_data[0], h_);
    }

    void back_propagation(const std::vector<tensor_t*>& in_data,
                          const std::vector<tensor_t*>& out_data,
                          std::vector<tensor_t*>&       out_grad,
                          std::vector<tensor_t*>&       in_grad) override {
        assert(in_data.size() == out_data.size());
        backward_activation(*out_grad[0], *out_data[0], *in_grad[0], h_);
    }

    bool one_hot() const {
        return h_.one_hot();
    }

private:
    shape3d shape_;
    Activation h_;
};

} // namespace tiny_dnn
