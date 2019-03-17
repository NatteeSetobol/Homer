#ifndef token
#define token

enum 
{
	RPL_MOTDSTART=375,
	RPL_MOTD = 372,
	RPL_ENDOFMOTD=376
};

enum Token_Type
{
	Unknown,
	End_of_line, 
	Null_Space,
	Space,
	Colon,
	Data,
	New_Line
};

struct Token
{
	size_t length;
	enum Token_Type type;
	char* text;
	char tok;
};

struct Tokenizer
{
	char* At;
};


struct Token GetToken(struct Tokenizer* tokenizer)
{
	struct Token tokens = {};
	char c=0;
	ui8* tokAddr=NULL;

	tokens.length = 1;
	tokens.text = tokenizer->At;
	c=tokenizer->At[0];
	tokens.tok = c;
	++tokenizer->At;

	switch(c)
	{
		case '\0' : { tokens.type = Null_Space; } break;
		case ':': { tokens.type = Colon; } break;
		case 0x20: { tokens.type = Space; } break;
		case '\r': { tokens.type = End_of_line; } break;
		case '\n': { tokens.type = New_Line; } break;
		default: { tokens.type = Data; } break;
	}

	return tokens;
}
#endif
