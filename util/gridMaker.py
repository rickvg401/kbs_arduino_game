# Maak muur met linker muis klik
# Verwijder muur met rechter muis klik
# Klik op 'e' om grid in goed format naar grid.txt te schrijven

import pygame

ofname = "grid.txt"

GRID_WIDTH = 32
GRID_HEIGHT = 24
BLOCKSIZE = 30

win = pygame.display.set_mode((GRID_WIDTH* BLOCKSIZE, GRID_HEIGHT* BLOCKSIZE))
clock = pygame.time.Clock()
FPS = 60

grid = [[0 for _ in range(GRID_WIDTH)] for _ in range(GRID_HEIGHT)]

BLACK = (0,0,0)
RED = (255,0,0)
GREEN = (0,255,0)
BLUE = (0,0,255)
WHITE = (255,255,255)

def drawGrid():
	win.fill(WHITE)
	for y, row in enumerate(grid):
		for x, cell in enumerate(row):
			pygame.draw.rect(win, BLACK, (x*BLOCKSIZE, y*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE), 1)
			if cell == 1:
				pygame.draw.rect(win, BLACK, (x*BLOCKSIZE, y*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE))

def exportGrid():
	with open(ofname, 'w') as f:
		for y, row in enumerate(grid):
			f.write("{")
			for col in range(GRID_WIDTH//8):
				byteGroup = [str(grid[y][x]) for x in range(col*8, col*8+8)]
				_hex = "0x%0.2X" % int(''.join(byteGroup), 2)
				f.write(f"{_hex},")
			f.write("},\n")


if __name__ == "__main__":
	running = True
	while running:
		draw = False
		erase = False
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				running = False
			elif event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					running = False
				elif event.key == pygame.K_e:
					exportGrid()
		try:
			keys = pygame.mouse.get_pressed()
			x, y = pygame.mouse.get_pos()
			if keys[0]:
				grid[y//BLOCKSIZE][x//BLOCKSIZE] = 1
			elif keys[2]:
				grid[y//BLOCKSIZE][x//BLOCKSIZE] = 0
		except IndexError:
			pass


		clock.tick(FPS)
		pygame.display.update()
		drawGrid()
