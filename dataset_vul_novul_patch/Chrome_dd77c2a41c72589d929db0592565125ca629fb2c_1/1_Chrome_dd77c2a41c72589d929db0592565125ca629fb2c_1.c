 void TestFlashMessageLoop::RunTests(const std::string& filter) {
   RUN_TEST(Basics, filter);
   RUN_TEST(RunWithoutQuit, filter);
 }
