  void CheckSad(unsigned int max_sad) {
    unsigned int reference_sad, exp_sad;
    reference_sad = ReferenceSAD(max_sad);
    exp_sad = SAD(max_sad);
    if (reference_sad <= max_sad) {
      ASSERT_EQ(exp_sad, reference_sad);
    } else {
      ASSERT_GE(exp_sad, reference_sad);
    }
  }
