std::unique_ptr<service_manager::Service> CreateMediaService() {
std::unique_ptr<service_manager::Service> CreateCdmService() {
   return std::unique_ptr<service_manager::Service>(
       new ::media::MediaService(base::MakeUnique<CdmMojoMediaClient>()));
 }
