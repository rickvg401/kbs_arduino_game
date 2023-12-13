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
coins = []
ghosts = []
players = []

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

	for coin in coins:
		pygame.draw.rect(win, GREEN, (coin[0]*BLOCKSIZE, coin[1]*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE))

	for ghost in ghosts:
		pygame.draw.rect(win, RED, (ghost[0]*BLOCKSIZE, ghost[1]*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE))

	for player in players:
		pygame.draw.rect(win, BLUE, (player[0]*BLOCKSIZE, player[1]*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE))

def exportGrid():
	print(coins)



	with open(ofname, 'w') as f:
		f.write("const uint8_t COINS_LENGTH  = "+str(len(coins))+";\n")
		f.write("const uint8_t GHOSTS_LENGTH = "+str(len(ghosts))+";\n")
		f.write("const uint8_t PLAYER_LENGTH = "+str(len(playerCheck))+";\n")

		f.write("uint8_t coins["+str(len(coins))+"][2] = {\n")
		for coin in coins:
			f.write("{"+str(coin[0])+","+str(coin[1])+"},\n")
		f.write("};\n")

		f.write("uint8_t players["+str(len(players))+"][2] = {\n")
		for player in players:
			f.write("{"+str(player[0])+","+str(player[1])+"},\n")

		f.write("};\n")

		f.write("uint8_t ghosts["+str(len(ghosts))+"][2] = {\n")
		for ghost in ghosts:
			f.write("{"+str(ghost[0])+","+str(ghost[1])+"},\n")

		f.write("};\n")

		
		f.write("uint8_t field[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION] = {\n")
		# export grid
		for y, row in enumerate(grid):
			f.write("{")
			for col in range(GRID_WIDTH//8):
				byteGroup = [str(grid[y][x]) for x in range(col*8, col*8+8)]
				_hex = "0x%0.2X" % int(''.join(byteGroup), 2)
				f.write(f"{_hex},")
			f.write("},\n")
		f.write("};")

if __name__ == "__main__":
	running = True
	while running:
		draw = False
		erase = False
		keys = pygame.mouse.get_pressed()
		x, y = pygame.mouse.get_pos()
		for event in pygame.event.get():
			
			if event.type == pygame.QUIT:
				running = False
			elif event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					running = False
				elif event.key == pygame.K_e:
					exportGrid()
				
				
				elif event.key == pygame.K_c:
					coin = [x//BLOCKSIZE,y//BLOCKSIZE]
					if(coin not in coins):
						coins.append(coin)
				elif event.key == pygame.K_x:
					coinCheck = [x//BLOCKSIZE,y//BLOCKSIZE]
					for coin in coins:
						if(coin[0] == coinCheck[0]  and coin[1] == coinCheck[1]):
							coins.remove(coin)
				
				elif event.key == pygame.K_g:
					ghost = [x//BLOCKSIZE,y//BLOCKSIZE]
					if(ghost not in ghosts):
						ghosts.append(ghost)
				elif event.key == pygame.K_h:
					ghostCheck = [x//BLOCKSIZE,y//BLOCKSIZE]
					for ghost in ghosts:
						if(ghost[0] == ghostCheck[0]  and ghost[1] == ghostCheck[1]):
							ghosts.remove(ghost)		

				elif event.key == pygame.K_p:
					player = [x//BLOCKSIZE,y//BLOCKSIZE]
					if(player not in players):
						players.append(player)
				elif event.key == pygame.K_l:
					playerCheck = [x//BLOCKSIZE,y//BLOCKSIZE]
					for player in players:
						if(player[0] == playerCheck[0]  and player[1] == playerCheck[1]):
							players.remove(player)		
		try:
			# keys = pygame.mouse.get_pressed()
			# x, y = pygame.mouse.get_pos()
			if keys[0]:
				grid[y//BLOCKSIZE][x//BLOCKSIZE] = 1
				print(grid)
			elif keys[2]:
				grid[y//BLOCKSIZE][x//BLOCKSIZE] = 0
				print(grid)

			
			
		except IndexError:
			pass


		clock.tick(FPS)
		pygame.display.update()
		drawGrid()
