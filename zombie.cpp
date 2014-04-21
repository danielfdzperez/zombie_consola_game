#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <stdio_ext.h>

#define EXIT 27
#define NENEMIES 7
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
    int direction; 
    bool is_moving;
};

struct TWeapon{
    int max_amo;//max amo
    int amo;
    int max_shots;//number of shots that can be fired
    int bullets_per_shot;
    int live;//live rest the gun
    int index;
    struct TShot *bullets;//number of bullets, the gun can shot
    const char *name;
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
    int level;
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

void enemy_atack(int *player_life, struct TCharacter *enemy, struct TCharacter player){

    if(player.level < 15)
	(*player_life) -= player.level;
    else
	(*player_life) -= 25;
}

/*Mejorar IA, cuando las columnas o las filas son menores que el otro va mal*/
void enemies_moviment(struct TCharacter enemy[NENEMIES], struct TCharacter player, int enemy_index, char **board, int *player_life){

    bool attack = false;
    bool dont_muve = false;
    for(int move=0; move<enemy_index; move++){//pasa por todos los enemigos hasta el momento
	dont_muve = false;
	attack = false;
	if(enemy[move].is_moving){
	    if(enemy[move].move >= VEL){
		if(player.posx == enemy[move].posx){

		    if(player.posy > enemy[move].posy){

			if(board[enemy[move].posy + 1][enemy[move].posx] == '=' || 
				board[enemy[move].posy + 1][enemy[move].posx] == player.look){
			    dont_muve = true;
			    attack = true;
			}
			if(board[enemy[move].posy + 1][enemy[move].posx] == ENEMY)
			    dont_muve = true;

			if(!dont_muve){
			    board[enemy[move].posy][enemy[move].posx] = ' ';
			    enemy[move].posy++;
			}
		    }

		    else{
			if(board[enemy[move].posy - 1][enemy[move].posx] == '=' || 
				board[enemy[move].posy - 1][enemy[move].posx] == player.look){
			    dont_muve = true;
			    attack = true;;
			}
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
			    if(board[enemy[move].posy][enemy[move].posx + 1] == '=' ||
				    board[enemy[move].posy][enemy[move].posx + 1] == player.look ){
				dont_muve = true;
				attack = true;
			    }
			    if(board[enemy[move].posy][enemy[move].posx + 1] == ENEMY)
				dont_muve = true;

			    if(!dont_muve){
				board[enemy[move].posy][enemy[move].posx] = ' ';
				enemy[move].posx++;
			    }
			}
			else{
			    if(board[enemy[move].posy][enemy[move].posx - 1] == '=' ||
				    board[enemy[move].posy][enemy[move].posx - 1] == player.look){
				dont_muve = true;
				attack = true;
			    }
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

	    if(attack)
		enemy_atack(player_life, &enemy[move], player);
	}
    }
}

void bullet_iniciliazitaion(struct TShot *bullet, int player_posy, int player_posx){

    switch((enum TOrientation)bullet->direction){
	case east:
	    bullet->pos.y = player_posy;
	    bullet->pos.x = player_posx + 1;
	    break;
	case west:
	    bullet->pos.y = player_posy;
	    bullet->pos.x = player_posx - 1;
	    break;
	case north:
	    bullet->pos.y = player_posy - 1;
	    bullet->pos.x = player_posx;
	    break;
	case south:
	    bullet->pos.y = player_posy + 1;
	    bullet->pos.x = player_posx;
	    break;
    }
}

void move_bulets(struct TWeapon guns[WEAPONS], char **board, struct TCharacter enemies[NENEMIES], int enemy_index){

    for(int i = 0; i < WEAPONS; i++)
	for(int bullet = 0; bullet < guns[i].max_shots; bullet++)
	    if(guns[i].bullets[bullet].is_moving == true){
		board[guns[i].bullets[bullet].pos.y][guns[i].bullets[bullet].pos.x] = ' ';
		switch((enum TOrientation) guns[i].bullets[bullet].direction){
		    case east:
			guns[i].bullets[bullet].pos.x ++;
			break;
		    case west:
			guns[i].bullets[bullet].pos.x --;
			break;
		    case north:
			guns[i].bullets[bullet].pos.y --;
			break;
		    case south:
			guns[i].bullets[bullet].pos.y ++;
			break;
		}
		if(board[guns[i].bullets[bullet].pos.y][guns[i].bullets[bullet].pos.x] == 'H')
		    guns[i].bullets[bullet].is_moving = false;
		else
		    if(board[guns[i].bullets[bullet].pos.y][guns[i].bullets[bullet].pos.x] == ENEMY){
			for(int  enemy = 0; enemy < enemy_index; enemy++)
			    if(guns[i].bullets[bullet].pos.y == enemies[enemy].posy && guns[i].bullets[bullet].pos.x == enemies[enemy].posx){
				guns[i].bullets[bullet].is_moving = false;
				enemies[enemy].life -= guns[i].live;
				if(enemies[enemy].life < 1){
				    enemies[enemy].is_moving = false;
				    board[enemies[enemy].posy][enemies[enemy].posx] = 'G';
				}

			    }
		    }
		    else
			board[guns[i].bullets[bullet].pos.y][guns[i].bullets[bullet].pos.x] = '*';
	    }
}

/*Actions of  player*/
void player_actions(struct TCharacter *player, struct TCharacter enemy[], char **board, struct TWeapon guns[WEAPONS], int enemy_index){

    switch((enum TOrientation) player->orientation){
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
	    for(int bullet = 0; bullet < guns[player->selected_weapon].bullets_per_shot; bullet++ )
		if(!guns[player->selected_weapon].bullets[guns[player->selected_weapon].index].is_moving && 
			guns[player->selected_weapon].amo > 0){
		    guns[player->selected_weapon].bullets[guns[player->selected_weapon].index].is_moving = true;
		    guns[player->selected_weapon].bullets[guns[player->selected_weapon].index].direction = player->orientation;
		    bullet_iniciliazitaion(&guns[player->selected_weapon].bullets[guns[player->selected_weapon].index], 
			    player->posy, player->posx);
		    move_bulets(guns, board, enemy, enemy_index);
		    if(player->selected_weapon != gun)
			guns[player->selected_weapon].amo --;
		    if(++guns[player->selected_weapon].index == guns[player->selected_weapon].max_shots)
			guns[player->selected_weapon].index = 0;
		}
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

void print_game(char **board, int tamano_x, int tamano_y, struct TCharacter player, struct TWeapon guns[WEAPONS]){
    erase();
    //clear();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    for(int y=0; y<tamano_y; y++){
	for(int x=0; x<tamano_x; x++)
	    switch(board[y][x]){
		case 'H':
		    attron(COLOR_PAIR(1));
		    printw("%c",board[y][x]);
		    attroff(COLOR_PAIR(1));
		    break;
		case 'O':
		    attron(COLOR_PAIR(2));
		    printw("%c",board[y][x]);
		    attroff(COLOR_PAIR(2));
		    break;
		case 'x':
		    attron(COLOR_PAIR(3));
		    printw("%c",board[y][x]);
		    attroff(COLOR_PAIR(3));
		    break;
		case 'G':
		    attron(COLOR_PAIR(4));
		    printw("%c",board[y][x]);
		    attroff(COLOR_PAIR(4));
		    break;
		default:
		    printw("%c",board[y][x]);
	    }
	printw("\n");
    }
    mvprintw(2, COLS - 9, "Life %i", player.life);
    mvprintw(4, COLS - 9, "Weapon");
    mvprintw(5, COLS - 9, "%s", guns[player.selected_weapon].name);
    mvprintw(6, COLS - 9, "Amo %i", guns[player.selected_weapon].amo);
    mvprintw(9, COLS - 9, "Level %i", player.level);
    refresh();
}

void initialize_guns(struct TWeapon guns[WEAPONS]){
    //initialize the gun
    guns[gun].max_amo = 9;
    guns[gun].max_shots = 2;
    guns[gun].bullets_per_shot = 1;
    guns[gun].live = 5;
    guns[gun].name = "gun";
    guns[gun].bullets = (struct TShot*) malloc(guns[gun].max_shots * sizeof(struct TShot));


    
    //initialize the rifle
    guns[rifle].max_amo = 900;
    guns[rifle].max_shots = 15;
    guns[rifle].bullets_per_shot = 5;
    guns[rifle].live = 3;
    guns[rifle].name = "rifle";
    guns[rifle].bullets = (struct TShot*) malloc(guns[rifle].max_shots * sizeof(struct TShot));

    
}
void reinitialize_guns(struct TWeapon guns[WEAPONS]){
    guns[gun].amo = 999;
    guns[gun].index = 0;
    for(int i = 0; i < guns[gun].max_shots; i++)
	guns[gun].bullets[i].is_moving = false;

    guns[rifle].amo = 100;
    guns[rifle].index = 0;
    for(int i = 0; i < guns[rifle].max_shots; i++)
	guns[rifle].bullets[i].is_moving = false;

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
    player.level = 1;

    initialize_guns(guns);


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
	reinitialize_guns(guns);

	do{
	    timeout ( 10 );
	    if(board_game.size_x < COLS-1 || board_game.size_x > COLS-1 || board_game.size_y < LINES || board_game.size_y > LINES)
		board_game.board = resize(&board_game);
	    print_game(board_game.board, board_game.size_x, board_game.size_y, player, guns);
	    if(enemy_index < NENEMIES )
		enemies_spawn(enemy, &enemy_index, board_game.board, board_game);
	    player.moviment = getch();
	    player_actions(&player, enemy, board_game.board, guns, enemy_index);
	    move_bulets(guns, board_game.board, enemy, enemy_index);
	    enemies_moviment(enemy, player, enemy_index, board_game.board, &player.life);
	}while(player.life > 0 && player.moviment != EXIT);

	clear();
	mvprintw(LINES/2, COLS/2, "Play again? ");
	refresh();

	__fpurge(stdin);
	player.moviment = getchar();
	__fpurge(stdin);
    }while(player.moviment != EXIT && player.moviment != 'n');

    for(int i = 0; i<board_game.size_y; i++)
	free(board_game.board[i]);
    free(board_game.board);
    for(int i = 0; i<WEAPONS; i++)
	free(guns[i].bullets);
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
