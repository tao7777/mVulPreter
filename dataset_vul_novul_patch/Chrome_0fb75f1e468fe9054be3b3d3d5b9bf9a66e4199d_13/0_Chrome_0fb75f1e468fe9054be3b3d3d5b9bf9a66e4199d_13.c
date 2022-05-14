 Win32StackFrameUnwinder::Win32StackFrameUnwinder()
    : Win32StackFrameUnwinder(make_scoped_ptr(new Win32UnwindFunctions)) {
 }
