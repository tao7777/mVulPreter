 void FileReaderLoader::start(ScriptExecutionContext* scriptExecutionContext, Blob* blob)
{
    m_urlForReading = BlobURL::createPublicURL(scriptExecutionContext->securityOrigin());
    if (m_urlForReading.isEmpty()) {
         failed(FileError::SECURITY_ERR);
         return;
     }
    ThreadableBlobRegistry::registerBlobURL(scriptExecutionContext->securityOrigin(), m_urlForReading, blob->url());
 
     ResourceRequest request(m_urlForReading);
    request.setHTTPMethod("GET");
    if (m_hasRange)
        request.setHTTPHeaderField("Range", String::format("bytes=%d-%d", m_rangeStart, m_rangeEnd));

    ThreadableLoaderOptions options;
    options.sendLoadCallbacks = SendCallbacks;
    options.sniffContent = DoNotSniffContent;
    options.preflightPolicy = ConsiderPreflight;
    options.allowCredentials = AllowStoredCredentials;
    options.crossOriginRequestPolicy = DenyCrossOriginRequests;
    options.contentSecurityPolicyEnforcement = DoNotEnforceContentSecurityPolicy;

    if (m_client)
        m_loader = ThreadableLoader::create(scriptExecutionContext, this, request, options);
    else
        ThreadableLoader::loadResourceSynchronously(scriptExecutionContext, request, *this, options);
}
