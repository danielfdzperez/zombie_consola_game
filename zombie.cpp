#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define EXIT 27
#define NENEMIES 5
struct TCharacter{
    int moviment;
    int live;
    int posx;
    int posy;
    char look;
    bool is_moving;
};

void enemies_spawn(struct TCharacter enemy[], int *enemy_index){

    //if(rand() % 100 == 37)
    if(*enemy_index < NENEMIES)
	if(!enemy[*enemy_index].is_moving){
	    enemy[*enemy_index].look = 'M';
	    enemy[*enemy_index].posy = enemy[*enemy_index].posx = 20+ rand() % 5;
	    enemy[*enemy_index].is_moving = true;
	    (*enemy_index) ++;
	}

}

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

void enemies_moviment(struct TCharacter enemy[NENEMIES], int *enemy_index){

    for(int move=0; move<NENEMIES; move++){
	if(enemy[move].posx + 1 == COLS){
	    enemy[move].is_moving = false;
	    (*enemy_index) --;
	}
	enemy[move].posx += 1;
    }


}

void print_game(struct TCharacter player, struct TCharacter enemy[NENEMIES], int enemy_index){
    clear();
    mvprintw(player.posy, player.posx, "%c", player.look);
    printw("%i\n", enemy_index);
    for(int print_enemies = 0; print_enemies < NENEMIES; print_enemies ++)
	if(enemy[print_enemies].is_moving)
	    mvprintw(enemy[print_enemies].posy, enemy[print_enemies].posx, "%c", enemy[print_enemies].look);
    refresh();

}
void loop_game(){

    int enemy_index;
    struct TCharacter player;
    struct TCharacter enemy[NENEMIES];

    player.look = 'P';
    player.posy = player.posx = 10;

    for(int i = 0; i < NENEMIES; i++)
	enemy[i].is_moving = false;

    enemy_index = 0;

    do{
	timeout ( 200 );
	print_game(player, enemy, enemy_index);
	enemies_spawn(enemy, &enemy_index);
	player.moviment = getch();
	player_moviment(&player);
	enemies_moviment(enemy, &enemy_index);
    }while(player.moviment != EXIT);

}

int main(int argc, char *argv[]){
    initscr();
    raw();
    keypad(stdscr, TRUE); 
    noecho();
    start_color();
    curs_set(0);
    srand(time(NULL));

    loop_game();

    endwin(); 
    return EXIT_SUCCESS;
}
