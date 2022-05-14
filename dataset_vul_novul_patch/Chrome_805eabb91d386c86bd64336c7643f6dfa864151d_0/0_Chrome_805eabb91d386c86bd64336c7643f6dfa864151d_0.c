size_t CancelableFileOperation(Function operation,
                               HANDLE file,
                               BufferType* buffer,
                               size_t length,
                               WaitableEvent* io_event,
                               WaitableEvent* cancel_event,
                               CancelableSyncSocket* socket,
                               DWORD timeout_in_ms) {
  ThreadRestrictions::AssertIOAllowed();
  COMPILE_ASSERT(sizeof(buffer[0]) == sizeof(char), incorrect_buffer_type);
  DCHECK_GT(length, 0u);
  DCHECK_LE(length, kMaxMessageLength);
  DCHECK_NE(file, SyncSocket::kInvalidHandle);

  TimeTicks current_time, finish_time;
  if (timeout_in_ms != INFINITE) {
    current_time = TimeTicks::Now();
    finish_time =
        current_time + base::TimeDelta::FromMilliseconds(timeout_in_ms);
  }

  size_t count = 0;
  do {
    OVERLAPPED ol = { 0 };
    ol.hEvent = io_event->handle();

    const DWORD chunk = GetNextChunkSize(count, length);
    DWORD len = 0;
    const BOOL operation_ok = operation(
        file, static_cast<BufferType*>(buffer) + count, chunk, &len, &ol);
    if (!operation_ok) {
       if (::GetLastError() == ERROR_IO_PENDING) {
         HANDLE events[] = { io_event->handle(), cancel_event->handle() };
         const int wait_result = WaitForMultipleObjects(
            arraysize(events), events, FALSE,
             timeout_in_ms == INFINITE ?
                 timeout_in_ms :
                 static_cast<DWORD>(
                    (finish_time - current_time).InMilliseconds()));
        if (wait_result != WAIT_OBJECT_0 + 0) {
          CancelIo(file);
        }

        if (!GetOverlappedResult(file, &ol, &len, TRUE))
          len = 0;

        if (wait_result == WAIT_OBJECT_0 + 1) {
          DVLOG(1) << "Shutdown was signaled. Closing socket.";
          socket->Close();
          return count;
        }

        DCHECK(wait_result == WAIT_OBJECT_0 + 0 || wait_result == WAIT_TIMEOUT);
      } else {
        break;
      }
    }

    count += len;

    if (len != chunk)
      break;

    if (timeout_in_ms != INFINITE && count < length)
      current_time = base::TimeTicks::Now();
  } while (count < length &&
           (timeout_in_ms == INFINITE || current_time < finish_time));

  return count;
}
