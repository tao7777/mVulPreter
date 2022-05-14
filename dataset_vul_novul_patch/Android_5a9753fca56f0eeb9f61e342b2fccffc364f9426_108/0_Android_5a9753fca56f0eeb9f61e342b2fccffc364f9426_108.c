int main(int argc, char **argv) {
 ::testing::InitGoogleTest(&argc, argv);


 #if ARCH_X86 || ARCH_X86_64
   const int simd_caps = x86_simd_caps();
   if (!(simd_caps & HAS_MMX))
    append_negative_gtest_filter(":MMX.*:MMX/*");
   if (!(simd_caps & HAS_SSE))
    append_negative_gtest_filter(":SSE.*:SSE/*");
   if (!(simd_caps & HAS_SSE2))
    append_negative_gtest_filter(":SSE2.*:SSE2/*");
   if (!(simd_caps & HAS_SSE3))
    append_negative_gtest_filter(":SSE3.*:SSE3/*");
   if (!(simd_caps & HAS_SSSE3))
    append_negative_gtest_filter(":SSSE3.*:SSSE3/*");
   if (!(simd_caps & HAS_SSE4_1))
    append_negative_gtest_filter(":SSE4_1.*:SSE4_1/*");
   if (!(simd_caps & HAS_AVX))
    append_negative_gtest_filter(":AVX.*:AVX/*");
   if (!(simd_caps & HAS_AVX2))
    append_negative_gtest_filter(":AVX2.*:AVX2/*");
 #endif
 
 #if !CONFIG_SHARED

 
 #if CONFIG_VP8
   vp8_rtcd();
#endif  // CONFIG_VP8
 #if CONFIG_VP9
   vp9_rtcd();
#endif  // CONFIG_VP9
  vpx_dsp_rtcd();
  vpx_scale_rtcd();
#endif  // !CONFIG_SHARED
 
   return RUN_ALL_TESTS();
 }
