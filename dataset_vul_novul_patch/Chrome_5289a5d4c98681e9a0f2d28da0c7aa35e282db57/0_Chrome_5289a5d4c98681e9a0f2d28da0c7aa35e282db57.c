void ServiceWorkerContainer::registerServiceWorkerImpl(ExecutionContext* executionContext, const KURL& rawScriptURL, const KURL& scope, PassOwnPtr<RegistrationCallbacks> callbacks)
{
    if (!m_provider) {
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeState, "Failed to register a ServiceWorker: The document is in an invalid state."));
        return;
    }

    RefPtr<SecurityOrigin> documentOrigin = executionContext->getSecurityOrigin();
    String errorMessage;
    if (!executionContext->isSecureContext(errorMessage)) {
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, errorMessage));
        return;
    }

    KURL pageURL = KURL(KURL(), documentOrigin->toString());
    if (!SchemeRegistry::shouldTreatURLSchemeAsAllowingServiceWorkers(pageURL.protocol())) {
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, String("Failed to register a ServiceWorker: The URL protocol of the current origin ('" + documentOrigin->toString() + "') is not supported.")));
        return;
    }

    KURL scriptURL = rawScriptURL;
    scriptURL.removeFragmentIdentifier();
    if (!documentOrigin->canRequest(scriptURL)) {
        RefPtr<SecurityOrigin> scriptOrigin = SecurityOrigin::create(scriptURL);
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, String("Failed to register a ServiceWorker: The origin of the provided scriptURL ('" + scriptOrigin->toString() + "') does not match the current origin ('" + documentOrigin->toString() + "').")));
        return;
    }
    if (!SchemeRegistry::shouldTreatURLSchemeAsAllowingServiceWorkers(scriptURL.protocol())) {
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, String("Failed to register a ServiceWorker: The URL protocol of the script ('" + scriptURL.getString() + "') is not supported.")));
        return;
    }

    KURL patternURL = scope;
    patternURL.removeFragmentIdentifier();

    if (!documentOrigin->canRequest(patternURL)) {
        RefPtr<SecurityOrigin> patternOrigin = SecurityOrigin::create(patternURL);
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, String("Failed to register a ServiceWorker: The origin of the provided scope ('" + patternOrigin->toString() + "') does not match the current origin ('" + documentOrigin->toString() + "').")));
        return;
    }
    if (!SchemeRegistry::shouldTreatURLSchemeAsAllowingServiceWorkers(patternURL.protocol())) {
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, String("Failed to register a ServiceWorker: The URL protocol of the scope ('" + patternURL.getString() + "') is not supported.")));
        return;
    }

    WebString webErrorMessage;
    if (!m_provider->validateScopeAndScriptURL(patternURL, scriptURL, &webErrorMessage)) {
        callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeType, WebString::fromUTF8("Failed to register a ServiceWorker: " + webErrorMessage.utf8())));
         return;
     }
 
    ContentSecurityPolicy* csp = executionContext->contentSecurityPolicy();
    if (csp) {
        if (!csp->allowWorkerContextFromSource(scriptURL, ContentSecurityPolicy::DidNotRedirect, ContentSecurityPolicy::SendReport)) {
            callbacks->onError(WebServiceWorkerError(WebServiceWorkerError::ErrorTypeSecurity, String("Failed to register a ServiceWorker: The provided scriptURL ('" + scriptURL.getString() + "') violates the Content Security Policy.")));
            return;
        }
    }

     m_provider->registerServiceWorker(patternURL, scriptURL, callbacks.leakPtr());
 }
