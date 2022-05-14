 FileStream::FileStream(const scoped_refptr<base::TaskRunner>& task_runner)
    : context_(base::MakeUnique<Context>(task_runner)) {}
