 Win32StackFrameUnwinderTest::CreateUnwinder() {
  return make_scoped_ptr(new Win32StackFrameUnwinder(&unwind_functions_));
 }
