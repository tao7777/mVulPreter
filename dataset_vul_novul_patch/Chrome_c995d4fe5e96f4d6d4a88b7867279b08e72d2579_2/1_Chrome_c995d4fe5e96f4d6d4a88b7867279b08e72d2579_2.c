 DataReductionProxySettings::DataReductionProxySettings()
     : unreachable_(false),
       deferred_initialization_(false),
       prefs_(nullptr),
       config_(nullptr),
       clock_(base::DefaultClock::GetInstance()) {}
