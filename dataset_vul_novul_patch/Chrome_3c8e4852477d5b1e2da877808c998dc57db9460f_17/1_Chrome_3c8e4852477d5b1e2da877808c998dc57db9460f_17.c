void NetworkHandler::GetResponseBodyForInterception(
     const String& interception_id,
     std::unique_ptr<GetResponseBodyForInterceptionCallback> callback) {
   DevToolsInterceptorController* interceptor =
      DevToolsInterceptorController::FromBrowserContext(
          process_->GetBrowserContext());
   if (!interceptor) {
     callback->sendFailure(Response::InternalError());
     return;
  }
  interceptor->GetResponseBody(interception_id, std::move(callback));
}
