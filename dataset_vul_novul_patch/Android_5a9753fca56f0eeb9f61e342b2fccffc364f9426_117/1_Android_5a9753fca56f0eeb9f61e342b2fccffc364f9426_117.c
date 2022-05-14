   virtual void TearDown() {
     vpx_free(src_);
     delete[] ref_;
    vpx_free(sec_);
     libvpx_test::ClearSystemState();
   }
