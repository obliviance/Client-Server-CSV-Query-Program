#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

struct Pokemon {
	int number;
	char name[30];
	char type1[10];
	char type2[10];
	int total;
	int HP;
	int attack;
	int defense;
	int sp_atk;
	int sp_def;
	int speed;
	int generation;
	int legendary;
} pokemon;


