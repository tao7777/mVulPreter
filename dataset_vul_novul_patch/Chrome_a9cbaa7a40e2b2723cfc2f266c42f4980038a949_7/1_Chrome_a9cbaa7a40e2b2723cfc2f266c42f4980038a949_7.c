UrlData::UrlData(const GURL& url, CORSMode cors_mode, UrlIndex* url_index)
    : url_(url),
      have_data_origin_(false),
      cors_mode_(cors_mode),
      url_index_(url_index),
       length_(kPositionNotSpecified),
       range_supported_(false),
       cacheable_(false),
      has_opaque_data_(false),
       last_used_(),
       multibuffer_(this, url_index_->block_shift_) {}
