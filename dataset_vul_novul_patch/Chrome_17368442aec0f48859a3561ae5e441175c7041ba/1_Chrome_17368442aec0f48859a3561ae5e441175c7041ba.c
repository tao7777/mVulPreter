download::DownloadItemImpl* DownloadManagerImpl::CreateActiveItem(
     uint32_t id,
     const download::DownloadCreateInfo& info) {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
  DCHECK(!base::ContainsKey(downloads_, id));
   download::DownloadItemImpl* download =
       item_factory_->CreateActiveItem(this, id, info);
 
  downloads_[id] = base::WrapUnique(download);
  downloads_by_guid_[download->GetGuid()] = download;
  DownloadItemUtils::AttachInfo(
      download, GetBrowserContext(),
      WebContentsImpl::FromRenderFrameHostID(info.render_process_id,
                                             info.render_frame_id));
  return download;
}
