 bool WebMediaPlayerImpl::HasSingleSecurityOrigin() const {
   if (data_source_)
     return data_source_->HasSingleOrigin();
   return true;
}
