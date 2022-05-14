void PPB_URLLoader_Impl::LastPluginRefWasDeleted(bool instance_destroyed) {
void PPB_URLLoader_Impl::ClearInstance() {
  Resource::ClearInstance();
  loader_.reset();
 }
