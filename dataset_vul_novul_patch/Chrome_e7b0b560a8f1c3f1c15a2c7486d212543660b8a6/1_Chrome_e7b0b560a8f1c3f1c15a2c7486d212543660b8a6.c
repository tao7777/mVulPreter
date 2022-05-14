void RequestSender::OnNetworkFetcherComplete(
    const GURL& original_url,
    std::unique_ptr<std::string> response_body,
    int net_error,
    const std::string& header_etag,
    int64_t xheader_retry_after_sec) {
  DCHECK(thread_checker_.CalledOnValidThread());

   VLOG(1) << "request completed from url: " << original_url.spec();
 
   int error = -1;
  if (response_body && response_code_ == 200) {
    DCHECK_EQ(0, net_error);
     error = 0;
  } else if (response_code_ != -1) {
     error = response_code_;
  } else {
     error = net_error;
  }
 
   int retry_after_sec = -1;
   if (original_url.SchemeIsCryptographic() && error > 0)
    retry_after_sec = base::saturated_cast<int>(xheader_retry_after_sec);

  base::ThreadTaskRunnerHandle::Get()->PostTask(
      FROM_HERE, base::BindOnce(&RequestSender::SendInternalComplete,
                                base::Unretained(this), error,
                                response_body ? *response_body : std::string(),
                                header_etag, retry_after_sec));
}
