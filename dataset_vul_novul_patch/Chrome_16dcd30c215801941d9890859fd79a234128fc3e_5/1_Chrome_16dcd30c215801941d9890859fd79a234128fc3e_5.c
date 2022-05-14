content::DownloadFile* MockDownloadFileFactory::CreateFile(
    DownloadCreateInfo* info,
    scoped_ptr<content::ByteStreamReader> stream,
    content::DownloadManager* download_manager,
     bool calculate_hash,
     const net::BoundNetLog& bound_net_log) {
   DCHECK(files_.end() == files_.find(info->download_id));
  MockDownloadFile* created_file = new MockDownloadFile();
   files_[info->download_id] = created_file;
 
   ON_CALL(*created_file, GetDownloadManager())
      .WillByDefault(Return(download_manager));
  EXPECT_CALL(*created_file, Initialize());

  return created_file;
}
