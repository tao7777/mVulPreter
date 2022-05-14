SocketStream::SocketStream(const GURL& url, Delegate* delegate)
     : delegate_(delegate),
       url_(url),
       max_pending_send_allowed_(kMaxPendingSendAllowed),
      context_(NULL),
       next_state_(STATE_NONE),
       factory_(ClientSocketFactory::GetDefaultFactory()),
       proxy_mode_(kDirectConnection),
      proxy_url_(url),
      pac_request_(NULL),
      privacy_mode_(kPrivacyModeDisabled),
      io_callback_(base::Bind(&SocketStream::OnIOCompleted,
                              base::Unretained(this))),
      read_buf_(NULL),
      current_write_buf_(NULL),
      waiting_for_write_completion_(false),
      closing_(false),
      server_closed_(false),
      metrics_(new SocketStreamMetrics(url)) {
  DCHECK(base::MessageLoop::current())
      << "The current base::MessageLoop must exist";
  DCHECK_EQ(base::MessageLoop::TYPE_IO, base::MessageLoop::current()->type())
      << "The current base::MessageLoop must be TYPE_IO";
  DCHECK(delegate_);
}
