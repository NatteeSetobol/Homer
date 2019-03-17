
       ,-','       `-.___ 
      /-;'               `._ 
     /\/          ._   _,'o \ 
    ( /\       _,--'\,','"`. ) 
     |\      ,'o     \'    //\ 
     |      \        /   ,--'""`-. 
     :       \_    _/ ,-'         `-._ 
      \        `--'  /                ) 
       `.  \`._    ,'     ________,',' 
         .--`     ,'  ,--` __\___,;' 
          \`.,-- ,' ,`_)--'  /`.,' 
           \( ;  | | )      (`-/ 
             `--'| |)       |-/ 
               | | |        | | 
               | | |,.,-.   | |_ 
               | `./ /   )---`  ) 
              _|  /    ,',   ,-' 
     -hrr-   ,'|_(    /-<._,' |--, 
             |    `--'---.     \/ \ 
             |          / \    /\  \ 
           ,-^---._     |  \  /  \  \ 
        ,-'        \----'   \/    \--`. 
       /            \              \   \ 

Homer Bot -- Handmade IRC Bot with Hot Loading based on Insobot!


Features:
-----------------------------------------


-Hot Loading (Can add your own modules without restarting th bot)

-Works with Twitch and IRC

-SO Plugins

-Easy to Install/Update your own Module (Just drag and drop your mod in the ./mod/ folder)

-Uses SSL


-------------------------------------------


Libs Requires


-Pthreads


-SSL 0.9 (optional)


To compile with GCC:

don't forgot to Add -ldl if you are on linux!

gcc irc.c -ldl -lpthread -I./<ssl folder/include -std=gnu99


To Compile .SO Files


OSX:

gcc -Wall -fPIC -c example.c -I<link to openssl> -w -g -lssl -lcrypto

Note: 

For 0.9, I just linked the include folder in the openSSL source

For > 1.0 I compiled the source and install OpenSSL myself but I had to add -L<link to the openSSL source folder> for some reason


Linux:

gcc -shared -Wl,-soname,libctest.so.1 -o example.so   example.o -lssl -lcrypto


OSX:

gcc -I<ssl folder/include>/include -shared -Wl,-install_name,myexampleso.1 -o myexample.so.1.0 ytsearch.o  -lssl -lcrypto -g


then move the .So to the MODS folder and it will run on start on or autoload if the bot is already running.



There's also a config file called config.txt where you can enter the network address, your nick and name and the rooms you want to join.


KNOW BUGS:

If you get a Stack Top is NULL message, ctrl-c out and restart the bot. I am not sure what causes it.

Edit: I found out that the live loading is what causes this bug to happen! I'm not sure how to fix it yet.



Thanks to all!! Insofaras, Insobot, Casey and Everyone at Handmade Hero!
