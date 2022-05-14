void UrlFetcherDownloader::OnNetworkFetcherComplete(base::FilePath file_path,
void UrlFetcherDownloader::OnNetworkFetcherComplete(int net_error,
                                                     int64_t content_size) {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
 
  const base::TimeTicks download_end_time(base::TimeTicks::Now());
  const base::TimeDelta download_time =
      download_end_time >= download_start_time_
          ? download_end_time - download_start_time_
          : base::TimeDelta();

   int error = -1;
  if (!net_error && response_code_ == 200)
     error = 0;
  else if (response_code_ != -1)
     error = response_code_;
  else
     error = net_error;
 
   const bool is_handled = error == 0 || IsHttpServerError(error);
 
   Result result;
   result.error = error;
  if (!error)
    result.response = file_path_;
 
   DownloadMetrics download_metrics;
   download_metrics.url = url();
  download_metrics.downloader = DownloadMetrics::kUrlFetcher;
  download_metrics.error = error;
  download_metrics.downloaded_bytes = error ? -1 : content_size;
  download_metrics.total_bytes = total_bytes_;
  download_metrics.download_time_ms = download_time.InMilliseconds();

  VLOG(1) << "Downloaded " << content_size << " bytes in "
          << download_time.InMilliseconds() << "ms from " << url().spec()
          << " to " << result.response.value();

  if (error && !download_dir_.empty()) {
    base::PostTask(FROM_HERE, kTaskTraits,
                   base::BindOnce(IgnoreResult(&base::DeleteFileRecursively),
                                  download_dir_));
  }

  main_task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&UrlFetcherDownloader::OnDownloadComplete,
                                base::Unretained(this), is_handled, result,
                                download_metrics));
}
