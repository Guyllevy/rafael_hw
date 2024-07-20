import pygame
import numpy as np

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
dt = 0.001
time_lim = 5
num_uav = 1

# clip coordinates
flip_clip = np.array([[1, 0], [0, -1]])
translate_clip = np.array([[SCREEN_WIDTH/2, SCREEN_HEIGHT/2]]).transpose()

### functions ###

def display_target(x,y):
    return

def display_uav(x, y, az, size, color):
    # object coordinates x in [-1,1], y in [-1,1]
    uav_vertices = np.array([[-0.3, -0.3],
                            [0.3, -0.3],
                            [0, 0.8]]).transpose() # tringle pointed up

    uav_color = color
    
    # world coordinates - scale and rotate UAV
    # (space- x in [-SCREEN_WIDTH/2, SCREEN_WIDTH/2], 
    #         y in [-SCREEN_HEIGHT/2, SCREEN_HEIGHT/2])
    scale = np.identity(2) * size

    az_rad = az * (np.pi / 180)
    alpha = -np.pi/2 + az_rad
    rotate = np.array([[np.cos(alpha), -np.sin(alpha)],
                       [np.sin(alpha), np.cos(alpha)]])

    translate_model = np.array([[x, y]]).transpose()

    model = scale @ rotate

    # transform
    points = model @ uav_vertices
    points = points + translate_model
    points = flip_clip @ points
    points = points + translate_clip
    points = points.transpose()
    
    pygame.draw.polygon(screen, uav_color, points)
    
    return


### main code ###
data_single_uav = []
with open("UAV0.txt", 'r') as file:
    for line in file:
        line = line.strip()
        time, x, y, az = line.split()
        time = float(time)
        x =    float(x)
        y =    float(y)
        az =   float(az)
        data_single_uav.append((time, x, y, az))
        
pygame.init()

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

run = True
index = 0
while run and index < len(data_single_uav):
    
    screen.fill((150,150,150))
    
    pos = data_single_uav[index]
    display_uav(pos[1], pos[2], pos[3], 25, (200, 40, 40))
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        
    pygame.display.update()
    
    index += 1
    

pygame.quit()
