std::string TestURLLoader::TestUntendedLoad() {
  pp::URLRequestInfo request(instance_);
  request.SetURL("test_url_loader_data/hello.txt");
  request.SetRecordDownloadProgress(true);
  TestCompletionCallback callback(instance_->pp_instance(), callback_type());

  pp::URLLoader loader(instance_);
  callback.WaitForResult(loader.Open(request, callback.GetCallback()));
  CHECK_CALLBACK_BEHAVIOR(callback);
  ASSERT_EQ(PP_OK, callback.result());

  int64_t bytes_received = 0;
  int64_t total_bytes_to_be_received = 0;
  while (true) {
     loader.GetDownloadProgress(&bytes_received, &total_bytes_to_be_received);
     if (total_bytes_to_be_received <= 0)
       return ReportError("URLLoader::GetDownloadProgress total size",
                         static_cast<int32_t>(total_bytes_to_be_received));
     if (bytes_received == total_bytes_to_be_received)
       break;
    if (pp::Module::Get()->core()->IsMainThread()) {
      NestedEvent event(instance_->pp_instance());
      event.PostSignal(10);
      event.Wait();
    }
  }
  std::string body;
  std::string error = ReadEntireResponseBody(&loader, &body);
  if (!error.empty())
    return error;
  if (body != "hello\n")
    return ReportError("Couldn't read data", callback.result());

  PASS();
}
