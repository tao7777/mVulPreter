LogoService::LogoService(
    const base::FilePath& cache_directory,
    TemplateURLService* template_url_service,
    std::unique_ptr<image_fetcher::ImageDecoder> image_decoder,
    scoped_refptr<net::URLRequestContextGetter> request_context_getter,
    bool use_gray_background)
    : cache_directory_(cache_directory),
      template_url_service_(template_url_service),
      request_context_getter_(request_context_getter),
      use_gray_background_(use_gray_background),
      image_decoder_(std::move(image_decoder)) {}
