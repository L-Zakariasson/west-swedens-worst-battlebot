# Example file showing a basic pygame "game loop"
import pygame
import serial

# pygame setup
pygame.init()

screen = pygame.display.set_mode((720, 360))
clock = pygame.time.Clock()
running = True

controller = pygame.joystick.Joystick(0)
serialPort = serial.Serial(
    port="COM12", baudrate=9600, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
)

while running:

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
	

    if controller.get_button(10) == 1:
        weapon = bytes([61])

        print(weapon)
        serialPort.write(weapon)
    else: 
        weapon = bytes([62])

        print(weapon)
        serialPort.write(weapon)

    if controller.get_axis(1) > 0:
        motorspped_l_down = bytes([abs(int(round(controller.get_axis(1)*60)))+0])
        
        print("L stick down ",motorspped_l_down)
        serialPort.write(motorspped_l_down)

    if controller.get_axis(1) < 0:
        motorspped_l_up = bytes([abs(int(round(controller.get_axis(1)*60)))+64])

        print("L stick up ",motorspped_l_up)
        serialPort.write(motorspped_l_up)       

    if controller.get_axis(3) > 0:
        motorspped_r_down = bytes([abs(int(round(controller.get_axis(3)*60)))+128])
        print("R stick down ",motorspped_r_down)
        serialPort.write(motorspped_r_down)

    if controller.get_axis(3) < 0:
        motorspped_r_up = bytes([abs(int(round(controller.get_axis(3)*60)))+192]) 
        print("R stick up ",motorspped_r_up)  
        serialPort.write(motorspped_r_up)

    pygame.display.flip() # do i need?

    clock.tick(60)  # limits FPS to 60

pygame.quit()