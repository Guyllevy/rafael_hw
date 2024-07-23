import pygame
import numpy as np
import colorsys

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600

# setting vertices
# object coordinates x in [-1,1], y in [-1,1]
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

target_vertices = np.array([[-0.3, -0.3],
                            [0.3, -0.3],
                            [0.3, 0.3],
                            [-0.3, 0.3]]).transpose()

grid_point_vertices = np.array([[0.5, -0.2],
                                [-0.5, -0.2],
                                [0, 0.8] ]).transpose()

# setting projection
# clip coordinates
clip_scale = 0.3
flip_clip = np.array([[1, 0], [0, -1]])
scale_clip = np.identity(2) * clip_scale
translate_clip = np.array([[SCREEN_WIDTH/2, SCREEN_HEIGHT/2]]).transpose()


# reading file for N_uav
d = {}
with open("io_files/SimParams.ini", 'r') as file:
    for line in file:
        line = line.strip()
        key, value = line.split('=')
        key = key.strip()
        value = value.strip() 
        d[key] = value

num_uav = int(d["N_uav"])
dt = float(d["Dt"])



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

def display_object(surface, vertices, color, model, translate_model):
    points = model @ vertices
    points = points + translate_model

    points = flip_clip @ scale_clip @ points
    points = points + translate_clip

    points = points.transpose()
    
    pygame.draw.polygon(surface, color, points)

def display_target(surface, x, y, size, color):

    # world coordinates - all R^2
    scale = np.identity(2) * size
    translate_model = np.array([[x, y]]).transpose()
    model = scale

    display_object(surface, target_vertices, color, model, translate_model)
    
    return

def display_uav(surface, x, y, az, size, color):
    az_rad = az * (np.pi / 180)
    alpha = -np.pi/2 + az_rad
    model = np.array([[np.cos(alpha), -np.sin(alpha)],
                       [np.sin(alpha), np.cos(alpha)]]) * size

    translate_model = np.array([[x, y]]).transpose()

    display_object(surface, uav_vertices, color, model, translate_model)
    
    return

def display_grid(surface, span, step, color):
    for x in range(-span, span+1, step):
        for y in range(-span, span+1, step):
            model = np.identity(2)
            translate_model = np.array([[x, y]]).transpose()
            display_object(surface, grid_point_vertices, color, model, translate_model)
    display_object(surface, grid_point_vertices, color, 20*np.identity(2), np.array([[0, 0]]).transpose())


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
furthest_target = max([max(abs(x), abs(y)) for (_,_,x,y) in data_targets])

            
pygame.init()

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
clock = pygame.time.Clock()
# Font for displaying FPS
font = pygame.font.SysFont("Arial", 18)
frame_rate_limit = int(1/dt)

# defining a background and drawing on it static objects, like targets - speed optimization
background = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT))
background.fill((40, 40, 40))

# drawing a grid
display_grid(background, 2*int(furthest_target), 100, (140, 140, 140))

# displaying targets
for target in data_targets:
    display_target(background, target[2], target[3], 30, uav_colors[target[1]])

run = True
data_point_index = 0
while run and data_point_index < len(data_uavs[0]):

    screen.blit(background, (0,0))

    # display UAVs
    for uav_index in range(len(data_uavs)):
        pos = data_uavs[uav_index][data_point_index]
        display_uav(screen, pos[1], pos[2], pos[3], 40, uav_colors[uav_index])

    fps = clock.get_fps()
    fps_text = font.render(f"FPS: {int(fps)}", True, pygame.Color('white'))
    screen.blit(fps_text, (10, 10))
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        
    pygame.display.update()
    
    data_point_index += 1

    clock.tick(frame_rate_limit)
    

pygame.quit()
