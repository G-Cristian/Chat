#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define COMMAND_NONE            0x00
//Command sent by client and received by server
#define CLIENT_CONNECTS         0x01
#define CLIENT_CREATES_GROUP    0x02
#define CLIENT_INVITES_CLIENT   0x03
#define CLIENT_JOINS_GROUP      0x04
#define CLIENT_LEAVES_GROUP     0x05
#define CLIENT_SENDS_MESSAGE    0x06
#define CLIENT_LEAVES_CHAT      0x07
//Command sent by server and received by client
#define SERVER_COMMAND_STATUS           0x11
#define SERVER_CLIENT_INVITED_CLIENT    0x12
#define SERVER_CLIENT_JOINED_GROUP      0x13
#define SERVER_CLIENT_LEFT_GROUP        0x14
#define SERVER_CLIENT_SENT_MESSAGE      0x15
#define SERVER_CLIENTS_IN_GROUP         0x16
//Status results
#define STATUS_NONE                     0x20
#define STATUS_OK                       0x21
#define STATUS_CLIENT_NAME_EXISTS       0x22
//STATUS_CLIENT_NAME_DOESNT_EXIST used in cases when a client invites another client.
//This status can indicate that the invited client doesn’t exist
#define STATUS_CLIENT_NAME_DOESNT_EXIST 0x23
#define STATUS_GROUP_NAME_EXISTS        0x24
#define STATUS_GROUP_NAME_DOESNT_EXIST  0x25
#define STATUS_RECEIVE_FAIL             0x26
#define STATUS_MESSAGE_SENT_WITH_FAILS  0x27
#define STATUS_SERVER_CRITICAL_ERROR    0x28

#endif
