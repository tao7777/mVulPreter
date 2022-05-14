ServiceWorkerContainer* NavigatorServiceWorker::serviceWorker(Navigator& navigator, ExceptionState& exceptionState)
ServiceWorkerContainer* NavigatorServiceWorker::serviceWorker(ExecutionContext* executionContext, Navigator& navigator, ExceptionState& exceptionState)
 {
    ASSERT(!navigator.frame() || executionContext->securityOrigin()->canAccessCheckSuborigins(navigator.frame()->securityContext()->securityOrigin()));
     return NavigatorServiceWorker::from(navigator).serviceWorker(exceptionState);
 }
