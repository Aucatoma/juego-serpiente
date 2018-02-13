/*--------------------------------------------------------------------
* snake_ncurses.c
*	Juego de snake utilizando la librería ncurses para manejar
*	la terminal de salida stdscr y usando programación orientada
*	a objetos.
*/

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <list>

using namespace std;

int altura, ancho;
WINDOW *win;

class Punto{
	public:
		int x, y;
	public:
		Punto(int a, int b): x(a), y(b){};
};

class Snake {
	public:
	int longitud;
	int vidas;
	int direccion;
	int simbolo;
	list<Punto*> cuerpo;
	list<Punto*>::iterator itCuerpo;
	int puntuacion;
	public:
		Snake(int, int, int, int, Punto *punto);
		void dibujarSnake();
		void borrarSnake();
		void moverSnake();
		void crecerSnake();
		void morirSnake();		
};

Snake::Snake(int _longitud, int _vidas, int _direccion, int _simbolo, Punto *_punto){
	longitud = _longitud;
	vidas = _vidas;
	direccion = _direccion;
	simbolo = _simbolo;
	cuerpo.push_back(_punto);
	puntuacion = 0;
}


void Snake::dibujarSnake(){
	for(itCuerpo = cuerpo.begin(); itCuerpo != cuerpo.end(); itCuerpo++){
		wmove(win, (*itCuerpo)->y, (*itCuerpo)->x);
		wprintw(win, "%c", simbolo);
		wmove(win, cuerpo.front()->y, cuerpo.front()->x);
	}
	wrefresh(win);
}

void Snake::borrarSnake(){	
	for(itCuerpo = cuerpo.begin(); itCuerpo != cuerpo.end(); itCuerpo++){
		wmove(win, (*itCuerpo)->y, (*itCuerpo)->x);
		wprintw(win, "%c", 32);
		wmove(win, cuerpo.front()->y, cuerpo.front()->x);
	}
	wrefresh(win);
}

void Snake::moverSnake(){
	int tecla, dir_ant;
	int x = (cuerpo.front())->x;
	int y = (cuerpo.front())->y;
	dir_ant = direccion;
	if((tecla = getch()) != ERR)
		direccion = tecla;
	
	borrarSnake();
	
	if(cuerpo.size() == longitud)
		cuerpo.pop_back();

	if((direccion == KEY_UP && dir_ant == KEY_DOWN)
		|| (direccion == KEY_DOWN && dir_ant == KEY_UP)
		|| (direccion == KEY_LEFT && dir_ant == KEY_RIGHT) 
		|| (direccion == KEY_RIGHT && dir_ant == KEY_LEFT)){
		direccion = dir_ant;
	}

	switch(direccion){
			case KEY_DOWN:
				if( y < altura )
					y++;
				break;
			case KEY_UP:
				if( y > 0 )
					y--;
				break;
			case KEY_LEFT:
				if( x > 0 )
					x--;
				break;
			case KEY_RIGHT:
				if( x < ancho )
					x++;
				break;
	}
	cuerpo.push_front(new Punto(x, y));
	dibujarSnake();
	while(getch()!=ERR){}
}

void Snake::crecerSnake(){
	if(longitud < 30)
		longitud++;
}

void Snake::morirSnake(){
	borrarSnake();
	
	if(direccion == KEY_UP || direccion == KEY_DOWN)
		simbolo = 124;
	else
		simbolo = 95;
	dibujarSnake();
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
	wrefresh(win);
	sleep(1);
	simbolo = 79;
	dibujarSnake();
	wrefresh(win);
	sleep(1);
	if(direccion == KEY_UP || direccion == KEY_DOWN)
		simbolo = 124;
	else
		simbolo = 95;
	dibujarSnake();
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
	wrefresh(win);
	sleep(1);
	simbolo = 79;
}

class Comida{
	public:
		int score;
		int x, y, simbolo;
	public:
		Comida(int a, int b, int c): x(a), y(b), simbolo(c){
		score = 50;
		};	
		void dibujarComida();
		void borrarComida();
};

void Comida::dibujarComida(){
	wmove(win, y, x);
	wprintw(win, "%c", simbolo);
	wrefresh(win);
}

void Comida::borrarComida(){
	wmove(win, y, x);
	wprintw(win, "%c", 32);
	wrefresh(win);
}

class ComidaEspecial: public Comida{
	public:
	ComidaEspecial (int a, int b, int c):Comida(a, b, c){
		score = 100;
	};
};

class ControladorSnake{
	public:
		Snake *snake;
		list<Comida*> *comidas;
		list<Comida*>::iterator itComidas;
	public:
		ControladorSnake(Snake *_snake, list<Comida*> *_comidas): snake(_snake), comidas(_comidas){};
		int colisionarComida();
		bool colisionarParedes();
		bool colisionarCuerpo();
};


int ControladorSnake::colisionarComida(){
	Punto *cabeza_snake = snake->cuerpo.front();
	for(itComidas = comidas->begin(); itComidas != comidas->end(); itComidas++)
		if((*itComidas)->x == cabeza_snake->x && (*itComidas)->y == cabeza_snake->y)
			return (*itComidas)->score;
	
	return 0;
}

bool ControladorSnake::colisionarParedes(){
	Punto *cabeza_snake = snake->cuerpo.front();
	if(cabeza_snake->x == ancho - 1 || cabeza_snake->y == altura - 2 || cabeza_snake->x == 0 || cabeza_snake->y == 0)
		return true;
	return false;
}


bool ControladorSnake::colisionarCuerpo(){
	int i = 0;
	Punto *cabeza_snake = snake->cuerpo.front();
	for(snake->itCuerpo = snake->cuerpo.begin(); snake->itCuerpo != snake->cuerpo.end(); snake->itCuerpo++){
			if(i == 0){
				i++;
				continue;
			}
			//move(0, 90);
			//printw("cabeza:%d %d cuerpo:%d %d", cabeza_snake->x, cabeza_snake->y, (*(snake->itCuerpo))->x, (*(snake->itCuerpo))->y);
			//sleep(3);
		if(cabeza_snake->x == (*(snake->itCuerpo))->x && cabeza_snake->y == (*(snake->itCuerpo))->y)
			return true;
	}
	return false;
}

class Juego{
	public:
		bool gana, pierde;
		timeval ti, tf, tc;
		int puntuacion;
		ControladorSnake *controladorSnake;
		Comida *comida;
		ComidaEspecial *comida_especial;
	public:
		Juego(ControladorSnake *_controladorSnake, Comida *_comida, ComidaEspecial *_comida_especial): 
			controladorSnake(_controladorSnake), gana(false), pierde(false), comida(_comida), 
			comida_especial(_comida_especial){
				puntuacion = 0;
				tc.tv_usec = -1;
		};
		void controlarJuego();
		bool controlarPosComida(Comida *comida);
		void inicializarJuego();
		void dibujarPanelVidas();
		void limpiarPanelVidas();
		unsigned int * obtenerTiempoJuego();
};

void Juego::controlarJuego(){
	srand(time(NULL));
	puntuacion = controladorSnake->colisionarComida();
	gettimeofday(&tf, NULL);
	
	if(puntuacion == 50){
		controladorSnake->snake->puntuacion += puntuacion;
		comida->borrarComida();
		do{
			comida->x = rand() % (ancho - 2) + 1;
			comida->y = rand() % (altura - 3) + 1;
		}while(controlarPosComida(comida));
		comida->dibujarComida();
	}

	if(puntuacion == 100){
		controladorSnake->snake->puntuacion += puntuacion;
		comida_especial->borrarComida();
		controladorSnake->comidas->pop_back();
		move(0,90);
		printw("%c%c%c%c%c%c%c%c", 32, 32, 32, 32, 32, 32, 32, 32);
		refresh();
	}

	if((((tf.tv_sec - ti.tv_sec) % 20) == 0) && (controladorSnake->comidas->size() == 1)){
		controladorSnake->comidas->push_back(comida_especial);
		do{
			comida_especial->x = rand() % (ancho - 2) + 1;
			comida_especial->y = rand() % (altura - 3) + 1;
		}while(controlarPosComida(comida_especial));
		comida_especial->dibujarComida();
		gettimeofday(&tc, NULL);
		tc.tv_sec--;
	}
	
	
	if((controladorSnake->comidas->size() == 2) && (((tf.tv_sec - tc.tv_sec) % 11) == 0)){
		controladorSnake->comidas->pop_back();
		comida_especial->borrarComida();
		move(0, 90);
		printw("%c%c%c%c%c%c%c%c", 32, 32, 32, 32, 32, 32, 32, 32);
		refresh();
	}
	
	if(controladorSnake->comidas->size() == 2){
		move(0,90);
		printw("%d (sec)", 10 - (tf.tv_sec - tc.tv_sec));
		refresh();	
	}
	if(controladorSnake->snake->puntuacion % 150 == 0 && puntuacion != 0)
		controladorSnake->snake->crecerSnake();

	if(controladorSnake->snake->puntuacion == 10000)
		gana = true;
	if(controladorSnake->colisionarParedes() || controladorSnake->colisionarCuerpo()){
		pierde = true;
		controladorSnake->snake->vidas == 0;
	}
}

void Juego::inicializarJuego(){
	controladorSnake->comidas->push_front(comida);
	comida->dibujarComida();
	controladorSnake->snake->dibujarSnake();
	move(0,0);
	printw("Vidas: %d Puntuación:%d     Pulsa una tecla de las flechas para jugar!", controladorSnake->snake->vidas,controladorSnake->snake->puntuacion);
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
	refresh();
	wrefresh(win);
}


bool Juego::controlarPosComida(Comida *comida){
			for(controladorSnake->snake->itCuerpo = controladorSnake->snake->cuerpo.begin();
				controladorSnake->snake->itCuerpo != controladorSnake->snake->cuerpo.end(); 
					controladorSnake->snake->itCuerpo++)
				if((*controladorSnake->snake->itCuerpo)->x == comida->x || (*controladorSnake->snake->itCuerpo)->y == comida->y){
					return true;
			
}	
		
			return false;
}

void Juego::dibujarPanelVidas(){
	limpiarPanelVidas();
	move(0, 0);
	printw("Vidas: %d\tPuntuación:%c%c%d%c%c%c", controladorSnake->snake->vidas, 32, 32, controladorSnake->snake->puntuacion, 32, 32, 32);
	refresh();
}

void Juego::limpiarPanelVidas(){
	int i = 0;
	move(0, 0);
	for(i = 0; i < 90; i++){
		printw("%c", 32);
	}
}

unsigned int * Juego::obtenerTiempoJuego(){
	unsigned int *tiempo;
	
	tiempo = (unsigned int *) malloc(sizeof(unsigned int)*3);
	
	tiempo[2] = tf.tv_sec - ti.tv_sec;
	
	tiempo[1] = (unsigned int) (tiempo[2] / 60);

	tiempo[2] = tiempo[2] % 60;

	tiempo[0] = (unsigned int) (tiempo[1] / 60);
	
	tiempo[1] = tiempo[1] % 60;	
	
	return tiempo;
}

int main(int argc, char *argv[]){
	/*Inicialización de la ventana*/
	initscr();
	getmaxyx(stdscr, altura, ancho);
	noecho();		/*Deshabilita el echo para mostrar caracteres en la terminal*/	
	//cbreak(); 		/*Para obtener un caracter a la vez y no hacer buffering*/
	keypad(stdscr, TRUE);	/*Habilita captura de Backspace, Delete y Arrows*/
	curs_set(0);		/*Deshabilita el flickering del cursor*/
	
	win = newwin(altura - 1,  ancho, 1, 0);
	/*Inicializar objetos del juego*/
	
	int inicio;
	double velocidad =1.10;
	unsigned int *tiempo;
	srand(time(NULL));
	list<Comida*> comidas;
	Snake snake (2, 1, -1, 79, new Punto(5,5)); 
	snake.cuerpo.push_back(new Punto(4, 5));
	Comida comida (rand() % (ancho - 2) + 1, 
			rand() % (altura - 3) + 1, 42);
	ComidaEspecial comida_especial (rand() % (ancho - 2) + 1, 
					rand() % (altura - 3) + 1, 36);
	ControladorSnake controladorSnake(&snake, &comidas);
	Juego juego(&controladorSnake, &comida, &comida_especial);
	juego.inicializarJuego();
	iniciar:
		inicio = getch();
	switch(inicio){
		case KEY_UP:
			snake.direccion = KEY_UP;
			break;
		case KEY_DOWN:
			snake.direccion = KEY_DOWN;
			break;
		case KEY_LEFT:
			snake.direccion = KEY_LEFT;
			break;
		case KEY_RIGHT:
			snake.direccion = KEY_RIGHT;
			break;
		default:
			goto iniciar;
	}
	nodelay(stdscr, TRUE);
	gettimeofday(&(juego.ti), NULL);
	while(getch() != ERR){}
	sleep(1);
	while(!juego.gana && !juego.pierde){
		juego.dibujarPanelVidas();
		wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
		snake.moverSnake();
		juego.controlarJuego();
		usleep((useconds_t) (velocidad*62500));
	}
	tiempo = juego.obtenerTiempoJuego();
	snake.morirSnake();
	juego.limpiarPanelVidas();
	move(0, 0);
	printw("Tu puntuación fue %d - Tiempo de Juego %u(h) %u(min) %u(sec)", snake.puntuacion, tiempo[0], tiempo[1], tiempo[2]);
	refresh();
	sleep(4);
	delwin(win);
	endwin();

}
