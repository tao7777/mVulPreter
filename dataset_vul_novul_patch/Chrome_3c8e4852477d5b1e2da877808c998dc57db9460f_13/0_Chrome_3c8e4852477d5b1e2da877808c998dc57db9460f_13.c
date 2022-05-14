void NetworkHandler::ContinueInterceptedRequest(
    const std::string& interception_id,
    Maybe<std::string> error_reason,
    Maybe<std::string> base64_raw_response,
    Maybe<std::string> url,
    Maybe<std::string> method,
    Maybe<std::string> post_data,
    Maybe<protocol::Network::Headers> headers,
     Maybe<protocol::Network::AuthChallengeResponse> auth_challenge_response,
     std::unique_ptr<ContinueInterceptedRequestCallback> callback) {
   DevToolsInterceptorController* interceptor =
      DevToolsInterceptorController::FromBrowserContext(browser_context_);
   if (!interceptor) {
     callback->sendFailure(Response::InternalError());
     return;
  }

  base::Optional<std::string> raw_response;
  if (base64_raw_response.isJust()) {
    std::string decoded;
    if (!base::Base64Decode(base64_raw_response.fromJust(), &decoded)) {
      callback->sendFailure(Response::InvalidParams("Invalid rawResponse."));
      return;
    }
    raw_response = decoded;
  }

  base::Optional<net::Error> error;
  bool mark_as_canceled = false;
  if (error_reason.isJust()) {
    bool ok;
    error = NetErrorFromString(error_reason.fromJust(), &ok);
    if (!ok) {
      callback->sendFailure(Response::InvalidParams("Invalid errorReason."));
      return;
    }

    mark_as_canceled = true;
  }

  interceptor->ContinueInterceptedRequest(
      interception_id,
      std::make_unique<DevToolsURLRequestInterceptor::Modifications>(
          std::move(error), std::move(raw_response), std::move(url),
          std::move(method), std::move(post_data), std::move(headers),
          std::move(auth_challenge_response), mark_as_canceled),
      std::move(callback));
}
