  TestOpenCallback()
      : callback_(
            base::Bind(&TestOpenCallback::SetResult, base::Unretained(this))) {}
