std::unique_ptr<service_manager::Service> CreateMediaService() {
   return std::unique_ptr<service_manager::Service>(
       new ::media::MediaService(base::MakeUnique<CdmMojoMediaClient>()));
 }
