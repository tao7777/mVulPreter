  TestResultCallback()
      : callback_(base::Bind(&TestResultCallback::SetResult,
                             base::Unretained(this))) {}
