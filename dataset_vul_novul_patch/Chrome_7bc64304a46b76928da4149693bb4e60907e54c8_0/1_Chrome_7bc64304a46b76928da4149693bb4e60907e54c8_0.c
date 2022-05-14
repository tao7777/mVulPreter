void ChromeRenderMessageFilter::OnWriteTcmallocHeapProfile(
    const FilePath::StringType& filepath,
    const std::string& output) {
  VLOG(0) << "Writing renderer heap profile dump to: " << filepath;
  file_util::WriteFile(FilePath(filepath), output.c_str(), output.size());
}
