		void CWebServer::GetFloorplanImage(WebEmSession & session, const request& req, reply & rep)
		{
			std::string idx = request::findValue(&req, "idx");
			if (idx == "") {
 				return;
 			}
 			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_queryBlob("SELECT Image FROM Floorplans WHERE ID=%s", idx.c_str());
 			if (result.empty())
 				return;
 			reply::set_content(&rep, result[0][0].begin(), result[0][0].end());
			std::string oname = "floorplan";
			if (result[0][0].size() > 10)
			{
				if (result[0][0][0] == 'P')
					oname += ".png";
				else if (result[0][0][0] == -1)
					oname += ".jpg";
				else if (result[0][0][0] == 'B')
					oname += ".bmp";
				else if (result[0][0][0] == 'G')
					oname += ".gif";
			}
			reply::add_header_attachment(&rep, oname);
		}
