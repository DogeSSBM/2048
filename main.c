#include "Includes.h"

#define NUMCOLOR		12

const Color colors[NUMCOLOR] = {
	(Color){0xef, 0xe3, 0xd6, 0xff},
	(Color){0xef, 0xdf, 0xc6, 0xff},
	(Color){0xf7, 0xb2, 0x7b, 0xff},
	(Color){0xf7, 0x96, 0x63, 0xff},
	(Color){0xf7, 0x7d, 0x63, 0xff},
	(Color){0xf7, 0x5d, 0x3a, 0xff},
	(Color){0xee, 0xce, 0x73, 0xff},
	(Color){0xee, 0xce, 0x63, 0xff},
	(Color){0xe7, 0xc6, 0x4a, 0xff},
	(Color){0xec, 0xc4, 0x40, 0xff},
	(Color){0xef, 0xca, 0x4d, 0xff},
	(Color){0xff, 0x3d, 0x3a, 0xff}
};

typedef struct{
	uint **num;
	Length len;
	uint scale;
}Grid;

void freeGrid(const Grid grid)
{
	if(grid.num == NULL)
		return;
	for(uint i = 0; i < grid.len.x; i++){
		if(grid[i] != NULL)
			free(grid.num[i]);
	}
	free(grid.num);
}

Grid newGrid(const Length len, const uint scale)
{
	Grid ret = {
		.num = malloc(sizeof(uint*)*len.x),
		.len = len,
		.scale = scale
	};
	for(uint i = 0; i < len.x; i++){
		ret.[i] = malloc(sizeof(uint)*len.y);
	}
	return ret;
}

Grid resizeGrid(const Grid grid, Length len)
{
	freeGrid(grid);
	return newGrid(len, grid.scale);
}

void drawGrid(const Grid grid)
{
	uint i = 0;
	for(uint y = 0; y < grid.len.y; y++){
		for(uint x = 0; x < grid.len.x; x++){
			setColor(colors[i]);
			i = wrap(i+1, 0, NUMCOLOR)
			fillSquareCoord()
		}
	}
}

int main(int argc, char const *argv[])
{
	Grid grid = newGrid((Length){4, 4}, 200);
	const Length window = coordMul(grid.len, grid.scale);
	init(window);

	while(1){
		Ticks frameStart = getTicks();
		clear();



		draw();
		events(frameStart + TPF);
	}
	return 0;
}
