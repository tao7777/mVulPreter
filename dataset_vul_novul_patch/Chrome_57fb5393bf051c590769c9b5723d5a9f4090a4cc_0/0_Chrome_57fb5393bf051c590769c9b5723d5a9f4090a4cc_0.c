 NetworkReaderProxy::NetworkReaderProxy(
    int64 offset,
     int64 content_length,
     const base::Closure& job_canceller)
    : remaining_offset_(offset),
      remaining_content_length_(content_length),
       error_code_(net::OK),
       buffer_length_(0),
       job_canceller_(job_canceller) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
}
