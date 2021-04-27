#include "Includes.h"
#define NUMCOLOR		12

typedef struct{
	uint **num;
	uint **flipped;
	uint len;
	uint scale;
	Color colors[NUMCOLOR];
}Grid;

void freeGrid(const Grid grid)
{
	if(grid.num == NULL)
		return;
	for(uint i = 0; i < grid.len; i++){
		if(grid.num[i] != NULL)
			free(grid.num[i]);
	}
	free(grid.num);
}

Grid newGrid(const uint len, const uint scale)
{
	Grid ret = {
		.num = malloc(sizeof(uint*)*len),
		.flipped = malloc(sizeof(uint*)*len),
		.len = len,
		.scale = scale,
		.colors = {
			(Color){238, 228, 218, 0xFF},	// 2
			(Color){237, 224, 200, 0xFF},	// 4
			(Color){242, 177, 121, 0xFF},	// 8
			(Color){245, 149,  99, 0xFF},	// 16
			(Color){246, 124,  95, 0xFF},	// 32
			(Color){246,  94,  59, 0xFF},	// 64
			(Color){237, 207, 114, 0xFF},	// 128
			(Color){237, 204,  97, 0xFF},	// 256
			(Color){237, 200,  80, 0xFF},	// 512
			(Color){237, 197,  63, 0xFF},	// 1024
			(Color){237, 194,  46, 0xFF},	// 2048
			WHITE					// Super
		}
	};
	for(uint i = 0; i < len; i++){
		ret.num[i] = malloc(sizeof(uint)*len);
		ret.flipped[i] = malloc(sizeof(uint)*len);
	}
	return ret;
}

Grid resizeGrid(const Grid grid, uint len)
{
	freeGrid(grid);
	return newGrid(len, grid.scale);
}

void fillSquareCoordResize(const Coord pos, const uint len, const int resize)
{
	SDL_RenderFillRect(
		gfx.renderer,
		&(const Rect){pos.x-resize, pos.y-resize, len+resize*2, len+resize*2}
	);
}

uint gridnum(const uint num)
{
	uint ret = 1;
	for(uint i = 0; i < num; i++)
		ret *= 2;
	return ret;
}

void drawGrid(const Grid grid, const bool b)
{
	for(uint y = 0; y < grid.len; y++){
		for(uint x = 0; x < grid.len; x++){
			setColor(GREY);
			fillSquareCoordResize(coordMul((Coord){x, y}, grid.scale), grid.scale, -2);
			setColor(grid.colors[grid.num[x][y]]);
			fillSquareCoordResize(coordMul((Coord){x, y}, grid.scale), grid.scale, -5);
			const uint gnum = gridnum(grid.num[x][y]);
			char buf[64] = {0};
			sprintf(buf, "%u", gnum);
			// printf("gnum: %u, buf: %s\n", gnum, buf);
			setFontSize(grid.scale/6);
			setFontColor(BLACK);
			drawTextCenteredCoord(coordAdd(coordMul((Coord){x, y}, grid.scale), grid.scale/2), buf);
		}
	}
}

bool moveDir(const Direction dir)
{
	const static SDL_Scancode  wdsa[4] = {
		SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A
	};
	const static SDL_Scancode  urdl[4] = {
		SDL_SCANCODE_UP, SDL_SCANCODE_RIGHT, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT
	};
	return (
		(keyPressed(wdsa[dir])||keyPressed(urdl[dir])) &&
		!(keyPressed(wdsa[dirINV(dir)])||keyPressed(urdl[dirINV(dir)]))
	);
}

void getFlipped(uint **num, uint **flipped, const uint len)
{

	for(uint x = 0; x < len; x++){
		for(uint y = 0; y < len; y++){
			flipped[len-y][x] = num[x][y];
		}
	}
}

int main(int argc, char const *argv[])
{
	Grid grid = newGrid(4, 200);
	for(uint y = 0; y < grid.len; y++){
		for(uint x = 0; x < grid.len; x++){
			grid.num[x][y] = (rand()%(NUMCOLOR*2))/2;
			printf("%4u ", grid.num[x][y]);
		}
		printf("\n");
	}
	getFlipped(grid.num, grid.flipped, grid.len);

	const Length window = coordMul((const Coord){grid.len, grid.len}, grid.scale);
	init(window);

	bool b = false;
	while(1){
		Ticks frameStart = getTicks();
		clear();

		for(uint i = 0; i < 4; i++){
			if(moveDir(i)){
				// shiftGrid(grid, i);
				printf("Shift %c\n", DirectionChar[i]);
				break;
			}
		}
		if(keyPressed(SDL_SCANCODE_SPACE)){
			b = !b;
		}

		drawGrid(grid, b);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}






/*
Down
2--   2--   2--  4-   #
-2-   24-   22-  24   #
2-2   2-4   -22  24   #
244   244   224  44   #

  2
  -
  2
  2
*/

// void reverseLine(uint *line)
// {
// 	uint new[4];
// 	for(uint i = 0; i < 4; i++)
// 		new[3-i]=line[i];
// 	memcpy(line, new, sizeof(uint)*4);
// }
//
// void shift(uint *line, const uint len)
// {
// 	bool match = false;
// 	do{
// 		for(int i = len-2; i >= 0; i--){
// 			if(line[i]==0 && line[i+1] != 0){
// 				match|=true;
// 				line[i]=line[i+1];
// 				line[i+1]=0;
// 			}
// 		}
// 	}while(match);
// }
//
// void shiftLine(uint *line, const Direction dir, const uint len)
// {
// 	if(dirPOS(dir))
// 		reverseLine(line);
// 	shift(line);
// 	if(dirPOS(dir))
// 		reverseLine(line);
// }
//
// Grid shiftGrid(const Grid grid, const Direction dir)
// {
// 	uint lines[4];
// 	for(uint i = 0; i < 4; i++){
// 		for(uint j = 0; j < 4; j++){
// 			lines[i][j] = grid.num[dirUD(dir)?i:j][dirUD(dir)?j:i];
// 		}
// 	}
//
//
// }
