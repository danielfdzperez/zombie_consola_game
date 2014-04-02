#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define EXIT 27
#define NENEMIES 100
struct TCharacter{
    int moviment;
    int live;
    int posx;
    int posy;
    char look;
};

/*Move player*/
void player_moviment(struct TCharacter *player){

    switch((*player).moviment){

	case KEY_RIGHT:
	    if((*player).posx + 1 != COLS)
		(*player).posx += 1;
	    
	    break;

	case KEY_LEFT:
	    if((*player).posx - 1 != 0)
		(*player).posx -= 1;
	    break;

	case KEY_UP:
	    if((*player).posy - 1 != 0)
		(*player).posy -= 1;
	    break;

	case KEY_DOWN:
	    if((*player).posy + 1 != LINES)
		(*player).posy += 1;
	    break;
    }
}


void print_game(struct TCharacter player, struct TCharacter enemy[NENEMIES]){
    clear();
    mvprintw(player.posy, player.posx, "%c", player.look);
    for(int print_enemies = 0; print_enemies < NENEMIES; )
    mvprintw(enemy[0].posy, enemy[0].posx, "%c", enemy[0].look);
    refresh();

}
void loop_game(){

    struct TCharacter player;
    struct TCharacter enemy[NENEMIES];
    player.look = 'P';
    player.posy = player.posx = 10;
    enemy[0].look = 'M';
    enemy[0].posy = enemy[0].posx = 20;
    do{
	print_game(player, enemy);
	player.moviment = getch();
	player_moviment(&player);
    }while(player.moviment != EXIT);

}

int main(int argc, char *argv[]){
    initscr();
    raw();
    keypad(stdscr, TRUE); 
    noecho();
    start_color();
    curs_set(0);

    loop_game();

    endwin(); 
    return EXIT_SUCCESS;
}
