 WebRunnerMainDelegate::WebRunnerMainDelegate(zx::channel context_channel)
    : context_channel_(std::move(context_channel)) {}
