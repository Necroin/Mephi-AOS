#ifndef _SERVERMESSAGES_H_
#define _SERVERMESSAGES_H_

char create_response[] = "Room created, waiting for second player...";
char create_error[] = "[Server] [Error] Room already created by other player";

char connect_response[] = "Connected to room, game started";
char other_connect_response[] = "Other player connected to room, game started";
char connect_error[] = "[Server] [Error] Room not exist";

char win_message[] = "You Win";
char lose_message[] = "Other player guessed number, you lost";

#endif