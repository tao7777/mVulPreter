void SetupTests() {
  RegisterTest("TestGraphics3DInterface", TestGraphics3DInterface);
  RegisterTest("TestOpenGLES2Interface", TestOpenGLES2Interface);
   RegisterTest("TestCreate", TestCreate);
   RegisterTest("TestIsGraphics3D", TestIsGraphics3D);
   RegisterTest("Test_glInitializePPAPI", Test_glInitializePPAPI);
   RegisterTest("TestSwapBuffers", TestSwapBuffers);
   RegisterTest("TestResizeBuffersWithoutDepthBuffer",
       TestResizeBuffersWithoutDepthBuffer);
  RegisterTest("TestResizeBuffersWithDepthBuffer",
      TestResizeBuffersWithDepthBuffer);
  RegisterTest("Test_glTerminatePPAPI", Test_glTerminatePPAPI);
}
