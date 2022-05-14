 bool SessionRestore::IsRestoring(const Profile* profile) {
  return (profiles_getting_restored &&
          profiles_getting_restored->find(profile) !=
          profiles_getting_restored->end());
 }
