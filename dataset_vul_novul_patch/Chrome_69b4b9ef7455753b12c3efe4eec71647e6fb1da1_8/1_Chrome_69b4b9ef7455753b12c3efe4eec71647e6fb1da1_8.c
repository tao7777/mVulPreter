 SecureProxyChecker::SecureProxyChecker(
     scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory)
    : url_loader_factory_(std::move(url_loader_factory)) {}
