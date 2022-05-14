void FileSystemManagerImpl::CreateWriter(const GURL& file_path,
                                          CreateWriterCallback callback) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
 
  if (!base::FeatureList::IsEnabled(blink::features::kWritableFilesAPI)) {
    bindings_.ReportBadMessage("FileSystemManager.CreateWriter");
    return;
  }

   FileSystemURL url(context_->CrackURL(file_path));
   base::Optional<base::File::Error> opt_error = ValidateFileSystemURL(url);
   if (opt_error) {
    std::move(callback).Run(opt_error.value(), nullptr);
    return;
  }
  if (!security_policy_->CanWriteFileSystemFile(process_id_, url)) {
    std::move(callback).Run(base::File::FILE_ERROR_SECURITY, nullptr);
    return;
  }

  blink::mojom::FileWriterPtr writer;
  mojo::MakeStrongBinding(std::make_unique<storage::FileWriterImpl>(
                              url, context_->CreateFileSystemOperationRunner(),
                              blob_storage_context_->context()->AsWeakPtr()),
                          MakeRequest(&writer));
  std::move(callback).Run(base::File::FILE_OK, std::move(writer));
}
