bool ChromeOSSetImeConfig(InputMethodStatusConnection* connection,
                          const char* section,
                          const char* config_name,
                          const ImeConfigValue& value) {
  DCHECK(section);
  DCHECK(config_name);
  g_return_val_if_fail(connection, FALSE);
  return connection->SetImeConfig(section, config_name, value);
}
