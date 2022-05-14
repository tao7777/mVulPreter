bool TabLifecycleUnitSource::TabLifecycleUnit::CanDiscard(
    DiscardReason reason,
    DecisionDetails* decision_details) const {
  DCHECK(decision_details->reasons().empty());


  if (!tab_strip_model_)
    return false;

  const LifecycleUnitState target_state =
      reason == DiscardReason::kProactive &&
              GetState() != LifecycleUnitState::FROZEN
          ? LifecycleUnitState::PENDING_DISCARD
          : LifecycleUnitState::DISCARDED;
  if (!IsValidStateChange(GetState(), target_state,
                          DiscardReasonToStateChangeReason(reason))) {
    return false;
  }

  if (GetWebContents()->IsCrashed())
    return false;

  if (!GetWebContents()->GetLastCommittedURL().is_valid() ||
      GetWebContents()->GetLastCommittedURL().is_empty()) {
    return false;
  }

  if (discard_count_ > 0) {
#if defined(OS_CHROMEOS)
    if (reason != DiscardReason::kUrgent)
      return false;
#else
    return false;
 #endif  // defined(OS_CHROMEOS)
   }
 

#if defined(OS_CHROMEOS)
  if (GetWebContents()->GetVisibility() == content::Visibility::VISIBLE)
    decision_details->AddReason(DecisionFailureReason::LIVE_STATE_VISIBLE);
#else
  if (tab_strip_model_->GetActiveWebContents() == GetWebContents())
    decision_details->AddReason(DecisionFailureReason::LIVE_STATE_VISIBLE);
#endif  // defined(OS_CHROMEOS)

  if (GetWebContents()->GetPageImportanceSignals().had_form_interaction)
    decision_details->AddReason(DecisionFailureReason::LIVE_STATE_FORM_ENTRY);

  IsMediaTabImpl(decision_details);

  if (GetWebContents()->GetContentsMimeType() == "application/pdf")
    decision_details->AddReason(DecisionFailureReason::LIVE_STATE_IS_PDF);

  if (!IsAutoDiscardable()) {
    decision_details->AddReason(
         DecisionFailureReason::LIVE_STATE_EXTENSION_DISALLOWED);
   }
 
   if (decision_details->reasons().empty()) {
     decision_details->AddReason(
         DecisionSuccessReason::HEURISTIC_OBSERVED_TO_BE_SAFE);
    DCHECK(decision_details->IsPositive());
  }
  return decision_details->IsPositive();
}
