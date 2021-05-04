#include "Includes.h"
#define NUMCOLOR		13

typedef struct{
	uint **num;
	uint len;
	uint scale;
}Grid;

Color gridColor(const uint n)
{
	const static Color colors[NUMCOLOR] = {
		(Color){205, 193, 180, 0xFF}, // None
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
	};
	return colors[clamp(n,0,NUMCOLOR)];
}

void freeGrid(const Grid grid)
{
	for(uint i = 0; i < grid.len; i++)
		free(grid.num[i]);
	free(grid.num);
}

void zeroGrid(Grid grid)
{
	for(uint i = 0; i < grid.len; i++)
		memset(grid.num[i], 0, sizeof(uint));
}

Grid newGrid(const uint len, const uint scale)
{
	Grid ret = {
		.num = malloc(sizeof(uint*)*len),
		.len = len,
		.scale = scale
	};
	for(uint i = 0; i < len; i++)
		ret.num[i] = malloc(sizeof(uint)*len);
	zeroGrid(ret);
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

uint gridLabel(const uint num)
{
	if(num == 0)
		return 0;
	return (uint)pow(2, num);
}

void drawGrid(const Grid grid)
{
	for(uint y = 0; y < grid.len; y++){
		for(uint x = 0; x < grid.len; x++){
			setColor(GREY);
			fillSquareCoordResize(coordMul((Coord){x, y}, grid.scale), grid.scale, -2);
			setColor(gridColor(grid.num[x][y]));
			fillSquareCoordResize(coordMul((Coord){x, y}, grid.scale), grid.scale, -5);
			char buf[64] = {0};
			if(grid.num[x][y]>0)
				sprintf(buf, "%u", gridLabel(grid.num[x][y]));
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

uint gridNumEmpty(const Grid grid)
{
	uint ret = 0;
	for(uint y = 0; y < grid.len; y++){
		for(uint x = 0; x < grid.len; x++){
			ret += grid.num[x][y] == 0;
		}
	}
	return ret;
}

void place(Grid grid, const uint pos)
{
	uint current = 0;
	for(uint y = 0; y < grid.len; y++){
		for(uint x = 0; x < grid.len; x++){
			if(current == pos && grid.num[x][y] == 0){
				grid.num[x][y] = rand()%3==2?2:1;
				return;
			}
			current += grid.num[x][y] == 0;
		}
	}
}

bool shiftGridDir(Grid grid, const bool combine, const Direction dir)
{
	bool shiftedouter = false;
	int x;
	int y;
	int mx = 0;
	int my = 0;
	if(dirUD(dir)){
		my = dirPOS(dir) ? 1 : -1;
	}else{
		mx = dirPOS(dir) ? 1 : -1;
	}
	for(int j = 0; j < grid.len; j++){
		bool shiftedinner = false;
		do{
			for(
				int k=dirPOS(dir) ? grid.len-2 : 1;
				dirPOS(dir) ? k>=0 : k < grid.len;
				k+=dirPOS(dir)?-1:1
			){
				if(dirUD(dir)){
					x = j;
					y = k;
				}else{
					x = k;
					y = j;
				}
				if(combine){
					if(
						grid.num[x+mx][y+my] > 0 &&
						grid.num[x+mx][y+my] == grid.num[x][y]
					){
						grid.num[x+mx][y+my] += 1;
						grid.num[x][y] = 0;
						shiftedouter |= true;
					}
				}else{
					if(
						grid.num[x+mx][y+my] == 0 &&
						grid.num[x][y] > 0
					){
						grid.num[x+mx][y+my] = grid.num[x][y];
						grid.num[x][y]=0;
						shiftedinner = true;
						shiftedouter|=true;
					}else{shiftedinner = false;}
				}
			}
		}while(!combine && shiftedinner);
	}
	return shiftedouter;
}

bool shiftGrid(Grid grid, const Direction dir)
{
	bool ret = false;
	for(uint i = 0; i < 3; i++)
		ret|=shiftGridDir(grid, i&1, dir);
	return ret;
}

int main(int argc, char const *argv[])
{
	Grid grid = newGrid(4, 200);
	place(grid, rand()%grid.len*grid.len);
	place(grid, rand()%grid.len*grid.len-1);
	const Length window = coordMul((const Coord){grid.len, grid.len}, grid.scale);
	init(window);


	while(1){
		Ticks frameStart = getTicks();
		clear();

		for(uint i = 0; i < 4; i++){
			if(moveDir(i)){
				const bool shifted = shiftGrid(grid, i);
				const uint empty = gridNumEmpty(grid);
				if(empty <= 1){
					printf("You lose!\n");
					freeGrid(grid);
					exit(0);
				}else if(shifted){
					place(grid, rand()%empty);
				}
				break;
			}
		}

		drawGrid(grid);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
