ServiceWorkerContainer* NavigatorServiceWorker::serviceWorker(Navigator& navigator, ExceptionState& exceptionState)
 {
     return NavigatorServiceWorker::from(navigator).serviceWorker(exceptionState);
 }
