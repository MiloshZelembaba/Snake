/*
CS 349 A1 Skeleton Code - Snake

- - - - - - - - - - - - - - - - - - - - - -

Commands to compile and run:

    g++ -o snake snake.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./snake

Note: the -L option and -lstdc++ may not be needed on some machines.
*/

#include <iostream>
#include <sstream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <utility> 
#include <vector>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
/*
 * Global game state variables
 */
const int Border = 1;
const int BufferSize = 10;
 //int FPS = 30;
int FPS = 30;
 int pace = 1;
 unsigned long speedTime;
 int gameSpeed = 50000;
int iterations = 0;
bool fruitExpire = false;

const int width = 800;
const int height = 600;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	Pixmap	pixmap;		// double buffer
	GC		 gc[7];
	int		width;		// size of window
	int		height;
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}

// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};   
class PowerUp : public Displayable {
	public:
		bool paintPowerup;
		virtual void paint(XInfo &xinfo) {
			if (paintPowerup){
				XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[5], x, y, fruitSize, fruitSize);
				XDrawRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], x, y, fruitSize, fruitSize);
			}
			
        }

        PowerUp(int fruitSize): fruitSize(fruitSize){
        	paintPowerup = false;
        	createNewPowerUp();
        }

        int getX() { 
        	return x;
        }

        int getY() {
        	return y;
        }

        bool onMiddleCross(int x, int y){
        	if (x == width/2 && y >= height/4 && y <= 3*height/4){
        		return true;
        	}

        	if (y == height/2 && x >= width/4 && x <= 3*width/4){
        		return true;
        	}

        	if (x >= width - 12 * fruitSize && y <= fruitSize*2){
        		return true;
        	}

        	return false;
        }


        void createNewPowerUp(){
        	srand (now());
        	// generate x,y in a random location but so that they can't be on the borders
        	while (true){
	 			x = (1+(rand() % ((800 - 3*fruitSize)/fruitSize))) * fruitSize;
	 			y = (1+(rand() % ((600 - 3*fruitSize)/fruitSize))) * fruitSize;
	 			if (!onMiddleCross(x,y)){
	 				return;
	 			}
 			}
        }

    private:
    	int fruitSize;
        int x;
        int y;
};   

class Fruit : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[3], x, y, fruitSize, fruitSize);
			XDrawRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], x, y, fruitSize, fruitSize);
			
        }

        Fruit(int fruitSize): fruitSize(fruitSize){
        	createNewFruit();
        }

        int getX() { 
        	return x;
        }

        int getY() {
        	return y;
        }

        bool onMiddleCross(int x, int y){
        	if (x == width/2 && y >= height/4 && y <= 3*height/4){
        		return true;
        	}

        	if (y == height/2 && x >= width/4 && x <= 3*width/4){
        		return true;
        	}

        	if (x >= width - 12 * fruitSize && y <= fruitSize*2){
        		return true;
        	}

        	return false;
        }


        void createNewFruit(){
        	srand (now());
        	// generate x,y in a random location but so that they can't be on the borders
        	while (true){
	 			x = (1+(rand() % ((800 - 3*fruitSize)/fruitSize))) * fruitSize;
	 			y = (1+(rand() % ((600 - 3*fruitSize)/fruitSize))) * fruitSize;
	 			if (!onMiddleCross(x,y)){
	 				return;
	 			}
 			}
        }

    private:
    	int fruitSize;
        int x;
        int y;
};   

class Snake : public Displayable {
	public:
		bool invincible;
		bool turnOffInviincible;
		Snake *tail;
		virtual void paint(XInfo &xinfo) {
			XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[2], x, y, blockSize, blockSize);
			XDrawRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], x, y, blockSize, blockSize);
			if (tail != NULL){
				tail->paint(xinfo);
			}
			if (invincible){
				XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[5], 770, 5, getBlockSize(), getBlockSize());
				XDrawRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], 770, 5, getBlockSize(), getBlockSize());
			}
		}

		void moveTail(){
			x = x + direction.first;
			y = y + direction.second;
			checkEdgeCollision();
			if (tail != NULL){
				tail->moveTail();
				tail->orientation = orientation;
				tail->direction = direction;
			}
		}

		~Snake(){
			if (tail != NULL){
				delete tail;
			}
		}
		
		void move(XInfo &xinfo, Fruit &fruit) {
			// incrementPaceCount();
			// //std::cout << paceCount << " - " << pace << std::endl;
			// if (paceCount <= pace){
			// 	return;
			// }

			if (now() - speedTime < gameSpeed){
				return;
			}
			speedTime = now();
			changeDirection = true;
			iterations++;

			if (turnOffInviincible){
				invincible = false;
				turnOffInviincible = false;
			}

			eatFruit(fruit);

			paceCount = 0;
			x = x + direction.first;
			y = y + direction.second;
			checkEdgeCollision();

			if (tail != NULL){
				tail->moveTail();
				tail->orientation = orientation;
				tail->direction = direction;
			}

            // ** ADD YOUR LOGIC **
            // Here, you will be performing collision detection between the snake, 
            // the fruit, and the obstacles depending on what the snake lands on.
		}

		void checkEdgeCollision(){
			if (x >= 800){ x = 0;}
			else if (x < 0){ x = 800 - blockSize;}
			else if (y >= 600){ y = 0;}
			else if (y < 0){ y = 600;}
			else {}
		}

		bool checkTailCollision(int headX, int headY){
			if (tail != NULL){
				return (headX == tail->getX() && headY == tail->getY()) || tail->checkTailCollision(headX,headY);
			}
			return false;
		}

		bool checkMapCollision(){
			if (x == width/2 && y >= height/4 && y < 3*height/4){
        		return true;
        	}

        	if (y == height/2 && x >= width/4 && x < 3*width/4){
        		return true;
        	}

        	if ((x == 0 || x == width - blockSize) && (y < height/2 - getBlockSize() || y > height/2 + getBlockSize())) {
        		return true;
        	}

        	if ((y == 0 || y == height - blockSize) && (x < width/2 - getBlockSize() || x > width/2 + getBlockSize())){
        		return true;
        	}

        	if (x >= width - 12 * getBlockSize() && y <= getBlockSize()*2){
        		return true;
        	}

        	return false;

		}
 

		bool checkCollisionWithSelf(){
			return checkTailCollision(x,y);
		}

		void extendTail(){
			if (tail == NULL){
				Snake *tmpSnake = new Snake(*this);
				tmpSnake->x = x - direction.first;
				tmpSnake->y = y - direction.second;

				tail = tmpSnake;
			} else {
				tail->extendTail();
			}
		}
		
		int getX() {
			return x;
		}
		
		int getY() {
			return y;
		}

        /*
         * ** ADD YOUR LOGIC **
         * Use these placeholder methods as guidance for implementing the snake behaviour. 
         * You do not have to use these methods, feel free to implement your own.
         */ 
        void eatFruit(Fruit &fruit) {
        	ateFruit = fruit.getX() == x && fruit.getY() == y;
        }

        bool didEatPowerup(PowerUp &powerup){
        	return powerup.getX() == x && powerup.getY() == y && powerup.paintPowerup;
        }

        bool didEatFruit(){
        	if (ateFruit){
        		ateFruit = false;
        		return true;
        	}

        	return false;
        }

        void didHitObstacle() {
        }

        void turnLeft() {
        	if (!changeDirection){ return; }
        	changeDirection = false;
        	if (orientation == RIGHT) {
        		return;
        	}

        	direction.first = -speed;
        	direction.second = 0;

        	orientation = LEFT;
        	
        }

        void turnRight() {
        	if (!changeDirection){ return; }
        	changeDirection = false;
        	if (orientation == LEFT) {
        		return;
        	}

        	direction.first = speed;
        	direction.second = 0;

        	orientation = RIGHT;

        }

        void turnUp(){
        	if (!changeDirection){ return; }
        	changeDirection = false;
        	if (orientation == DOWN) {
        		return;
        	}

        	direction.first = 0;
        	direction.second = -speed;

        	orientation = UP;
        }
		
        void turnDown(){
        	if (!changeDirection){ return; }
        	changeDirection = false;
        	if (orientation == UP) {
        		return;
        	}

        	direction.first = 0;
        	direction.second = speed;

        	orientation = DOWN;
        }

		Snake(int x, int y): x(x), y(y), orientation(RIGHT), paceCount(0), tail(NULL) {
			invincible = false;
			turnOffInviincible = false;
			changeDirection = true;
			blockSize = 10;
			speed = blockSize;
			direction = std::make_pair(speed,0);
			
		}

		
		void incrementPaceCount(){
			paceCount++;
		}

		int getBlockSize(){
			return blockSize;
		}

		void setPace(int p){
			pace = p;
		}
	
	private:
		bool changeDirection;
		int x;
		int y;
		bool ateFruit;
		double speed;
		int blockSize;
		enum Direction {UP, DOWN, LEFT, RIGHT};
		int paceCount;
		Direction orientation;
		std::pair <double, double> direction; // represents x,y movement
};
bool gameInstance = false;
bool paused = true;
int score = 0;

class Board : public Displayable {
	public:
		Board(Fruit &fruit, Snake &snake, PowerUp &powerup): fruit(fruit), snake(snake), powerup(powerup){
			displayedPowerup = false;
			initBoard();
		};

		virtual void paint(XInfo &xinfo) {
			for (int i=0; i<map.size(); i++){
				std::pair<int,int> tmp = map.at(i);
				XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], tmp.first, tmp.second, snake.getBlockSize(), snake.getBlockSize());
				XDrawRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], tmp.first, tmp.second, snake.getBlockSize(), snake.getBlockSize());
			}
			powerup.paint(xinfo);
			const char *scoreText = "Score: ";
			XDrawString(xinfo.display, xinfo.pixmap, xinfo.gc[1], 695, 15, scoreText, 7);
			std::ostringstream oss;
			oss << score;
			int length = oss.str().length();
			const char *scoreNum = oss.str().c_str();
			XDrawString(xinfo.display, xinfo.pixmap, xinfo.gc[1], 735, 15, scoreNum, length);

        }

        void endGame(){
        	snake.invincible = false;
        	displayedPowerup = false;
        	powerup.paintPowerup = false;
        	gameInstance = false;
        }

		void moveSnake(XInfo &xinfo){
			snake.move(xinfo, fruit);
			bool collision;
			collision = snake.checkCollisionWithSelf() || snake.checkMapCollision();
			if (collision){
				if (snake.invincible){
					snake.turnOffInviincible = true;
					return;
				}
				endGame();
				return;
			}
			checkCollision(snake);

			if (snake.didEatPowerup(powerup) || fruitExpire){
				if (snake.didEatPowerup(powerup)){
					snake.invincible = true;
				}
				powerup.createNewPowerUp();
				while (powerup.getX() == fruit.getX() && powerup.getY() == fruit.getY()){
					powerup.createNewPowerUp();
				}
				powerup.paintPowerup = false;
			}

			if (snake.didEatFruit()){
				++score;
				createNewFruit();
				while (powerup.getX() == fruit.getX() && powerup.getY() == fruit.getY()){
					createNewFruit();
				}
				createNewFruit();
				snake.extendTail();
			}

			if ((score+1) % 11 == 0 && !displayedPowerup){
				if (!powerup.paintPowerup){
					iterations = 0;
					displayedPowerup = true;
					powerup.paintPowerup = true;
				}
			} else if ((score+1) % 11 != 0){
				displayedPowerup = false;
			}

			if (powerup.paintPowerup){
				fruitExpire = false;
				if (iterations > 100){
					iterations = 0;
					fruitExpire = true;
				}
			}
		}

		void addBorders(){
			for (int i=0; i<width; i+=snake.getBlockSize()){
				if (i < width/2 - snake.getBlockSize() || i > width/2 + snake.getBlockSize()){
					std::pair<int,int> tmp;
					if (i >= width - 12 * snake.getBlockSize()){
						tmp = make_pair(i, snake.getBlockSize()*2);	
					} else {
						tmp = make_pair(i,0);
					}
					
					std::pair<int,int> tmp1 = make_pair(i,height - snake.getBlockSize());
					map.push_back(tmp);
					map.push_back(tmp1);		
				}	
			}
			for (int i=0; i<height; i+=snake.getBlockSize()){
				if (i < height/2 - snake.getBlockSize() || i > height/2 + snake.getBlockSize()){
					std::pair<int,int> tmp = make_pair(0,i);
					std::pair<int,int> tmp1;
					if (i < snake.getBlockSize()*2){
						tmp1 = make_pair(width - snake.getBlockSize()*12,i);
					} else {
						tmp1 = make_pair(width - snake.getBlockSize(),i);
					}
					map.push_back(tmp);		
					map.push_back(tmp1);			
				}
			}
		}

		void addCenter(){
			for (int i=width/4; i<(3*width)/4; i+=snake.getBlockSize()){
				std::pair<int,int> tmp = make_pair(i,height/2);
				map.push_back(tmp);	
			}
			for (int i=height/4; i<(3*height)/4; i+=snake.getBlockSize()){
				std::pair<int,int> tmp = make_pair(width/2,i);
				map.push_back(tmp);	
			}
		}

		void initBoard(){
			addBorders();
			addCenter();
		}

		void checkCollision(Snake &snake){

		}

		void createNewFruit(){
			fruit.createNewFruit();
		}

		
	private:
		std::vector<std::pair<int,int> > map;
		bool displayedPowerup;
		PowerUp &powerup;
		Snake &snake;
		Fruit &fruit;
};

list<Displayable *> dList;           // list of Displayables
Snake snake(100, 450);
Fruit fruit(10);
PowerUp powerup(10);
Board board(fruit, snake, powerup);
Pixmap background;

void setBackground(XInfo &xinfo){
	XFillRectangle(xinfo.display, background, xinfo.gc[1], 
	 	0, 0, xinfo.width, xinfo.height);
}

void startController() {
	if (!gameInstance){
		delete snake.tail;
		snake = Snake(100, 450);
		paused = true;
		score = 0;
	}
	gameInstance = true;
}

void restartGame(){
	score = 0;
	paused = true;
	delete snake.tail;
	snake = Snake(100, 450);
}


void initSplashScreen(XInfo &xinfo){
	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], 
	  	0, 0, xinfo.width, xinfo.height);
	const char *title = "Welcome to Snake";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[4], 315, 200, title, 16);
	const char *myName = "Milosh Zelembaba";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 10, 20, myName, 16);
	const char *myEmail = "mzelemba@uwaterloo.ca";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 10, 35, myEmail, 21);
	const char *myId = "20579583";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 10, 52, myId, 8);


	const char *toPlay = "To Play:";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 10, 350, toPlay, 8);
	const char *instructions = "Use the arrow keys to avoid the barriers, eat the fruit, and survive as long as you can!";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 15, 370, instructions, 88);

	const char *keys = "P - Play/Pause            Q - Quit            R - Restart ";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 235, 500, keys, 58);

	const char *you = "- This is you";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 75, 410, you, 13);
	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[2], 
	  	55, 400, 10, 10);
	XDrawRectangle(xinfo.display, xinfo.window, xinfo.gc[1], 
	  	55, 400, 10, 10);
	const char *fruitText = "- This is the fruit you are hungry for";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 75, 425, fruitText, 38);
	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[3], 
	  	55, 415, 10, 10);
	XDrawRectangle(xinfo.display, xinfo.window, xinfo.gc[1], 
	  	55, 415, 10, 10);
	const char *powerupText = "- This powerup will let you pass through a WALL (not boarder) or yourself ONCE";
	XDrawString(xinfo.display, xinfo.window, xinfo.gc[1], 75, 440, powerupText, 78);
	XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[5], 
	  	55, 430, 10, 10);
	XDrawRectangle(xinfo.display, xinfo.window, xinfo.gc[1], 
	  	55, 430, 10, 10);


	

	XFlush( xinfo.display );
	
}

/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"animation",		// window's title
		"Animate",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);
	XSetGraphicsExposures(xInfo.display, xInfo.gc[i],0);

	i = 1;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                      1, LineSolid, CapButt, JoinRound);
	XSetGraphicsExposures(xInfo.display, xInfo.gc[i],0);

	XColor color;
  	Colormap colormap;
  	char green[] = "#1e6829";

	colormap = DefaultColormap(xInfo.display, 0);
	XParseColor(xInfo.display, colormap, green, &color);
	XAllocColor(xInfo.display, colormap, &color);
	i = 2;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], color.pixel);
	XSetGraphicsExposures(xInfo.display, xInfo.gc[i],0);

	char purp[]  = "#FF00AA";

	colormap = DefaultColormap(xInfo.display, 0);
	XParseColor(xInfo.display, colormap, purp, &color);
	XAllocColor(xInfo.display, colormap, &color);

	i = 3;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], color.pixel);
	XSetGraphicsExposures(xInfo.display, xInfo.gc[i],0);

	i = 4;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	Font font;
	font = XLoadFont(xInfo.display, "*x20");
	XSetFont(xInfo.display, xInfo.gc[4], font);
	XSetGraphicsExposures(xInfo.display, xInfo.gc[i],0);

	i = 5;
	char realPurp[]  = "#00FFFF";

	colormap = DefaultColormap(xInfo.display, 0);
	XParseColor(xInfo.display, colormap, realPurp, &color);
	XAllocColor(xInfo.display, colormap, &color);
	
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], color.pixel);
	XSetGraphicsExposures(xInfo.display, xInfo.gc[i],0);



	int depth = DefaultDepth(xInfo.display, DefaultScreen(xInfo.display));
	xInfo.pixmap = XCreatePixmap(xInfo.display, xInfo.window, hints.width, hints.height, depth);
	xInfo.width = hints.width;
	xInfo.height = hints.height;


	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | 
		EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask);  // for resize events

	//setBackground(xInfo);

	XSetWindowBackgroundPixmap(xInfo.display, xInfo.window, None);

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	XClearWindow( xinfo.display, xinfo.window );
	
	// get height and width of window (might have changed since last repaint)

	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	// big black rectangle to clear background

	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 
	 	0, 0, xinfo.width, xinfo.height);

	// std::cout << "about to paste background" << std::endl;
	// XCopyArea(xinfo.display, background, xinfo.window, xinfo.gc[0], 
	// 	0, 0, xinfo.width, xinfo.height,  // region of pixmap to copy
	// 	0, 0); // position to put top left corner of pixmap in window
	// std::cout << "pasted background" << std::endl;
    
	// draw display list
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}

	// copy buffer to window
	XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc[0], 
		0, 0, xinfo.width, xinfo.height,  // region of pixmap to copy
		0, 0); // position to put top left corner of pixmap in window
	XFlush( xinfo.display );
}

void paint(XInfo &xinfo){
	if (gameInstance){
		repaint(xinfo);
	} else {
		initSplashScreen(xinfo);
	}
}

void controlPause(){
	paused = !paused;
}

void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		} else if (text[0] == 'p'){
			startController();
			controlPause();
		} else if (text[0] == 'r'){
			restartGame();
		}

	} 
	 if (!paused){
		switch (key){
			case XK_Left:
				snake.turnLeft();
				break;
			case XK_Right:
				snake.turnRight();
				break;
			case XK_Up:
				snake.turnUp();
				break;
			case XK_Down:
				snake.turnDown();
				break;
		}
	}
}

void handleAnimation(XInfo &xinfo, int inside) {
	if (gameInstance && !paused){
		board.moveSnake(xinfo);
	}
}

unsigned long paintTime = now();

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&snake);
    dList.push_front(&fruit);
    dList.push_front(&board);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		/*
		 * This is NOT a performant event loop!  
		 * It needs help!
		 */
		
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			switch( event.type ) {
				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		} 

		//usleep(1000000/FPS);
		if (now() - paintTime > 1000000/FPS){
			paintTime = now();	
			paint(xinfo);	
		}
		handleAnimation(xinfo, inside);	
	}
}

/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;
	// 50,000   ---   200,000
	speedTime = now();
	if (argc == 3){
		if (atoi(argv[1]) >= 1 && atoi(argv[1]) <= 100){
			FPS = atoi(argv[1]);
		}	
		if (atoi(argv[2]) >= 1 && atoi(argv[2]) <= 10){
			gameSpeed = (30000 + (10 - atoi(argv[2])) * 12000);
		}
	}

	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
