#ifndef _PLUG_IN_
#define _PLUG_IN_

#define SEND_MSG(name) void name(char* message)
typedef SEND_MSG(send_msg);

#define GOTOXY(name) void name(int x, int y)
typedef GOTOXY(goto_xy);



struct irc_commands
{
	char* ownerName;
	send_msg* SendMessage;
	goto_xy* GotoXY;

};

#define ON_KICK(name) void name(char* channel, char* user, char* comment, struct irc_commands* commands)
typedef ON_KICK(on_kick);

#define ON_NICK(name) void name(char* oldNick, char* newNick, struct irc_commands* commands)
typedef ON_NICK(on_nick);

#define ON_QUIT(name) void name(char* user, char* msg, struct irc_commands* commands)
typedef ON_QUIT(on_quit);

#define ON_NOTICE(name) void name(char* from, char* msg, char* addr, struct irc_commands* commands)
typedef ON_NOTICE(on_notice);

#define ON_PRIVMSG(name) void name(char* from, char* user, char* msg, struct irc_commands* commands)
typedef ON_PRIVMSG(on_privmsg);

#define ON_JOIN(name) void name(char* roomname, char* user,struct irc_commands* commands )
typedef ON_JOIN(on_join);

#define ON_MODE(name) void name(char* from, char* to, char* modes,struct irc_commands* commands )
typedef ON_MODE(on_mode);

#define ON_TOPIC(name) void name(char* from,char* oldTopic, char* newTopic, struct irc_commands* commands) 
typedef ON_TOPIC(on_topic);

#define ON_INVITE(name) void name(char* from,char* whom, char* room, struct irc_commands* commands)
typedef ON_INVITE(on_invite);

#define ON_KEYPRESS(name) void name(char key, struct irc_commands* commands)
typedef ON_KEYPRESS(on_keypress);


ON_KEYPRESS(OnKeyPressStub)
{
}

ON_PRIVMSG(OnPrivMsgStub)
{
}

ON_JOIN(OnJoinStub)
{
}

ON_NOTICE(OnNoticeStub)
{
}

ON_QUIT(OnQuitStub)
{
}

ON_NICK(OnNickStub)
{
}

ON_KICK(OnKickStub)
{
}

ON_MODE(OnModeStub)
{
}

ON_TOPIC(OnTopicStub)
{
}

ON_INVITE(OnInviteStub)
{
}

#endif
