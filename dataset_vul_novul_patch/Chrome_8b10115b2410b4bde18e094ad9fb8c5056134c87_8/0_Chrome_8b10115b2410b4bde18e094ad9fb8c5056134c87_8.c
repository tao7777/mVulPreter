 std::unique_ptr<HttpResponse> HandleFileRequest(
     const base::FilePath& server_root,
    const HttpRequest& request) {
  base::ScopedAllowBlockingForTesting allow_blocking;

  GURL request_url = request.GetURL();
  std::string relative_path(request_url.path());

  std::string post_prefix("/post/");
  if (base::StartsWith(relative_path, post_prefix,
                       base::CompareCase::SENSITIVE)) {
    if (request.method != METHOD_POST)
      return nullptr;
    relative_path = relative_path.substr(post_prefix.size() - 1);
  }

  RequestQuery query = ParseQuery(request_url);

  std::unique_ptr<BasicHttpResponse> failed_response(new BasicHttpResponse);
  failed_response->set_code(HTTP_NOT_FOUND);

  if (query.find("expected_body") != query.end()) {
    if (request.content.find(query["expected_body"].front()) ==
        std::string::npos) {
      return std::move(failed_response);
    }
  }

  if (query.find("expected_headers") != query.end()) {
    for (const auto& header : query["expected_headers"]) {
      if (header.find(":") == std::string::npos)
        return std::move(failed_response);
      std::string key = header.substr(0, header.find(":"));
      std::string value = header.substr(header.find(":") + 1);
      if (request.headers.find(key) == request.headers.end() ||
          request.headers.at(key) != value) {
        return std::move(failed_response);
      }
    }
  }

  DCHECK(base::StartsWith(relative_path, "/", base::CompareCase::SENSITIVE));
  std::string request_path = relative_path.substr(1);
  base::FilePath file_path(server_root.AppendASCII(request_path));
  std::string file_contents;
  if (!base::ReadFileToString(file_path, &file_contents)) {
    file_path = file_path.AppendASCII("index.html");
    if (!base::ReadFileToString(file_path, &file_contents))
      return nullptr;
  }

   if (request.method == METHOD_HEAD)
     file_contents = "";
 
  if (!UpdateReplacedText(query, &file_contents))
    return std::move(failed_response);
 
   base::FilePath::StringPieceType mock_headers_extension;
 #if defined(OS_WIN)
  base::string16 temp = base::ASCIIToUTF16(kMockHttpHeadersExtension);
  mock_headers_extension = temp;
#else
  mock_headers_extension = kMockHttpHeadersExtension;
#endif

  base::FilePath headers_path(file_path.AddExtension(mock_headers_extension));

   if (base::PathExists(headers_path)) {
     std::string headers_contents;
 
    if (!base::ReadFileToString(headers_path, &headers_contents) ||
        !UpdateReplacedText(query, &headers_contents)) {
       return nullptr;
    }
 
     return std::make_unique<RawHttpResponse>(headers_contents, file_contents);
   }

  std::unique_ptr<BasicHttpResponse> http_response(new BasicHttpResponse);
  http_response->set_code(HTTP_OK);

  if (request.headers.find("Range") != request.headers.end()) {
    std::vector<HttpByteRange> ranges;

    if (HttpUtil::ParseRangeHeader(request.headers.at("Range"), &ranges) &&
        ranges.size() == 1) {
      ranges[0].ComputeBounds(file_contents.size());
      size_t start = ranges[0].first_byte_position();
      size_t end = ranges[0].last_byte_position();

      http_response->set_code(HTTP_PARTIAL_CONTENT);
      http_response->AddCustomHeader(
          "Content-Range",
          base::StringPrintf("bytes %" PRIuS "-%" PRIuS "/%" PRIuS, start, end,
                             file_contents.size()));

      file_contents = file_contents.substr(start, end - start + 1);
    }
  }

  http_response->set_content_type(GetContentType(file_path));
  http_response->AddCustomHeader("Accept-Ranges", "bytes");
  http_response->AddCustomHeader("ETag", "'" + file_path.MaybeAsASCII() + "'");
  http_response->set_content(file_contents);
  return std::move(http_response);
}
