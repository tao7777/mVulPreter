void DownloadManagerImpl::DownloadUrl(
    std::unique_ptr<download::DownloadUrlParameters> params,
    std::unique_ptr<storage::BlobDataHandle> blob_data_handle,
    scoped_refptr<network::SharedURLLoaderFactory> blob_url_loader_factory) {
  if (params->post_id() >= 0) {
    DCHECK(params->prefer_cache());
    DCHECK_EQ("POST", params->method());
  }

  download::RecordDownloadCountWithSource(
      download::DownloadCountTypes::DOWNLOAD_TRIGGERED_COUNT,
       params->download_source());
   auto* rfh = RenderFrameHost::FromID(params->render_process_host_id(),
                                       params->render_frame_host_routing_id());
   BeginDownloadInternal(std::move(params), std::move(blob_data_handle),
                         std::move(blob_url_loader_factory), true,
                         rfh ? rfh->GetSiteInstance()->GetSiteURL() : GURL());
}
