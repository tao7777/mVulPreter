void SetDelegateOnIO(content::ResourceDispatcherHostDelegate* new_delegate) {
  content::ResourceDispatcherHost::Get()->SetDelegate(new_delegate);
}
