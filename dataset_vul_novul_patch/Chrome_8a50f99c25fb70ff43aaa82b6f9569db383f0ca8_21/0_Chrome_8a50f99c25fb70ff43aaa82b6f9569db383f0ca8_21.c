   void RunBackoffTest(double jitter) {
     fake_registration_manager_.SetJitter(jitter);
     ObjectIdSet ids = GetSequenceOfIds(kObjectIdsCount);
    fake_registration_manager_.UpdateRegisteredIds(ids);
 
     ObjectIdSet lost_ids = GetSequenceOfIds(2);
    LoseRegistrations(lost_ids);
    ExpectPendingRegistrations(
        lost_ids, 0.0,
        fake_registration_manager_.GetPendingRegistrationsForTest());

    fake_registration_manager_.FirePendingRegistrationsForTest();
    LoseRegistrations(lost_ids);

    double scaled_jitter =
        jitter * RegistrationManager::kRegistrationDelayMaxJitter;

    double expected_delay =
        RegistrationManager::kInitialRegistrationDelaySeconds *
        (1.0 + scaled_jitter);
    expected_delay = std::floor(expected_delay);
    ExpectPendingRegistrations(
        lost_ids, expected_delay,
        fake_registration_manager_.GetPendingRegistrationsForTest());

    fake_registration_manager_.FirePendingRegistrationsForTest();
    LoseRegistrations(lost_ids);
    expected_delay *=
        RegistrationManager::kRegistrationDelayExponent + scaled_jitter;
    expected_delay = std::floor(expected_delay);
    ExpectPendingRegistrations(
        lost_ids, expected_delay,
        fake_registration_manager_.GetPendingRegistrationsForTest());

    while (expected_delay < RegistrationManager::kMaxRegistrationDelaySeconds) {
      fake_registration_manager_.FirePendingRegistrationsForTest();
      LoseRegistrations(lost_ids);
      expected_delay *=
          RegistrationManager::kRegistrationDelayExponent + scaled_jitter;
      expected_delay = std::floor(expected_delay);
    }
    ExpectPendingRegistrations(
        lost_ids,
        RegistrationManager::kMaxRegistrationDelaySeconds,
        fake_registration_manager_.GetPendingRegistrationsForTest());
  }
