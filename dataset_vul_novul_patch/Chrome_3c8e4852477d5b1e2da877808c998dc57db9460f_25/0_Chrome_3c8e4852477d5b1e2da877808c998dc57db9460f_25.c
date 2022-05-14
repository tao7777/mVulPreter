Response ServiceWorkerHandler::DeliverPushMessage(
    const std::string& origin,
    const std::string& registration_id,
     const std::string& data) {
   if (!enabled_)
     return CreateDomainNotEnabledErrorResponse();
  if (!browser_context_)
     return CreateContextErrorResponse();
   int64_t id = 0;
   if (!base::StringToInt64(registration_id, &id))
     return CreateInvalidVersionIdErrorResponse();
   PushEventPayload payload;
   if (data.size() > 0)
     payload.setData(data);
  BrowserContext::DeliverPushMessage(
      browser_context_, GURL(origin), id, payload,
      base::BindRepeating([](mojom::PushDeliveryStatus status) {}));

   return Response::OK();
 }
