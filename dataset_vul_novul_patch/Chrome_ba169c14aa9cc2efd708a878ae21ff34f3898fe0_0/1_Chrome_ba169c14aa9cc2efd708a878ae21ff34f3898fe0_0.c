void DOMStorageContextWrapper::OpenSessionStorage(
    int process_id,
    const std::string& namespace_id,
    mojo::ReportBadMessageCallback bad_message_callback,
     blink::mojom::SessionStorageNamespaceRequest request) {
   if (!mojo_session_state_)
     return;
  mojo_task_runner_->PostTask(
       FROM_HERE,
       base::BindOnce(&SessionStorageContextMojo::OpenSessionStorage,
                      base::Unretained(mojo_session_state_), process_id,
                     namespace_id, std::move(bad_message_callback),
                      std::move(request)));
 }
