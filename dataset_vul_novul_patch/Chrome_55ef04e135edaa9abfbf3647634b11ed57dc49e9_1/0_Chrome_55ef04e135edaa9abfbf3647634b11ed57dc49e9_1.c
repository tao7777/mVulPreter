void Resource::LastPluginRefWasDeleted(bool instance_destroyed) {
void Resource::LastPluginRefWasDeleted() {
   DCHECK(resource_id_ != 0);
   instance()->module()->GetCallbackTracker()->PostAbortForResource(
       resource_id_);
   resource_id_ = 0;
 }
