import pygame
import numpy as np
import colorsys

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600

d = {}
with open("io_files/SimParams.ini", 'r') as file:
    for line in file:
        line = line.strip()
        key, value = line.split('=')
        key = key.strip()
        value = value.strip() 
        d[key] = value

num_uav = int(d["N_uav"])


# clip coordinates
clip_scale = 0.4
flip_clip = np.array([[1, 0], [0, -1]])
scale_clip = np.identity(2) * clip_scale
translate_clip = np.array([[SCREEN_WIDTH/2, SCREEN_HEIGHT/2]]).transpose()


### functions ###

def hsl_to_rgb(h, s, l):
    return tuple(round(i * 255) for i in colorsys.hls_to_rgb(h, l, s))

def generate_colors(n, saturation=0.8, lightness=0.4):
    colors = []
    for i in range(n):
        hue = i / n
        rgb_color = hsl_to_rgb(hue, saturation, lightness)
        colors.append(rgb_color)
    return colors

def display_target(x,y,size,color):
     # object coordinates x in [-1,1], y in [-1,1]
    target_vertices = np.array([[-0.3, -0.3],
                            [0.3, -0.3],
                            [0.3, 0.3],
                            [-0.3, 0.3]]).transpose() # tringle pointed up

    target_color = color
    
    # world coordinates - scale and rotate UAV
    # (space- x in [-SCREEN_WIDTH/2, SCREEN_WIDTH/2], 
    #         y in [-SCREEN_HEIGHT/2, SCREEN_HEIGHT/2])
    scale = np.identity(2) * size

    translate_model = np.array([[x, y]]).transpose()

    model = scale

    # transform
    points = model @ target_vertices
    points = points + translate_model
    points = flip_clip @ scale_clip @ points
    points = points + translate_clip
    points = points.transpose()
    
    pygame.draw.polygon(screen, target_color, points)
    
    return

def display_uav(x, y, az, size, color):
    # object coordinates x in [-1,1], y in [-1,1]
    # uav_vertices = np.array([[-0.3, -0.3],
    #                         [0.3, -0.3],
    #                         [0, 0.8]]).transpose() # tringle pointed up

    uav_vertices = np.array(   [[0.00,0.65], 
                                [0.11,0.54], 
                                [0.11,0.43], 
                                [0.06,0.32], 
                                [0.06,0.10], 
                                [0.88,-0.01], 
                                [0.94,-0.12], 
                                [0.06,-0.12], 
                                [0.06,-0.34], 
                                [0.28,-0.45], 
                                [0.28,-0.56], 
                                [0.00,-0.51], 
                                [-0.28,-0.56], 
                                [-0.28,-0.45], 
                                [-0.06,-0.34], 
                                [-0.06,-0.12], 
                                [-0.94,-0.12], 
                                [-0.88,-0.01], 
                                [-0.06,0.10], 
                                [-0.06,0.32], 
                                [-0.11,0.43], 
                                [-0.11,0.54], 
                                [0.00,0.65] ]).transpose()

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
    points = flip_clip @ scale_clip @ points
    points = points + translate_clip
    points = points.transpose()
    
    pygame.draw.polygon(screen, uav_color, points)
    
    return


### main code ###
data_uavs = []
for uav_number in range(num_uav):
    data_single_uav = []
    with open("io_files/UAV" + str(uav_number) + ".txt", 'r') as file:
        for line in file:
            line = line.strip()
            time, x, y, az = line.split()
            time = float(time)
            x =    float(x)
            y =    float(y)
            az =   float(az)
            data_single_uav.append((time, x, y, az))
    data_uavs.append(data_single_uav)

data_targets = []
with open("io_files/SimCmds.txt", 'r') as file:
    for line in file:
        line = line.strip()
        time, targeting_uav, x, y = line.split()
        time = float(time)
        targeting_uav =  int(targeting_uav)
        x =    float(x)
        y =    float(y)
        data_targets.append((time, targeting_uav, x, y))

uav_colors = generate_colors(num_uav)

            
pygame.init()

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

run = True
data_point_index = 0
while run and data_point_index < len(data_uavs[0]):
    
    screen.fill((40, 40, 40))

    # drawing a grid
    for x in range(-1500, 1500, 250):
        for y in range(-1500,1500,250):
            display_target(x, y, 10, (140, 140, 140))
    
    # display UAVs
    for uav_index in range(len(data_uavs)):
        pos = data_uavs[uav_index][data_point_index]
        display_uav(pos[1], pos[2], pos[3], 40, uav_colors[uav_index])

    # displaying targets
    for target in data_targets:
        display_target(target[2], target[3], 30, uav_colors[target[1]])
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        
    pygame.display.update()
    
    data_point_index += 1
    

pygame.quit()
