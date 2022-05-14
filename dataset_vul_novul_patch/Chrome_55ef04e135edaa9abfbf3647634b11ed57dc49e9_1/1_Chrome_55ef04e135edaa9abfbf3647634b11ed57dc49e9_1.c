void Resource::LastPluginRefWasDeleted(bool instance_destroyed) {
   DCHECK(resource_id_ != 0);
   instance()->module()->GetCallbackTracker()->PostAbortForResource(
       resource_id_);
   resource_id_ = 0;
  if (instance_destroyed)
    instance_ = NULL;
 }
