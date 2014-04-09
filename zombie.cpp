#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define EXIT 27
#define NENEMIES 100
#define SPACE 32
#define VEL 100
enum TSide {left, top, right, bottom};

struct TPosition{
    int x;
    int y;
};

struct TWeapon{
    struct TPosition pos;
    bool is_moving;
    int live;
};
struct TCharacter{
    int moviment; //the direction
    int live; //the live 
    int posx; //x position = column
    int posy; //y position = row
    char look; //character ex: 'M'
    bool is_moving; //especify the enemy is in moviment
    int move; //?
};

void enemies_spawn(struct TCharacter enemy[], int *enemy_index){
    if(rand() % 56 == 7){
	switch(rand() % 4){
	    case left:
		if(!enemy[*enemy_index].is_moving){
		    enemy[*enemy_index].posy = rand() % LINES;
		    enemy[*enemy_index].posx = 0;
		}
		break;
	    case top:
		if(!enemy[*enemy_index].is_moving){
		    enemy[*enemy_index].posy = 0;
		    enemy[*enemy_index].posx = rand() % COLS;
		}
		break;
	    case right:
		if(!enemy[*enemy_index].is_moving){
		    enemy[*enemy_index].posy = rand() % LINES;
		    enemy[*enemy_index].posx = COLS-20;
		}
		break;
	    case bottom:
		if(!enemy[*enemy_index].is_moving){
		    enemy[*enemy_index].posy = LINES-1;
		    enemy[*enemy_index].posx = rand() % COLS;
		}
		break;
	}

	enemy[*enemy_index].is_moving = true;
	(*enemy_index) ++;
    }
}

void enemies_moviment(struct TCharacter enemy[NENEMIES], struct TCharacter player){

    bool dont_muve = false;
    for(int move=0; move<NENEMIES; move++){
	dont_muve = false;
	if(enemy[move].is_moving)
	    if(enemy[move].move >= VEL){
		if(player.posx == enemy[move].posx){

		    if(player.posy > enemy[move].posy){

			if(player.posx == enemy[move].posx && player.posy == enemy[move].posy + 1)
			    dont_muve = true;
			for(int i=0; i<NENEMIES && dont_muve == false; i++)
			    if(enemy[i].posx == enemy[move].posx && enemy[i].posy == enemy[move].posy + 1)
				dont_muve = true;

			if(!dont_muve)
			    enemy[move].posy++;
		    }

		    else{
			if(player.posx == enemy[move].posx && player.posy == enemy[move].posy - 1)
			    dont_muve = true;
			for(int i=0; i<NENEMIES && dont_muve == false; i++)
			    if(enemy[i].posx == enemy[move].posx && enemy[i].posy == enemy[move].posy - 1)
				dont_muve = true;

			if(!dont_muve)
			    enemy[move].posy--;
		    }
		}
		else
		    if(player.posy == enemy[move].posy){
			if(player.posx > enemy[move].posx){
			    if(player.posx == enemy[move].posx + 1 && player.posy == enemy[move].posy)
				dont_muve = true;
			    for(int i=0; i<NENEMIES && dont_muve == false; i++)
				if(enemy[i].posx == enemy[move].posx + 1 && enemy[i].posy == enemy[move].posy)
				    dont_muve = true;

			    if(!dont_muve)
				enemy[move].posx++;
			}
			else{
			    if(player.posx == enemy[move].posx - 1 && player.posy == enemy[move].posy)
				dont_muve = true;
			    for(int i=0; i<NENEMIES && dont_muve == false; i++)
				if(enemy[i].posx == enemy[move].posx - 1 && enemy[i].posy == enemy[move].posy)
				    dont_muve = true;

			    if(!dont_muve)
				enemy[move].posx--;
			}

		    }
		    else
			if(abs(player.posx - enemy[move].posx) < abs(player.posy - enemy[move].posy))
			    if(player.posx < enemy[move].posx){

				enemy[move].posx --;
			    }
			    else{
				enemy[move].posx ++;
			    }

			else
			    if(player.posy < enemy[move].posy){
				enemy[move].posy --;
			    }
			    else{
				enemy[move].posy ++;
			    }
		enemy[move].move = 0;
	    }
	    else
		enemy[move].move ++;
    }
}

/*Actions of  player*/
void player_actions(struct TCharacter *player, struct TCharacter enemy[]){

    bool enemy_position = false;
    switch(player->moviment){

	case KEY_RIGHT:
	    if(player->posx + 1 != COLS-1){
		for(int i=0; i<NENEMIES && enemy_position == false; i++)
		    if(player->posx + 1 == enemy[i].posx && player->posy == enemy[i].posy)
			enemy_position = true;
		if(!enemy_position)
		    player->posx ++;
	    }
	    break;

	case KEY_LEFT:
	    if(player->posx - 1 != 0){
		for(int i=0; i<NENEMIES && enemy_position == false; i++)
		    if(player->posx - 1 == enemy[i].posx && player->posy == enemy[i].posy)
			enemy_position = true;
		if(!enemy_position)
		    player->posx -= 1;
	    }
	    break;

	case KEY_UP:
	    if(player->posy - 1 != 0){
		for(int i=0; i<NENEMIES && enemy_position == false; i++)
		    if(player->posx  == enemy[i].posx && player->posy - 1 == enemy[i].posy)
			enemy_position = true;
		if(!enemy_position)
		    player->posy -= 1;
	    }
	    break;

	case KEY_DOWN:
	    if(player->posy + 1 != LINES-1){
		for(int i=0; i<NENEMIES && enemy_position == false; i++)
		    if(player->posx == enemy[i].posx && player->posy + 1 == enemy[i].posy)
			enemy_position = true;
		if(!enemy_position)
		    player->posy += 1;
	    }
	    break;

	case SPACE:
	    break;
    }
}

void print_game(struct TCharacter player, struct TCharacter enemy[NENEMIES], int enemy_index){
    clear();
    mvprintw(player.posy, player.posx, "%c", player.look);
    for(int print_enemies = 0; print_enemies < NENEMIES; print_enemies ++)
	if(enemy[print_enemies].is_moving)
	    mvprintw(enemy[print_enemies].posy, enemy[print_enemies].posx, "%c", enemy[print_enemies].look);
    printw("row = %i col = %i", abs(enemy[0].posy-player.posy), abs(enemy[0].posx-player.posx));
    refresh();

}
void loop_game(){

    int enemy_index;
    struct TCharacter player;
    struct TCharacter enemy[NENEMIES];
    struct TWeapon gun[5];

    player.look = 'P';
    player.posy = player.posx = 10;

#ifdef BETA    
    for(int i = 0; i < ; i++){
    }
#endif


    for(int i = 0; i < NENEMIES; i++){
	enemy[i].is_moving = false;
	enemy[i].look = 'M';
	enemy[i].move = 0;
    }


    enemy_index = 0;

    do{
	timeout ( 5 );
	print_game(player, enemy, enemy_index);
	if(enemy_index < NENEMIES )
	    enemies_spawn(enemy, &enemy_index);
	player.moviment = getch();
	player_actions(&player, enemy);
	enemies_moviment(enemy, player);
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
