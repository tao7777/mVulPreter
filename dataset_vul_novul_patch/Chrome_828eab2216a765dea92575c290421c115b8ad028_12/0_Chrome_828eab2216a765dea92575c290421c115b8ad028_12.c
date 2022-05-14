void StoreAccumulatedContentLength(int received_content_length,
void StoreAccumulatedContentLength(
    int received_content_length,
    int original_content_length,
    chrome_browser_net::DataReductionRequestType data_reduction_type) {
   BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
       base::Bind(&UpdateContentLengthPrefs,
                  received_content_length, original_content_length,
                 data_reduction_type));
 }
