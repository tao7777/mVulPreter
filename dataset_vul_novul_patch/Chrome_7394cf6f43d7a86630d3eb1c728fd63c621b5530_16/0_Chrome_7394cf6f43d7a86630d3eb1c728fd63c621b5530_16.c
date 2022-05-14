void ExpectCanDiscardFalseTrivial(const LifecycleUnit* lifecycle_unit,
                                  DiscardReason discard_reason) {
   DecisionDetails decision_details;
   EXPECT_FALSE(lifecycle_unit->CanDiscard(discard_reason, &decision_details));
   EXPECT_FALSE(decision_details.IsPositive());
  // |reasons()| will either contain the status for the 4 local site features
  // heuristics or be empty if the database doesn't track this lifecycle unit.
  EXPECT_TRUE(decision_details.reasons().empty() ||
              (decision_details.reasons().size() == 4));
 }
