   virtual void TearDown() {
    delete[] src_;
    delete[] ref_;
     libvpx_test::ClearSystemState();
   }
