void ElementsUploadDataStreamTest::FileChangedHelper(
    const base::FilePath& file_path,
    const base::Time& time,
    bool error_expected) {
   std::vector<std::unique_ptr<UploadElementReader>> element_readers;
  element_readers.push_back(base::MakeUnique<UploadFileElementReader>(
       base::ThreadTaskRunnerHandle::Get().get(), file_path, 1, 2, time));
 
   TestCompletionCallback init_callback;
  std::unique_ptr<UploadDataStream> stream(
      new ElementsUploadDataStream(std::move(element_readers), 0));
  ASSERT_THAT(stream->Init(init_callback.callback(), NetLogWithSource()),
              IsError(ERR_IO_PENDING));
  int error_code = init_callback.WaitForResult();
  if (error_expected)
    ASSERT_THAT(error_code, IsError(ERR_UPLOAD_FILE_CHANGED));
  else
    ASSERT_THAT(error_code, IsOk());
}
