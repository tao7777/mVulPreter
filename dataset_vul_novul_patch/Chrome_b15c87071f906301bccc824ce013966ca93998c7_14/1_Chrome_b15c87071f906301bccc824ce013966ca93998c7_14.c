WtsSessionProcessDelegate::Core::Core(
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> io_task_runner,
    const FilePath& binary_path,
    bool launch_elevated,
    const std::string& channel_security)
    : main_task_runner_(main_task_runner),
       io_task_runner_(io_task_runner),
       binary_path_(binary_path),
       channel_security_(channel_security),
       launch_elevated_(launch_elevated),
       stopping_(false) {
   DCHECK(main_task_runner_->BelongsToCurrentThread());
}
