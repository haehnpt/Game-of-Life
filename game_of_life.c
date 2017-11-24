#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> //Windows specific

#define XDIM 32
#define YDIM 32

#define DEAD 0
#define ALIVE 1

#define HISTORY_SIZE 32
#define SIM_SPEED 100

/*
//
// Prefdefinitions
//
*/
void init_cells(int*);
void display_cells(const int*);
int alive_neighbouring_cells(const int*, int, int);
int alive_cells(const int*);
void copy(int*,const int*);
void shift_history(int*);
void evolution_step(int*);
void display_stats(const int*, int, int);
int equal(const int*, const int*);
int oscillating(const int*, int*);

/*
//
// Main function
//
*/
int main(int argc, char* argv[])
{
	int cells[HISTORY_SIZE][XDIM][YDIM];
	int gen = 1;
	int osc = 0;
	srand(time(NULL));
	init_cells((int*) cells);
	while (alive_cells((int*)cells))
	{
		system("cls"); //Windows specific
		display_cells((int*) cells);
		evolution_step((int*) cells);
		if (osc == 0)
			oscillating((int*)cells, &osc);
		display_stats((int*) cells, gen, osc);
		if (osc == 0)
			gen++;
		Sleep(SIM_SPEED); //Windows specific
	}
	system("cls"); //Windows specific
	display_cells((int*) cells);
	display_stats((int*) cells, gen, osc);
}

/*
//
// Initialize cells to either DEAD (0) or ALIVE (1) by chance
//
*/
void init_cells(int* cells)
{
	for (int y = 0; y < YDIM; y++)
		for (int x = 0; x < XDIM; x++)
			*(cells + y * XDIM + x) = rand()%2;
}

/*
//
// Display all cells in a visually pleasant way
//
*/
void display_cells(const int* cells)
{
	char line[10000];
	int c = 0;
	for (int y = -1; y <= YDIM; y++)
	{
		line[c] = '|';
		c++;
		for (int x = 0; x < XDIM; x++)
		{
			if (y == -1 || y == YDIM)
				line[c] = '-';
			else
			{
				if (*(cells + y * XDIM + x) == ALIVE)
					line[c] = 'O';
				else
					line[c] = ' ';
			}
			c++;
		}
		line[c] = '|';
		c++;
		line[c] = '\n';
		c++;
		line[c] = '\0';
	}	
	printf(line);
}
/*
//
// Count all alive and neighbouring cells
//
*/
int alive_neighbouring_cells(const int* cells, int x, int y)
{
	int count = 0, tx = 0, ty = 0;
	for (int b = -1; b <= 1; b++)
		for (int a = -1; a <= 1; a++)
		{
			tx = x+a < XDIM ? (x+a < 0 ? XDIM-1 : x+a): 0;
			ty = y+b < YDIM ? (y+b < 0 ? YDIM-1 : y+b): 0;
			if (!(a == 0 && b == 0))
				count += *(cells + ty * XDIM + tx);
		}
	return count;
}

/*
//
// Count all alive cells
//
*/
int alive_cells(const int* cells)
{
	int count = 0;
	for (int y = 0; y < YDIM; y++)
		for (int x = 0; x < XDIM; x++)
			count += *(cells + y * XDIM + x);
	return count;
}

/*
//
// Copy an array of cells to another array
//
*/
void copy(int* cells_to, const int* cells_from)
{
	for (int y = 0; y < YDIM; y++)
		for (int x = 0; x < XDIM; x++)
			*(cells_to + y * XDIM + x) = *(cells_from + y * XDIM + x);
}

/*
//
// Shift the history of cells one step forward
//
*/
void shift_history(int* cells)
{
	for (int a = HISTORY_SIZE - 1; a > 0; a--)
		copy(cells + a * (XDIM * YDIM), cells + (a-1) * (XDIM * YDIM));
}

/*
//
// Evaluate, whether cells remain alive, die or are reborn
//
*/
void evolution_step(int* cells)
{
	shift_history(cells);
	for (int y = 0; y < YDIM; y++)
		for (int x = 0; x < XDIM; x++)
		{
			switch (alive_neighbouring_cells(cells + XDIM * YDIM, x, y))
			{
				case 2:
					*(cells + y * XDIM + x) = *(cells + XDIM * YDIM + y * XDIM + x);
					break;
				case 3:
					*(cells + y * XDIM + x) = ALIVE;
					break;
				default:
					*(cells + y * XDIM + x) = DEAD;
			}
		}
}

/*
//
// Display statistics about the history of population development
//
*/
void display_stats(const int* cells, int gen, int osc)
{
	printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\nStats: Generation %6d\n", gen);
	printf("\nNumber of Living Cells:       %05d / %05d === %5.2f%c", alive_cells(cells), XDIM * YDIM, alive_cells(cells) / (float)(XDIM * YDIM) * 100, 37);
	printf("\nNumber of Dead Cells:         %05d / %05d === %5.2f%c", XDIM * YDIM - alive_cells(cells), XDIM * YDIM, (XDIM * YDIM - alive_cells(cells)) / (float)(XDIM * YDIM) * 100, 37);
	printf("\nChange to last Generation:     %4d cells", alive_cells(cells) - alive_cells(cells + XDIM * YDIM));
	printf("\nStable state / Oscillating? : %s (%d steps)", (osc == 0 ? "NO" : "YES"), osc);
	printf("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

/*
//
// Check, whether states are equal
//
*/
int equals(const int* cells_a, const int* cells_b)
{
	for (int y = 0; y < YDIM; y++)
		for (int x = 0; x < XDIM; x++)
			if (*(cells_a + y * XDIM + x) != *(cells_b + y * XDIM + x))
				return 0;
	return 1;
}

/*
//
// Check, whether a state is stable / oscillating
//
*/
int oscillating(const int* cells, int* osc)
{
	for (int a = 1; a < HISTORY_SIZE; a++)
		if (equals(cells, cells + a * XDIM * YDIM))
		{
			*osc = a;
			return 1;
		}
	return 0;
}















