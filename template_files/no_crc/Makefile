all:
	gcc src/client/client.c src/sha256/sha256.c src/libs/confirmations.c src/libs/file_operations.c -o client
	gcc src/server/server.c src/sha256/sha256.c src/libs/confirmations.c src/libs/file_operations.c -o server
	cp ../video.mp4 template_files/video.mp4
clean:
	rm server
	rm client
	rm template_files/video.mp4
