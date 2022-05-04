clear
gcc -o main main.c configuration.c for_thread.c information.c other_methods.c projectile.c tank.c singly_linked_list.c -lpthread
valgrind --leak-check=full --track-origins=yes ./main