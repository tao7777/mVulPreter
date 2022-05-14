   virtual void SetUp() {
     source_stride_ = (width_ + 31) & ~31;
     reference_stride_ = width_ * 2;
     rnd_.Reset(ACMRandom::DeterministicSeed());
   }
