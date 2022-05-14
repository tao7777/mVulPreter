  ConvolveFunctions(convolve_fn_t h8, convolve_fn_t h8_avg,
                    convolve_fn_t v8, convolve_fn_t v8_avg,
                    convolve_fn_t hv8, convolve_fn_t hv8_avg)
      : h8_(h8), v8_(v8), hv8_(hv8), h8_avg_(h8_avg), v8_avg_(v8_avg),
        hv8_avg_(hv8_avg) {}
