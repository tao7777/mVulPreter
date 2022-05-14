HRESULT WaitForLoginUIAndGetResult(
    CGaiaCredentialBase::UIProcessInfo* uiprocinfo,
    std::string* json_result,
    DWORD* exit_code,
    BSTR* status_text) {
  LOGFN(INFO);
  DCHECK(uiprocinfo);
  DCHECK(json_result);
  DCHECK(exit_code);
  DCHECK(status_text);

  const int kBufferSize = 4096;
  std::vector<char> output_buffer(kBufferSize, '\0');
  base::ScopedClosureRunner zero_buffer_on_exit(
      base::BindOnce(base::IgnoreResult(&RtlSecureZeroMemory),
                     &output_buffer[0], kBufferSize));

  HRESULT hr = WaitForProcess(uiprocinfo->procinfo.process_handle(),
                              uiprocinfo->parent_handles, exit_code,
                               &output_buffer[0], kBufferSize);
  LOGFN(INFO) << "exit_code=" << exit_code;
 
   if (*exit_code == kUiecAbort) {
     LOGFN(ERROR) << "Aborted hr=" << putHR(hr);
    return E_ABORT;
  } else if (*exit_code != kUiecSuccess) {
    LOGFN(ERROR) << "Error hr=" << putHR(hr);
    *status_text =
        CGaiaCredentialBase::AllocErrorString(IDS_INVALID_UI_RESPONSE_BASE);
    return E_FAIL;
  }

  *json_result = std::string(&output_buffer[0]);
  return S_OK;
}
