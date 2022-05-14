 MagickExport const char *GetMagickFeatures(void)
 {
   return "DPC"
#if defined(MAGICKCORE_WINDOWS_SUPPORT) && defined(_DEBUG)
  " Debug"
#endif
#if defined(MAGICKCORE_CIPHER_SUPPORT)
  " Cipher"
 #endif
 #if defined(MAGICKCORE_HDRI_SUPPORT)
   " HDRI"
 #endif
#if defined(MAGICKCORE_BUILD_MODULES) || defined(_DLL)
  " Modules"
#endif
 #if defined(MAGICKCORE_OPENCL_SUPPORT)
   " OpenCL"
 #endif
 #if defined(MAGICKCORE_OPENMP_SUPPORT)
   " OpenMP"
#endif
#if defined(ZERO_CONFIGURATION_SUPPORT)
  " Zero-configuration"
 #endif
   ;
 }
