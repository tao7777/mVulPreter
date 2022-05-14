int TestOpenProcess(DWORD process_id) {
int TestOpenProcess(DWORD process_id, DWORD access_mask) {
  HANDLE process = ::OpenProcess(access_mask,
                                  FALSE,  // Do not inherit handle.
                                  process_id);
   if (NULL == process) {
    if (ERROR_ACCESS_DENIED == ::GetLastError()) {
      return SBOX_TEST_DENIED;
    } else {
      return SBOX_TEST_FAILED_TO_EXECUTE_COMMAND;
    }
  } else {
    ::CloseHandle(process);
    return SBOX_TEST_SUCCEEDED;
  }
}
