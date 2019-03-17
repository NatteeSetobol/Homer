#ifndef _PLUG_IN_
#define _PLUG_IN_

#define SEND_MSG(name) void name(char* message)
typedef SEND_MSG(send_msg);

struct irc_commands
{
	send_msg* SendMessage;
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

#define ON_MODE(name) void name(char* from, char* user, char* mode);
typedef ON_MODE(on_mode);

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

#endif
