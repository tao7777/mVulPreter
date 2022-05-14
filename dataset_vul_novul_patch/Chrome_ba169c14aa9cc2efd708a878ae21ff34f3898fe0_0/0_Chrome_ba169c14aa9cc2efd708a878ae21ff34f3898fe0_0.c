void DOMStorageContextWrapper::OpenSessionStorage(
    int process_id,
    const std::string& namespace_id,
    mojo::ReportBadMessageCallback bad_message_callback,
     blink::mojom::SessionStorageNamespaceRequest request) {
   if (!mojo_session_state_)
     return;
  // The bad message callback must be called on the same sequenced task runner
  // as the binding set. It cannot be called from our own mojo task runner.
  auto wrapped_bad_message_callback = base::BindOnce(
      [](mojo::ReportBadMessageCallback bad_message_callback,
         scoped_refptr<base::SequencedTaskRunner> bindings_runner,
         const std::string& error) {
        bindings_runner->PostTask(
            FROM_HERE, base::BindOnce(std::move(bad_message_callback), error));
      },
      std::move(bad_message_callback), base::SequencedTaskRunnerHandle::Get());
  mojo_task_runner_->PostTask(
       FROM_HERE,
       base::BindOnce(&SessionStorageContextMojo::OpenSessionStorage,
                      base::Unretained(mojo_session_state_), process_id,
                     namespace_id, std::move(wrapped_bad_message_callback),
                      std::move(request)));
 }
