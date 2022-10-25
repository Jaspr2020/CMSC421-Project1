all:
	gcc -Wall -o LikeServer LikeServer.c
	gcc -Wall -o PrimaryLikesServer PrimaryLikesServer.c
	gcc -Wall -o ParentProcess ParentProcess.c

run:
	./ParentProcess