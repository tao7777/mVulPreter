void ExpectCanDiscardFalseTrivial(const LifecycleUnit* lifecycle_unit,
                                  DiscardReason discard_reason) {
   DecisionDetails decision_details;
   EXPECT_FALSE(lifecycle_unit->CanDiscard(discard_reason, &decision_details));
   EXPECT_FALSE(decision_details.IsPositive());
  EXPECT_TRUE(decision_details.reasons().empty());
 }
