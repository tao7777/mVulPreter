void WebLocalFrameImpl::SetCommittedFirstRealLoad() {
   DCHECK(GetFrame());
   GetFrame()->Loader().StateMachine()->AdvanceTo(
       FrameLoaderStateMachine::kCommittedMultipleRealLoads);
  GetFrame()->SetShouldSendResourceTimingInfoToParent(false);
 }
