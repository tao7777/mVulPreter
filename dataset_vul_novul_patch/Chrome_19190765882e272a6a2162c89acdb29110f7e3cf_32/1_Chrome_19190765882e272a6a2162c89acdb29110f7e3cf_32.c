void MigrationTest::SetUpVersion69Database() {
  sql::Connection connection;
  ASSERT_TRUE(connection.Open(GetDatabasePath()));
  ASSERT_TRUE(connection.BeginTransaction());
  ASSERT_TRUE(connection.Execute(
      "CREATE TABLE extended_attributes(metahandle bigint, key varchar(127), "
          "value blob, PRIMARY KEY(metahandle, key) ON CONFLICT REPLACE);"
      "CREATE TABLE metas (metahandle bigint primary key ON CONFLICT FAIL,"
          "base_version bigint default -1,server_version bigint default 0,"
          "mtime bigint default 0,server_mtime bigint default 0,"
          "ctime bigint default 0,server_ctime bigint default 0,"
          "server_position_in_parent bigint default 0,"
          "local_external_id bigint default 0,id varchar(255) default 'r',"
          "parent_id varchar(255) default 'r',"
          "server_parent_id varchar(255) default 'r',"
          "prev_id varchar(255) default 'r',next_id varchar(255) default 'r',"
          "is_unsynced bit default 0,is_unapplied_update bit default 0,"
          "is_del bit default 0,is_dir bit default 0,"
          "is_bookmark_object bit default 0,server_is_dir bit default 0,"
          "server_is_del bit default 0,"
          "server_is_bookmark_object bit default 0,"
          "non_unique_name varchar,server_non_unique_name varchar(255),"
          "bookmark_url varchar,server_bookmark_url varchar,"
           "singleton_tag varchar,bookmark_favicon blob,"
           "server_bookmark_favicon blob, specifics blob, "
           "server_specifics blob);"
      "INSERT INTO metas VALUES(1,-1,0," LEGACY_PROTO_TIME_VALS(1)
          ",0,0,'r','r','r','r','r',0,0,0,1,0,0,0,0,NULL,NULL,NULL,NULL,NULL,"
           "NULL,NULL,X'',X'');"
      "INSERT INTO metas VALUES(2,669,669," LEGACY_PROTO_TIME_VALS(2)
          ",-2097152,"
           "4,'s_ID_2','s_ID_9','s_ID_9','s_ID_2','s_ID_2',0,0,1,0,1,0,1,1,"
           "'Deleted Item','Deleted Item','http://www.google.com/',"
           "'http://www.google.com/2',NULL,'AASGASGA','ASADGADGADG',"
           "X'C28810220A16687474703A2F2F7777772E676F6F676C652E636F6D2F120841415"
           "34741534741',X'C28810260A17687474703A2F2F7777772E676F6F676C652E636F"
           "6D2F32120B4153414447414447414447');"
      "INSERT INTO metas VALUES(4,681,681," LEGACY_PROTO_TIME_VALS(4)
          ",-3145728,"
           "3,'s_ID_4','s_ID_9','s_ID_9','s_ID_4','s_ID_4',0,0,1,0,1,0,1,1,"
           "'Welcome to Chromium','Welcome to Chromium',"
           "'http://www.google.com/chrome/intl/en/welcome.html',"
          "'http://www.google.com/chrome/intl/en/welcome.html',NULL,NULL,NULL,"
          "X'C28810350A31687474703A2F2F7777772E676F6F676C652E636F6D2F6368726F6"
           "D652F696E746C2F656E2F77656C636F6D652E68746D6C1200',X'C28810350A3168"
           "7474703A2F2F7777772E676F6F676C652E636F6D2F6368726F6D652F696E746C2F6"
           "56E2F77656C636F6D652E68746D6C1200');"
      "INSERT INTO metas VALUES(5,677,677," LEGACY_PROTO_TIME_VALS(5)
          ",1048576,7,"
           "'s_ID_5','s_ID_9','s_ID_9','s_ID_5','s_ID_5',0,0,1,0,1,0,1,1,"
           "'Google','Google','http://www.google.com/',"
           "'http://www.google.com/',NULL,'AGASGASG','AGFDGASG',X'C28810220A166"
           "87474703A2F2F7777772E676F6F676C652E636F6D2F12084147415347415347',X'"
           "C28810220A16687474703A2F2F7777772E676F6F676C652E636F6D2F12084147464"
           "447415347');"
      "INSERT INTO metas VALUES(6,694,694," LEGACY_PROTO_TIME_VALS(6)
          ",-4194304,6"
           ",'s_ID_6','s_ID_9','s_ID_9','r','r',0,0,0,1,1,1,0,1,'The Internet',"
           "'The Internet',NULL,NULL,NULL,NULL,NULL,X'C2881000',X'C2881000');"
      "INSERT INTO metas VALUES(7,663,663," LEGACY_PROTO_TIME_VALS(7)
          ",1048576,0,"
           "'s_ID_7','r','r','r','r',0,0,0,1,1,1,0,1,'Google Chrome',"
           "'Google Chrome',NULL,NULL,'google_chrome',NULL,NULL,NULL,NULL);"
      "INSERT INTO metas VALUES(8,664,664," LEGACY_PROTO_TIME_VALS(8)
          ",1048576,0,"
           "'s_ID_8','s_ID_7','s_ID_7','r','r',0,0,0,1,1,1,0,1,'Bookmarks',"
           "'Bookmarks',NULL,NULL,'google_chrome_bookmarks',NULL,NULL,"
           "X'C2881000',X'C2881000');"
      "INSERT INTO metas VALUES(9,665,665," LEGACY_PROTO_TIME_VALS(9)
          ",1048576,1,"
           "'s_ID_9','s_ID_8','s_ID_8','r','s_ID_10',0,0,0,1,1,1,0,1,"
           "'Bookmark Bar','Bookmark Bar',NULL,NULL,'bookmark_bar',NULL,NULL,"
           "X'C2881000',X'C2881000');"
      "INSERT INTO metas VALUES(10,666,666," LEGACY_PROTO_TIME_VALS(10)
          ",2097152,2,"
           "'s_ID_10','s_ID_8','s_ID_8','s_ID_9','r',0,0,0,1,1,1,0,1,"
           "'Other Bookmarks','Other Bookmarks',NULL,NULL,'other_bookmarks',"
           "NULL,NULL,X'C2881000',X'C2881000');"
      "INSERT INTO metas VALUES(11,683,683," LEGACY_PROTO_TIME_VALS(11)
          ",-1048576,"
           "8,'s_ID_11','s_ID_6','s_ID_6','r','s_ID_13',0,0,0,0,1,0,0,1,"
           "'Home (The Chromium Projects)','Home (The Chromium Projects)',"
           "'http://dev.chromium.org/','http://dev.chromium.org/other',NULL,"
           "'AGATWA','AFAGVASF',X'C28810220A18687474703A2F2F6465762E6368726F6D6"
           "9756D2E6F72672F1206414741545741',X'C28810290A1D687474703A2F2F646576"
           "2E6368726F6D69756D2E6F72672F6F7468657212084146414756415346');"
      "INSERT INTO metas VALUES(12,685,685," LEGACY_PROTO_TIME_VALS(12)
          ",0,9,"
           "'s_ID_12','s_ID_6','s_ID_6','s_ID_13','s_ID_14',0,0,0,1,1,1,0,1,"
           "'Extra Bookmarks','Extra Bookmarks',NULL,NULL,NULL,NULL,NULL,"
           "X'C2881000',X'C2881000');"
      "INSERT INTO metas VALUES(13,687,687," LEGACY_PROTO_TIME_VALS(13)
          ",-917504,"
           "10,'s_ID_13','s_ID_6','s_ID_6','s_ID_11','s_ID_12',0,0,0,0,1,0,0,"
           "1,'ICANN | Internet Corporation for Assigned Names and Numbers',"
           "'ICANN | Internet Corporation for Assigned Names and Numbers',"
           "'http://www.icann.com/','http://www.icann.com/',NULL,'PNGAXF0AAFF',"
           "'DAAFASF',X'C28810240A15687474703A2F2F7777772E6963616E6E2E636F6D2F1"
           "20B504E474158463041414646',X'C28810200A15687474703A2F2F7777772E6963"
           "616E6E2E636F6D2F120744414146415346');"
      "INSERT INTO metas VALUES(14,692,692," LEGACY_PROTO_TIME_VALS(14)
          ",1048576,11,"
           "'s_ID_14','s_ID_6','s_ID_6','s_ID_12','r',0,0,0,0,1,0,0,1,"
           "'The WebKit Open Source Project','The WebKit Open Source Project',"
           "'http://webkit.org/','http://webkit.org/x',NULL,'PNGX','PNG2Y',"
          "X'C288101A0A12687474703A2F2F7765626B69742E6F72672F1204504E4758',X'C2"
          "88101C0A13687474703A2F2F7765626B69742E6F72672F781205504E473259');"
      "CREATE TABLE share_info (id VARCHAR(128) primary key, "
          "last_sync_timestamp INT, name VARCHAR(128), "
          "initial_sync_ended BIT default 0, store_birthday VARCHAR(256), "
          "db_create_version VARCHAR(128), db_create_time int, "
          "next_id bigint default -2, cache_guid VARCHAR(32));"
      "INSERT INTO share_info VALUES('nick@chromium.org',694,"
          "'nick@chromium.org',1,'c27e9f59-08ca-46f8-b0cc-f16a2ed778bb',"
          "'Unknown',1263522064,-65542,"
          "'9010788312004066376x-6609234393368420856x');"
      "CREATE TABLE share_version (id VARCHAR(128) primary key, data INT);"
      "INSERT INTO share_version VALUES('nick@chromium.org',69);"
  ));
  ASSERT_TRUE(connection.CommitTransaction());
}
