README doc for Milosh Zelembaba's Snake (CS349 Assignment 1)
mzelemba@uwaterloo.ca
20579583


~~~~   Development Env:
* macOS Sierra version 10.12.2
* java version: 1.8.0_112


~~~~   Running the game:
For default arguments: make run
else do: make
	 ./snake FPS SPEED      (where 1 <= FPS <= 100   &&   1 <= SPEED <= 10)
otherwise default arugments will be used for invalid entries

~~~~   Playing the game:
To play the game use the arrow keys to change the direction of the snake (the green square, thats you).
Each fruit (the pink block) will increase your score by 1 and also increase the size of your tail by 1.
The goal of the game is to collect as many fruit as possible without dying. Dying includes running into a barrier
or running into your own tail. For every 10 fruit that you collect, a powerup will appear (the blue fruit) and you have
100 iterations (movements, to account for different speeds) to reach the fruit which generally means you should
go straight for the fruit once it appears if you wish to get it before it disapears. When you collect the fruit
you will see it in the top right corner of the screen to let you know when you have the powerup


~~~~   Enhancements:
* Added an invicibility powerup that lets you pass through a wall or yourself (NOT an outside edge)
  Disappears after a certain amount of time thats dependent on your current speed
* Map design, certain areas allow for map wrap around and there are obsticles in the center of the map aswell


~~~~   Overall Design:
We have a Board object that controls game play and the objects we see on the screen. So it tells a Snake object to move
as well as telling when the Fruit object or Powerup object to generate new versions of themselves.
This overall game play is controlled by an event loop where frame rate is controlled as well as game flow (different from
game play). 
