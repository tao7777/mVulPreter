  TestCompletionCallback()
      : callback_(base::Bind(&TestCompletionCallback::SetResult,
                             base::Unretained(this))) {}
