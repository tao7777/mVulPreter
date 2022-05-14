void UrlFetcherDownloader::StartURLFetch(const GURL& url) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  if (download_dir_.empty()) {
    Result result;
    result.error = -1;

    DownloadMetrics download_metrics;
    download_metrics.url = url;
    download_metrics.downloader = DownloadMetrics::kUrlFetcher;
    download_metrics.error = -1;
    download_metrics.downloaded_bytes = -1;
    download_metrics.total_bytes = -1;
    download_metrics.download_time_ms = 0;

    main_task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&UrlFetcherDownloader::OnDownloadComplete,
                                  base::Unretained(this), false, result,
                                  download_metrics));
     return;
   }
 
  file_path_ = download_dir_.AppendASCII(url.ExtractFileName());
   network_fetcher_ = network_fetcher_factory_->Create();
   network_fetcher_->DownloadToFile(
      url, file_path_,
       base::BindOnce(&UrlFetcherDownloader::OnResponseStarted,
                      base::Unretained(this)),
       base::BindRepeating(&UrlFetcherDownloader::OnDownloadProgress,
                           base::Unretained(this)),
       base::BindOnce(&UrlFetcherDownloader::OnNetworkFetcherComplete,
                     base::Unretained(this)));
 
   download_start_time_ = base::TimeTicks::Now();
 }
