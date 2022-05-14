bool TabLifecycleUnitSource::TabLifecycleUnit::CanFreeze(
    DecisionDetails* decision_details) const {
  DCHECK(decision_details->reasons().empty());


  if (!IsValidStateChange(GetState(), LifecycleUnitState::PENDING_FREEZE,
                          StateChangeReason::BROWSER_INITIATED)) {
    return false;
  }

  if (TabLoadTracker::Get()->GetLoadingState(GetWebContents()) !=
      TabLoadTracker::LoadingState::LOADED) {
     return false;
   }
 

  if (GetWebContents()->GetVisibility() == content::Visibility::VISIBLE)
    decision_details->AddReason(DecisionFailureReason::LIVE_STATE_VISIBLE);

   IsMediaTabImpl(decision_details);
 
  // Consult the local database to see if this tab could try to communicate with
  // the user while in background (don't check for the visibility here as
  // there's already a check for that above).
  CheckIfTabCanCommunicateWithUserWhileInBackground(GetWebContents(),
                                                    decision_details);

   if (decision_details->reasons().empty()) {
     decision_details->AddReason(
         DecisionSuccessReason::HEURISTIC_OBSERVED_TO_BE_SAFE);
   }

   return decision_details->IsPositive();
 }
