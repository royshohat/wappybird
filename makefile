client:
	gcc client.c -o client -I/usr/include/SDL3 -lSDL3 -lm -Wall -Wextra -pedantic

net_client:
	gcc net_client.c util.c -o net_client -Wall -Wextra -pedantic

server:
	gcc server.c util.c -o server -Wall -Wextra -pedantic

dbg_server:
	gcc server.c util.c -o server -Wall -Wextra -pedantic -g

