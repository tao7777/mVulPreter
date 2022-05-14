void RecordResourceCompletionUMA(bool image_complete,
                                 bool css_complete,
                                 bool xhr_complete) {
  base::UmaHistogramBoolean("OfflinePages.Background.ResourceCompletion.Image",
                            image_complete);
  base::UmaHistogramBoolean("OfflinePages.Background.ResourceCompletion.Css",
                            css_complete);
  base::UmaHistogramBoolean("OfflinePages.Background.ResourceCompletion.Xhr",
                            xhr_complete);
}
