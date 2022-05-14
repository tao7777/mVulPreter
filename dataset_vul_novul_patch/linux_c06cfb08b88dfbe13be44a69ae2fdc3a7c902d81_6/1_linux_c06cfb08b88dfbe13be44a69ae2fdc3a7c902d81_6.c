int user_match(const struct key *key, const struct key_match_data *match_data)
{
	return strcmp(key->description, match_data->raw_data) == 0;
}
