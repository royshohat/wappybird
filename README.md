# Wappybird

### This repository is multiplayer game designed in the C programming language using sockets, multithreading and more.

> **Note**: This repo is still in development

## Docs

Note: timestamp syncing helps calculate on way ping for a more accurate update and sync between client and server.

Packet types:
timestamp agreement, state update server, state update client, information about game settings (like acceleration, exchange at the beggin of the game) 

	1.	exchange details: time sync..? (I think would be a good idea, helpful for calculating one-way ping) Server agrees with client on game details like the Acceleration constant, bullet speed, etc.. assign a unique id to each client
	2.	Client pings server constantly, has a ready button synced with the server. and wait for others to join, if all the players are ready start a 3 second countdown then the game starts. Send start packet to all clients.
	3.	Clients must start send information to the server about key presses. Server listen to this in the main thread (select would be good here for multiple recv functions). Server gets the information, preforms a prediction algorithm on the client, then sends an update to all clients about their state according to the server, which they must follow. To sum up, Each handle client thread in the server sends the updates to the client, and the main thread listens to the clients

Client -> Server update packet structure:
timestamp, id of client, keys pressed, location pressed (ensures exact sync) 
 
(Server processes update)

Sever -> Clients. Broadcast update structure:
timestamp, id of updated client, new state details: location, velocity. Boolean is new projectile?, if true include projectile details: location (actually same as the update location so not needed..), velocity, sender ID

# PROTOCOL

**C is short for Client<br>
S is short for Server**
## Pre-game: 
### Client Join

[C -> S] (Request) Join Packet<br> 
[S -> C] (Response) Update Join Packet

### Initialize timing

> **Note**: At this step, a shared timestamp is agreed upon, it will be used to ensure sync accuracy between the Client and Server. Steps described below.

1. Server pings client 10 times to get the average **ping**. <br>
*ping* = *sendT* - *recvT* <br>
2. S notes the sendTimestamp and sends a timestamp request C: <br> 
 [S -> C] REQ_TIMESTAMP <br>
3. Responds with its **clientTimestamp**<br>
 [C -> S] Hey, my time stamp is clientTimestamp! <br>
4. Server receive it at preforms the following calculation: <br>

 *time_offset* = *sendTimestamp* - (*clientTimestamp* - *ping*) <br>

> **Note**: *time_offset* will be negative if the server time is before client's.<br>

5. Server sends client the time_offset *(-1)

<!-- TODO: read and make sure everything make sense -->

### Ready
[C -> S] (Request) Ready/Unready <br>
> **Note**: Each time a client sends a Ready Request, the server updates all clients about the client state:

[S -> C] (Broadcast) Other player ready state
b

## Game
