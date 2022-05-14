void NetworkReaderProxy::OnGetContent(scoped_ptr<std::string> data) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
   DCHECK(data && !data->empty());
 
   pending_data_.push_back(data.release());
   if (!buffer_) {
    return;
  }

  int result = ReadInternal(&pending_data_, buffer_.get(), buffer_length_);
  remaining_content_length_ -= result;
  DCHECK_GE(remaining_content_length_, 0);

  buffer_ = NULL;
  buffer_length_ = 0;
  DCHECK(!callback_.is_null());
  base::ResetAndReturn(&callback_).Run(result);
}
