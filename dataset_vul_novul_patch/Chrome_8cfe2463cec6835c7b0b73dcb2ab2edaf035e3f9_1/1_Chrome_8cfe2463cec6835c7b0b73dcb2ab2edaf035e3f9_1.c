mojom::AppPtr AppControllerImpl::CreateAppPtr(const apps::AppUpdate& update) {
   auto app = chromeos::kiosk_next_home::mojom::App::New();
   app->app_id = update.AppId();
   app->type = update.AppType();
  app->display_name = update.Name();
  app->readiness = update.Readiness();

  if (app->type == apps::mojom::AppType::kArc) {
    app->android_package_name = MaybeGetAndroidPackageName(app->app_id);
  }
   return app;
 }
