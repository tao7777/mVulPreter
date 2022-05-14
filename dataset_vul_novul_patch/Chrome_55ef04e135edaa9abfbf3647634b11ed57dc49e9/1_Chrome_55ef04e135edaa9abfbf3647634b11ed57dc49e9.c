void PPB_URLLoader_Impl::LastPluginRefWasDeleted(bool instance_destroyed) {
  Resource::LastPluginRefWasDeleted(instance_destroyed);
  if (instance_destroyed) {
    loader_.reset();
  }
 }
