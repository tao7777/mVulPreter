WarmupURLFetcher::WarmupURLFetcher(
    CreateCustomProxyConfigCallback create_custom_proxy_config_callback,
    WarmupURLFetcherCallback callback,
    GetHttpRttCallback get_http_rtt_callback,
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner,
    const std::string& user_agent)
    : is_fetch_in_flight_(false),
      previous_attempt_counts_(0),
      create_custom_proxy_config_callback_(create_custom_proxy_config_callback),
      callback_(callback),
      get_http_rtt_callback_(get_http_rtt_callback),
       user_agent_(user_agent),
       ui_task_runner_(ui_task_runner) {
   DCHECK(create_custom_proxy_config_callback);
 }
