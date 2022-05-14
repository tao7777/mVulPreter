receive_carbon(void **state)
{
    prof_input("/carbons on");

    prof_connect();
    assert_true(stbbr_received(
        "<iq id='*' type='set'><enable xmlns='urn:xmpp:carbons:2'/></iq>"
    ));

    stbbr_send(
        "<presence to='stabber@localhost' from='buddy1@localhost/mobile'>"
            "<priority>10</priority>"
            "<status>On my mobile</status>"
        "</presence>"
    );
    assert_true(prof_output_exact("Buddy1 (mobile) is online, \"On my mobile\""));
    prof_input("/msg Buddy1");
     assert_true(prof_output_exact("unencrypted"));
 
     stbbr_send(
        "<message type='chat' to='stabber@localhost/profanity' from='buddy1@localhost'>"
             "<received xmlns='urn:xmpp:carbons:2'>"
                 "<forwarded xmlns='urn:xmpp:forward:0'>"
                     "<message id='prof_msg_7' xmlns='jabber:client' type='chat' lang='en' to='stabber@localhost/profanity' from='buddy1@localhost/mobile'>"
                        "<body>test carbon from recipient</body>"
                    "</message>"
                "</forwarded>"
            "</received>"
        "</message>"
    );

    assert_true(prof_output_regex("Buddy1/mobile: .+test carbon from recipient"));
}
