/*
    Copyright (c) 2013, Taiga Nomi
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
#include "tiny_dnn/util/util.h"
#include "tiny_dnn/activations/activation_function.h"
#include <algorithm>

namespace tiny_dnn {

/**
 * operation with activation
 */
template <typename Op, typename Activation>
class composite_layer : public Op {
public:
    using Op::Op;
    using Op::load_and_construct;
    using Op::serialize;

    void forward_propagation(const std::vector<tensor_t*>& in_data,
                             std::vector<tensor_t*>& out_data) override {
        Op::forward_propagation(in_data, out_data);

        if (a.empty()) {
            init_temporary_buf(out_data[0]->size());
        }
        a = *out_data[0];

        activation::forward_activation(*out_data[0], a, h_);
    }

    void back_propagation(const std::vector<tensor_t*>& in_data,
                          const std::vector<tensor_t*>& out_data,
                          std::vector<tensor_t*>&       out_grad,
                          std::vector<tensor_t*>&       in_grad) override {
 
        activation::backward_activation(*out_grad[0], *out_data[0], da, h_);

        if (da_ptr.size() < out_grad.size()) {
            grow(out_grad, &da_ptr);
            grow(out_data, &a_ptr);
        }
        Op::back_propagation(in_data, a_ptr, da_ptr, in_grad);
    }

    activation::function& activation_function() { return h_; }
    std::pair<float_t, float_t> out_value_range() const override { return h_.scale(); }

    void set_sample_count(serial_size_t sample_count) override {
        Op::set_sample_count(sample_count);
        init_temporary_buf(sample_count);
    }
private:
    void grow(const std::vector<tensor_t*>& src, std::vector<tensor_t*>* dst) {
        dst->resize(src.size());
        for (size_t i = 1; i < dst->size(); i++) {
            (*dst)[i] = src[i];
        }
    }

    void init_temporary_buf(size_t num_samples) {
        size_t length = this->out_shape()[0].size();
        a.resize(num_samples);
        da.resize(num_samples);

        for (size_t i = 0; i < num_samples; i++) {
            a[i].resize(length);
            da[i].resize(length);
        }

        a_ptr.resize(1);
        da_ptr.resize(1);
        a_ptr[0] = &a;
        da_ptr[0] = &da;
    }

    tensor_t a;
    tensor_t da;
    std::vector<tensor_t*> a_ptr;
    std::vector<tensor_t*> da_ptr;
    Activation h_;
};

/*
 If we split activation from conv layer, 
 we can use composite_layer as an equivalent of conv<activation>
 
 template <class T>
 using conv_ = composite_layer<conv, T>

 void a() {
     network<sequential> net;

     // v1.0.0 style
     net << conv(...) << tan_h();

     // old style
     net << conv_<tan_h>(...);
 }

*/

} // namespace tiny_dnn
