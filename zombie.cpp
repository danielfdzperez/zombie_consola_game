#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#define EXIT 27
#define NENEMIES 1000
#define SPACE 32
#define VEL 100
#define ENEMY 'x'
#define WEAPONS 2
enum TSide {left, top, right, bottom};
enum TOrientation {north, east, south, west};
enum TGuns {gun, rifle};

struct TScreen{
    char **board;
    int size_x;
    int size_y;
    int max_x;
    int max_y;
    int border;
};

struct TPosition{
    int x;
    int y;
};

struct TShot{
    struct TPosition pos;
    bool is_moving;
};

struct TWeapon{
    int max_amo;//max amo
    int max_shots;//number of shots that can be fired
    int bullets_per_shot;
    int live;//live rest the gun
    int index;
    struct TShot *bullets;//number of bullets, the gun can shot
};

struct TCharacter{
    int moviment; //the direction
    int life; //the live 
    int posx; //x position = column
    int posy; //y position = row
    char look; //character ex: 'M'
    bool is_moving; //especify the enemy is in moviment
    int move; //the time needs a zombie to move
    int attack;//the time needs a zombie to atack
    int orientation;
    int selected_weapon;
};

void fill_in(struct TScreen board_game){
    for(int i=0; i<board_game.size_y; i++)
	memset(board_game.board[i], ' ', board_game.size_x * sizeof(char));
    for(int y=0; y<board_game.size_y; y++)
	for(int x=0; x<board_game.size_x; x++)
	    if(y == 0 || y == board_game.size_y - 1 || x == 0 || x == board_game.border)
		board_game.board[y][x] = 'H';
}

char **resize(struct TScreen *board_game){
    int diferencia;
    if(board_game->size_x > COLS-1){
	board_game->size_x = COLS - 1;
	board_game->border = board_game->size_x - 10;
    }

    if(board_game->size_y > LINES)
	board_game->size_y = LINES;

    if(board_game->size_x < COLS-1)
	if (COLS - 1 <= board_game->max_x){
	    board_game->size_x = COLS - 1;
	    board_game->border = board_game->size_x - 10;
	}
	else{
	    diferencia = (COLS-1) - board_game->size_x;
	    board_game->max_x = board_game->size_x = COLS - 1; 
	    board_game->border = board_game->size_x - 10;
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

/*to do: change board to board_game.board*/
void enemies_spawn(struct TCharacter enemy[], int *enemy_index, char **board, struct TScreen board_game){
    bool be_born=false;
    int pos;
    if(rand() % 56 == 3){
	switch(rand() % 4){
	    case left:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % board_game.size_y;
		    if(board[pos][1] != ENEMY && board[pos][1] != 'H'){
			enemy[*enemy_index].posy = pos;
			enemy[*enemy_index].posx = 1;
			be_born = true;
		    }
		}
		break;
	    case top:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % board_game.border;
		    if(board[1][pos] != ENEMY && board[1][pos] != 'H'){
			enemy[*enemy_index].posy = 1;
			enemy[*enemy_index].posx = pos;
			be_born = true;
		    }
		}
		break;
	    case right:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % board_game.size_y;
		    if(board[pos][board_game.border-2] != ENEMY && board[pos][board_game.border-2] != 'H'){
			enemy[*enemy_index].posy = pos;
			enemy[*enemy_index].posx = board_game.border-2;
			be_born = true;
		    }
		}
		break;
	    case bottom:
		if(!enemy[*enemy_index].is_moving){
		    pos = rand() % board_game.border;
		    if(board[board_game.size_y-2][pos] != ENEMY && board[board_game.size_y-2][pos] != 'H'){
			enemy[*enemy_index].posy = board_game.size_y-2;
			enemy[*enemy_index].posx = pos;
			be_born = true;
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

void enemy_atack(struct TCharacter *player, struct TCharacter enemy[],int enemy_index){

}
/*Mejorar IA, cuando las columnas o las filas son menores que el otro va mal*/
void enemies_moviment(struct TCharacter enemy[NENEMIES], struct TCharacter player, int enemy_index, char **board){

    bool dont_muve = false;
    for(int move=0; move<enemy_index; move++){//pasa por todos los enemigos hasta el momento
	dont_muve = false;
	if(enemy[move].is_moving)
	    if(enemy[move].move >= VEL){
		if(player.posx == enemy[move].posx){

		    if(player.posy > enemy[move].posy){

			if(board[enemy[move].posy + 2][enemy[move].posx] == player.look || 
				board[enemy[move].posy + 1][enemy[move].posx] == player.look)
			    dont_muve = true;
			if(board[enemy[move].posy + 1][enemy[move].posx] == ENEMY)
			    dont_muve = true;

			if(!dont_muve){
			    board[enemy[move].posy][enemy[move].posx] = ' ';
			    enemy[move].posy++;
			}
		    }

		    else{
			if(board[enemy[move].posy - 2][enemy[move].posx] == player.look || 
				board[enemy[move].posy - 1][enemy[move].posx] == player.look)
			    dont_muve = true;
			if(board[enemy[move].posy - 1][enemy[move].posx] == ENEMY)
			    dont_muve = true;

			if(!dont_muve){
			    board[enemy[move].posy][enemy[move].posx] = ' ';
			    enemy[move].posy--;
			}
		    }
		}
		else
		    if(player.posy == enemy[move].posy){
			if(player.posx > enemy[move].posx){
			    if(board[enemy[move].posy][enemy[move].posx + 2] == player.look ||
				    board[enemy[move].posy][enemy[move].posx + 1] == player.look )
				dont_muve = true;
			    if(board[enemy[move].posy][enemy[move].posx + 1] == ENEMY)
				dont_muve = true;

			    if(!dont_muve){
				board[enemy[move].posy][enemy[move].posx] = ' ';
				enemy[move].posx++;
			    }
			}
			else{
			    if(board[enemy[move].posy][enemy[move].posx - 2] == player.look ||
				    board[enemy[move].posy][enemy[move].posx - 1] == player.look)
				dont_muve = true;
			    if(board[enemy[move].posy][enemy[move].posx - 1] == ENEMY)
				dont_muve = true;

			    if(!dont_muve){
				board[enemy[move].posy][enemy[move].posx] = ' ';
				enemy[move].posx--;
			    }
			}

		    }
		    else
			if(abs(player.posx - enemy[move].posx) < abs(player.posy - enemy[move].posy))
			    if(player.posx < enemy[move].posx){

				if(board[enemy[move].posy][enemy[move].posx - 1] != ENEMY){
				    board[enemy[move].posy][enemy[move].posx] = ' ';
				    enemy[move].posx --;
				}
			    }
			    else{
				if(board[enemy[move].posy][enemy[move].posx + 1] != ENEMY){
				    board[enemy[move].posy][enemy[move].posx] = ' ';
				    enemy[move].posx ++;
				}
			    }

			else
			    if(player.posy < enemy[move].posy){
				if(board[enemy[move].posy - 1][enemy[move].posx] != ENEMY){
				    board[enemy[move].posy][enemy[move].posx] = ' ';
				    enemy[move].posy --;
				}
			    }
			    else{
				if(board[enemy[move].posy + 1][enemy[move].posx] != ENEMY){
				    board[enemy[move].posy][enemy[move].posx] = ' ';
				    enemy[move].posy ++;
				}
			    }
		enemy[move].move = 0;
	    }
	    else
		enemy[move].move ++;
	board[enemy[move].posy][enemy[move].posx] = enemy[move].look;
    }
}

/*Actions of  player*/
void player_actions(struct TCharacter *player, struct TCharacter enemy[], char **board){

    switch(player->orientation){
	case east:
	    board[player->posy][player->posx+1] = ' ';
	    break;
	case west:
	    board[player->posy][player->posx-1] = ' ';
	    break;
	case north:
	    board[player->posy-1][player->posx] = ' ';
	    break;
	case south:
	    board[player->posy+1][player->posx] = ' ';
	    break;
    }

    switch(player->moviment){

	case KEY_RIGHT:
	    if(board[player->posy][player->posx + 2] != 'H' && board[player->posy][player->posx + 2] != ENEMY &&
		    board[player->posy][player->posx + 1] != ENEMY){
		board[player->posy][player->posx] = ' ';
		player->posx ++;
		player->orientation = east;
	    }
	    break;

	case KEY_LEFT:
	    if(board[player->posy][player->posx - 2] != 'H' && board[player->posy][player->posx - 2] != ENEMY &&
		    board[player->posy][player->posx - 1] != ENEMY){
		board[player->posy][player->posx] = ' ';
		player->posx --;
		player->orientation = west;
	    }
	    break;

	case KEY_UP:
	    if(board[player->posy - 2][player->posx] != 'H' && board[player->posy - 2][player->posx] != ENEMY &&
		    board[player->posy - 1][player->posx] != ENEMY){
		board[player->posy][player->posx] = ' ';
		player->posy --;
		player->orientation = north;
	    }
	    break;

	case KEY_DOWN:
	    if(board[player->posy + 2][player->posx] != 'H' && board[player->posy + 2][player->posx] != ENEMY &&
		    board[player->posy + 1][player->posx] != ENEMY){
		board[player->posy][player->posx] = ' ';
		player->posy += 1;
		player->orientation = south;
	    }
	    break;

	case SPACE:
	    break;

	case 'z':
	    if (player->selected_weapon > 0)
		player->selected_weapon --;
	    break;
	case 'x':
	    if (player->selected_weapon < WEAPONS-1)
		player->selected_weapon++;
	    break;
    }
    switch(player->orientation){
	case east:
	    board[player->posy][player->posx+1] = '=';
	    break;
	case west:
	    board[player->posy][player->posx-1] = '=';
	    break;
	case north:
	    board[player->posy-1][player->posx] = '=';
	    break;
	case south:
	    board[player->posy+1][player->posx] = '=';
	    break;
    }
    board[player->posy][player->posx] = player->look;
}

void print_game(struct TCharacter player, struct TCharacter enemy[NENEMIES], int enemy_index, char **tablero, int tamano_x, int tamano_y){
    erase();
    for(int y=0; y<tamano_y; y++){
	for(int x=0; x<tamano_x; x++)
	    printw("%c",tablero[y][x]);
	printw("\n");
    }
    //mvprintw(5, 10, "%i L%i C%i", enemy_index, LINES, COLS);
    mvprintw(5, 10, "%i", player.selected_weapon);
    refresh();
}
void loop_game(){

    int enemy_index;
    struct TCharacter player;
    struct TCharacter enemy[NENEMIES];
    struct TScreen board_game;
    struct TWeapon guns[WEAPONS];

    player.look = 'O';
    player.posy = player.posx = 10;
    player.life = 100;
    player.orientation = east;
    player.selected_weapon = gun;

    //initialize the gun
    guns[gun].max_amo = 9;
    guns[gun].max_shots = 5;
    guns[gun].bullets_per_shot = 1;
    guns[gun].live = 5;
    guns[gun].index = 0;
    guns[gun].bullets = (struct TShot*) malloc(guns[gun].max_shots * sizeof(struct TShot));


    for(int i = 0; i < 5; i++)
	guns[gun].bullets[i].is_moving = false;

    //initialize the enemies
    for(int i = 0; i < NENEMIES; i++){
	enemy[i].is_moving = false;
	enemy[i].look = ENEMY;
	enemy[i].move = 0;
	enemy[i].life = 50;
    }

    //initialize the board_game
    board_game.max_x = board_game.size_x = COLS - 1;
    board_game.max_y = board_game.size_y = LINES;
    board_game.border = board_game.size_x - 10;

    board_game.board = initialize(board_game);

    enemy_index = 0;

    do{
	timeout ( 5 );
	if(board_game.size_x < COLS-1 || board_game.size_x > COLS-1 || board_game.size_y < LINES || board_game.size_y > LINES)
	    board_game.board = resize(&board_game);
	print_game(player, enemy, enemy_index, board_game.board, board_game.size_x, board_game.size_y);
	if(enemy_index < NENEMIES )
	    enemies_spawn(enemy, &enemy_index, board_game.board, board_game);
	player.moviment = getch();
	player_actions(&player, enemy, board_game.board);
	enemies_moviment(enemy, player, enemy_index, board_game.board);
	//enemy_atack(&player, enemy, enemy_index);

    }while(player.moviment != EXIT);

    for(int i = 0; i<board_game.size_y; i++)
	free(board_game.board[i]);
    free(board_game.board);
    free(guns[gun].bullets);
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
