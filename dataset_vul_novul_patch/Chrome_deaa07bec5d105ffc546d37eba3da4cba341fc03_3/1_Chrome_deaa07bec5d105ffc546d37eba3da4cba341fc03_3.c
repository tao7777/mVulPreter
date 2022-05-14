  explicit LogoDelegateImpl(
      std::unique_ptr<image_fetcher::ImageDecoder> image_decoder)
      : image_decoder_(std::move(image_decoder)) {}
