   void CheckSADs() {
     unsigned int reference_sad, exp_sad[4];
 
     SADs(exp_sad);
    for (int block = 0; block < 4; ++block) {
      reference_sad = ReferenceSAD(block);
 
      EXPECT_EQ(reference_sad, exp_sad[block]) << "block " << block;
     }
   }
