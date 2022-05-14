void ConfigureQuicParams(base::StringPiece quic_trial_group,
                         const VariationParameters& quic_trial_params,
                         bool is_quic_force_disabled,
                         bool is_quic_force_enabled,
                         const std::string& quic_user_agent_id,
                         net::HttpNetworkSession::Params* params) {
  params->enable_quic =
      ShouldEnableQuic(quic_trial_group, quic_trial_params,
                       is_quic_force_disabled, is_quic_force_enabled);
  params->mark_quic_broken_when_network_blackholes =
      ShouldMarkQuicBrokenWhenNetworkBlackholes(quic_trial_params);

  params->enable_server_push_cancellation =
      ShouldEnableServerPushCancelation(quic_trial_params);

  params->retry_without_alt_svc_on_quic_errors =
      ShouldRetryWithoutAltSvcOnQuicErrors(quic_trial_params);

  params->support_ietf_format_quic_altsvc =
      ShouldSupportIetfFormatQuicAltSvc(quic_trial_params);

   if (params->enable_quic) {
     params->enable_quic_proxies_for_https_urls =
         ShouldEnableQuicProxiesForHttpsUrls(quic_trial_params);
     params->quic_connection_options =
         GetQuicConnectionOptions(quic_trial_params);
     params->quic_client_connection_options =
        GetQuicClientConnectionOptions(quic_trial_params);
    params->quic_close_sessions_on_ip_change =
        ShouldQuicCloseSessionsOnIpChange(quic_trial_params);
    params->quic_goaway_sessions_on_ip_change =
        ShouldQuicGoAwaySessionsOnIpChange(quic_trial_params);
    int idle_connection_timeout_seconds =
        GetQuicIdleConnectionTimeoutSeconds(quic_trial_params);
    if (idle_connection_timeout_seconds != 0) {
      params->quic_idle_connection_timeout_seconds =
          idle_connection_timeout_seconds;
    }
    int reduced_ping_timeout_seconds =
        GetQuicReducedPingTimeoutSeconds(quic_trial_params);
    if (reduced_ping_timeout_seconds > 0 &&
        reduced_ping_timeout_seconds < quic::kPingTimeoutSecs) {
      params->quic_reduced_ping_timeout_seconds = reduced_ping_timeout_seconds;
    }
    int max_time_before_crypto_handshake_seconds =
        GetQuicMaxTimeBeforeCryptoHandshakeSeconds(quic_trial_params);
    if (max_time_before_crypto_handshake_seconds > 0) {
      params->quic_max_time_before_crypto_handshake_seconds =
          max_time_before_crypto_handshake_seconds;
    }
    int max_idle_time_before_crypto_handshake_seconds =
        GetQuicMaxIdleTimeBeforeCryptoHandshakeSeconds(quic_trial_params);
    if (max_idle_time_before_crypto_handshake_seconds > 0) {
      params->quic_max_idle_time_before_crypto_handshake_seconds =
          max_idle_time_before_crypto_handshake_seconds;
    }
    params->quic_race_cert_verification =
        ShouldQuicRaceCertVerification(quic_trial_params);
    params->quic_estimate_initial_rtt =
        ShouldQuicEstimateInitialRtt(quic_trial_params);
    params->quic_headers_include_h2_stream_dependency =
        ShouldQuicHeadersIncludeH2StreamDependencies(quic_trial_params);
    params->quic_migrate_sessions_on_network_change_v2 =
        ShouldQuicMigrateSessionsOnNetworkChangeV2(quic_trial_params);
    params->quic_migrate_sessions_early_v2 =
        ShouldQuicMigrateSessionsEarlyV2(quic_trial_params);
    params->quic_retry_on_alternate_network_before_handshake =
        ShouldQuicRetryOnAlternateNetworkBeforeHandshake(quic_trial_params);
    params->quic_go_away_on_path_degrading =
        ShouldQuicGoawayOnPathDegrading(quic_trial_params);
    params->quic_race_stale_dns_on_connection =
        ShouldQuicRaceStaleDNSOnConnection(quic_trial_params);
    int max_time_on_non_default_network_seconds =
        GetQuicMaxTimeOnNonDefaultNetworkSeconds(quic_trial_params);
    if (max_time_on_non_default_network_seconds > 0) {
      params->quic_max_time_on_non_default_network =
          base::TimeDelta::FromSeconds(max_time_on_non_default_network_seconds);
    }
    int max_migrations_to_non_default_network_on_write_error =
        GetQuicMaxNumMigrationsToNonDefaultNetworkOnWriteError(
            quic_trial_params);
    if (max_migrations_to_non_default_network_on_write_error > 0) {
      params->quic_max_migrations_to_non_default_network_on_write_error =
          max_migrations_to_non_default_network_on_write_error;
    }
    int max_migrations_to_non_default_network_on_path_degrading =
        GetQuicMaxNumMigrationsToNonDefaultNetworkOnPathDegrading(
            quic_trial_params);
    if (max_migrations_to_non_default_network_on_path_degrading > 0) {
      params->quic_max_migrations_to_non_default_network_on_path_degrading =
          max_migrations_to_non_default_network_on_path_degrading;
    }
    params->quic_allow_server_migration =
        ShouldQuicAllowServerMigration(quic_trial_params);
    params->quic_host_whitelist = GetQuicHostWhitelist(quic_trial_params);
  }

  size_t max_packet_length = GetQuicMaxPacketLength(quic_trial_params);
  if (max_packet_length != 0) {
    params->quic_max_packet_length = max_packet_length;
  }

  params->quic_user_agent_id = quic_user_agent_id;

  quic::QuicTransportVersionVector supported_versions =
      GetQuicVersions(quic_trial_params);
  if (!supported_versions.empty())
    params->quic_supported_versions = supported_versions;
}
