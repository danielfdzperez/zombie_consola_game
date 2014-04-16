#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define EXIT 27
#define NENEMIES 10000
#define SPACE 32
#define VEL 100
enum TSide {left, top, right, bottom};

struct TScreen{
    char **board;
    int size_x;
    int size_y;
    int max_x;
    int max_y;
};

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
    int life; //the live 
    int posx; //x position = column
    int posy; //y position = row
    char look; //character ex: 'M'
    bool is_moving; //especify the enemy is in moviment
    int move; //?
    int attack;
};

/*Testing*/
void pintar(char **tablero, int tamano_x, int tamano_y){

    clear();
    for(int y=0; y<tamano_y; y++){
	for(int x=0; x<tamano_x; x++)
	    printw("%c",tablero[y][x]);
	printw("\n");
    }
    refresh();
}

void fill_in(struct TScreen board_game){
    for(int i=0; i<board_game.size_y; i++)
	memset(board_game.board[i], ' ', board_game.size_x * sizeof(char));
    for(int y=0; y<board_game.size_y; y++)
	for(int x=0; x<board_game.size_x; x++)
	    if(y == 0 || y == board_game.size_y - 1 || x == 0 || x == board_game.size_x - 1)
		board_game.board[y][x] = 'H';
}

char **resize(struct TScreen *board_game){
    int diferencia;
    if(board_game->size_x > COLS-1)
	board_game->size_x = COLS - 1;

    if(board_game->size_y > LINES)
	board_game->size_y = LINES;

    if(board_game->size_x < COLS-1)
	if (COLS - 1 <= board_game->max_x)
	    board_game->size_x = COLS - 1;
	else{
	    diferencia = (COLS-1) - board_game->size_x;
	    board_game->max_x = board_game->size_x = COLS - 1; 
	    for (int i= 0; i<board_game->size_y; i++)
		board_game->board[i] = (char *) malloc(board_game->size_x * sizeof(char));
	}

    if(board_game->size_y < LINES)
	if(LINES <= board_game->max_y)
	    board_game->size_y = LINES;
	else{
	    diferencia = LINES - board_game->size_y;
	    board_game->max_y = board_game->size_y = LINES;
	    board_game->board =(char**) realloc(board_game->board, board_game->size_y * sizeof(char*));
	    for(int i = 0; i<diferencia; i++){
		board_game->board[board_game->size_y-(i+1)] = (char*) malloc(board_game->size_x * sizeof(char));
	    }
	}
    fill_in(*board_game);
    return board_game->board;
}

char ** initialize(struct TScreen board_game){
    board_game.board = (char **) malloc(board_game.size_y * sizeof (char *));
    for (int i= 0; i<board_game.size_y; i++)
	board_game.board[i] = (char *) malloc(board_game.size_x * sizeof(char));

    fill_in(board_game);
    return board_game.board;

}

void enemies_spawn(struct TCharacter enemy[], int *enemy_index){
    bool be_born=true;
    int pos;
    if(rand() % 56 == 3){
	switch(rand() % 4){
	    case left:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % LINES;
		    for(int i = 0; i < *enemy_index; i++)
			if(enemy[i].posy == pos && enemy[i].posx == 0){
			    be_born = false;
			    break;
			}
			else{
			    enemy[*enemy_index].posy = pos;
			    enemy[*enemy_index].posx = 0;
			}
		}
		break;
	    case top:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % COLS;
		    for(int i = 0; i < *enemy_index; i++)
			if(enemy[i].posy == 0 && enemy[i].posx == pos){
			    be_born = false;
			    break;
			}
			else{
			    enemy[*enemy_index].posy = 0;
			    enemy[*enemy_index].posx = pos;
			}
		}
		break;
	    case right:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % LINES;
		    for(int i = 0; i < *enemy_index; i++)
			if(enemy[i].posy == pos && enemy[i].posx == COLS-1){
			    be_born = false;
			    break;
			}
			else{
			    enemy[*enemy_index].posy = pos;
			    enemy[*enemy_index].posx = COLS-1;
			}
		}
		break;
	    case bottom:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % COLS;
		    for(int i = 0; i < *enemy_index; i++)
			if(enemy[i].posy == LINES-1 && enemy[i].posx == pos){
			    be_born = false;
			    break;
			}
			else{
			    enemy[*enemy_index].posy = LINES-1;
			    enemy[*enemy_index].posx = pos;
			}
		}
		break;
	}

	if(be_born){
	    enemy[*enemy_index].is_moving = true;
	    (*enemy_index) ++;
	}
    }
}

void enemies_moviment(struct TCharacter enemy[NENEMIES], struct TCharacter player, int enemy_index){

    bool dont_muve = false;
    for(int move=0; move<enemy_index; move++){//pasa por todos los enemigos hasta el momento
	dont_muve = false;
	if(enemy[move].is_moving)
	    if(enemy[move].move >= VEL){
		if(player.posx == enemy[move].posx){

		    if(player.posy > enemy[move].posy){

			if(player.posx == enemy[move].posx && player.posy == enemy[move].posy + 1)
			    dont_muve = true;
			for(int i=0; i<enemy_index && dont_muve == false; i++)
			    if(enemy[i].posx == enemy[move].posx && enemy[i].posy == enemy[move].posy + 1)
				dont_muve = true;

			if(!dont_muve)
			    enemy[move].posy++;
		    }

		    else{
			if(player.posx == enemy[move].posx && player.posy == enemy[move].posy - 1)
			    dont_muve = true;
			for(int i=0; i<enemy_index && dont_muve == false; i++)
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
			    for(int i=0; i<enemy_index && dont_muve == false; i++)
				if(enemy[i].posx == enemy[move].posx + 1 && enemy[i].posy == enemy[move].posy)
				    dont_muve = true;

			    if(!dont_muve)
				enemy[move].posx++;
			}
			else{
			    if(player.posx == enemy[move].posx - 1 && player.posy == enemy[move].posy)
				dont_muve = true;
			    for(int i=0; i<enemy_index && dont_muve == false; i++)
				if(enemy[i].posx == enemy[move].posx - 1 && enemy[i].posy == enemy[move].posy)
				    dont_muve = true;

			    if(!dont_muve)
				enemy[move].posx--;
			}

		    }
		    else
			if(abs(player.posx - enemy[move].posx) < abs(player.posy - enemy[move].posy))
			    if(player.posx < enemy[move].posx){
				for(int i=0; i<enemy_index && dont_muve == false; i++)
				    if(enemy[i].posx == enemy[move].posx - 1 && enemy[i].posy == enemy[move].posy)
					dont_muve = true;

				if(!dont_muve)
				    enemy[move].posx --;
			    }
			    else{
				for(int i=0; i<enemy_index && dont_muve == false; i++)
				    if(enemy[i].posx == enemy[move].posx + 1 && enemy[i].posy == enemy[move].posy)
					dont_muve = true;

				if(!dont_muve)
				    enemy[move].posx ++;
			    }

			else
			    if(player.posy < enemy[move].posy){
				for(int i=0; i<enemy_index && dont_muve == false; i++)
				    if(enemy[i].posx == enemy[move].posx && enemy[i].posy == enemy[move].posy - 1)
					dont_muve = true;

				if(!dont_muve)
				    enemy[move].posy --;
			    }
			    else{
				for(int i=0; i<enemy_index && dont_muve == false; i++)
				    if(enemy[i].posx == enemy[move].posx && enemy[i].posy == enemy[move].posy + 1)
					dont_muve = true;

				if(!dont_muve)
				    enemy[move].posy ++;
			    }
		enemy[move].move = 0;
	    }
	    else
		enemy[move].move ++;
    }
}

void enemy_atack(struct TCharacter *player, struct TCharacter enemy[],int enemy_index){

}
/*Actions of  player*/
void player_actions(struct TCharacter *player, struct TCharacter enemy[], char **board){

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

    board[player->posy][player->posx] = 'O';
}

void print_game(struct TCharacter player, struct TCharacter enemy[NENEMIES], int enemy_index){
    //clear();
    erase();
    mvprintw(player.posy, player.posx, "%c", player.look);
    for(int print_enemies = 0; print_enemies < NENEMIES; print_enemies ++)
	if(enemy[print_enemies].is_moving)
	    mvprintw(enemy[print_enemies].posy, enemy[print_enemies].posx, "%c", enemy[print_enemies].look);
    printw("row = %i col = %i", abs(enemy[0].posy-player.posy), abs(enemy[0].posx-player.posx));
    mvprintw(LINES/2, COLS/2, "%i", enemy_index);
    refresh();
}
void loop_game(){
    erase();
    int enemy_index;
    struct TCharacter player;
    struct TCharacter enemy[NENEMIES];
    struct TWeapon gun[5];
    struct TScreen board_game;

    player.look = 'P';
    player.posy = player.posx = 10;
    player.life = 100;

#ifdef BETA    
    for(int i = 0; i < ; i++){
    }
#endif


    for(int i = 0; i < NENEMIES; i++){
	enemy[i].is_moving = false;
	enemy[i].look = 'M';
	enemy[i].move = 0;
    }

    board_game.max_x = board_game.size_x = COLS - 1;
    board_game.max_y = board_game.size_y = LINES;

    board_game.board = initialize(board_game);

    enemy_index = 0;

    do{
		timeout ( 5 );
	if(board_game.size_x < COLS-1 || board_game.size_x > COLS-1 || board_game.size_y < LINES || board_game.size_y > LINES)
	    board_game.board = resize(&board_game);
	//print_game(player, enemy, enemy_index);
	if(enemy_index < NENEMIES )
	    enemies_spawn(enemy, &enemy_index);
	player.moviment = getch();
	player_actions(&player, enemy, board_game.board);
	enemies_moviment(enemy, player, enemy_index);
	enemy_atack(&player, enemy, enemy_index);

	//testing
	pintar( board_game.board, board_game.size_x, board_game.size_y);

    }while(player.moviment != EXIT);

    for(int i = 0; i<board_game.size_y; i++)
	free(board_game.board[i]);
    free(board_game.board);
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
