 InputImeEventRouter* InputImeEventRouterFactory::GetRouter(Profile* profile) {
   if (!profile)
     return nullptr;
   InputImeEventRouter* router = router_map_[profile];
   if (!router) {
    router = new InputImeEventRouter(profile);
     router_map_[profile] = router;
   }
   return router;
}
