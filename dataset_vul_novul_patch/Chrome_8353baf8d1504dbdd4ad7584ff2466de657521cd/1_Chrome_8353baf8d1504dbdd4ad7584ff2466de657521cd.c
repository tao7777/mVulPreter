ServiceWorkerNetworkProvider::CreateForNavigation(
    int route_id,
    const RequestNavigationParams& request_params,
    blink::WebLocalFrame* frame,
    bool content_initiated) {
  bool browser_side_navigation = IsBrowserSideNavigationEnabled();
  bool should_create_provider_for_window = false;
  int service_worker_provider_id = kInvalidServiceWorkerProviderId;
  std::unique_ptr<ServiceWorkerNetworkProvider> network_provider;

  if (browser_side_navigation && !content_initiated) {
    should_create_provider_for_window =
        request_params.should_create_service_worker;
    service_worker_provider_id = request_params.service_worker_provider_id;
    DCHECK(ServiceWorkerUtils::IsBrowserAssignedProviderId(
               service_worker_provider_id) ||
           service_worker_provider_id == kInvalidServiceWorkerProviderId);
  } else {
    should_create_provider_for_window =
        ((frame->effectiveSandboxFlags() & blink::WebSandboxFlags::Origin) !=
         blink::WebSandboxFlags::Origin);
  }

  if (should_create_provider_for_window) {
    bool is_parent_frame_secure =
        !frame->parent() || frame->parent()->canHaveSecureChild();
 
     if (service_worker_provider_id == kInvalidServiceWorkerProviderId) {
       network_provider = std::unique_ptr<ServiceWorkerNetworkProvider>(
          new ServiceWorkerNetworkProvider(route_id,
                                           SERVICE_WORKER_PROVIDER_FOR_WINDOW,
                                           is_parent_frame_secure));
    } else {
      CHECK(browser_side_navigation);
      DCHECK(ServiceWorkerUtils::IsBrowserAssignedProviderId(
          service_worker_provider_id));
      network_provider = std::unique_ptr<ServiceWorkerNetworkProvider>(
          new ServiceWorkerNetworkProvider(
              route_id, SERVICE_WORKER_PROVIDER_FOR_WINDOW,
              service_worker_provider_id, is_parent_frame_secure));
    }
  } else {
    network_provider = std::unique_ptr<ServiceWorkerNetworkProvider>(
        new ServiceWorkerNetworkProvider());
  }
  return network_provider;
}
