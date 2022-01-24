import pygame
import random

pygame.init()

white = (255, 255, 255)
black = (0, 0, 0)
red = (255, 0, 0)
magenta = (220, 20, 60)
green = (0, 255, 0)

display_params = (500, 500)
display = pygame.display.set_mode(display_params)
pygame.display.set_caption('Simple Snake')

clock = pygame.time.Clock()
block_size = 10
font = pygame.font.SysFont("Arial", 35)
snake_speed = 10


def draw_text(text, color, position, bias=(False, False)):
    value = font.render(text, True, color)
    x = position[0] - value.get_width() // 2
    y = position[1] - value.get_height() // 2
    if bias[0]:
        x = position[0]
    if bias[1]:
        y = position[1]
    display.blit(value, (x, y))


def draw_snake(snake):
    is_head = True
    for x in snake:
        color = red
        if is_head:
            color = magenta
            is_head = False
        pygame.draw.rect(display, color, (x[0], x[1], block_size, block_size))


def draw_food(position):
    pygame.draw.rect(display, green, (position[0], position[1], block_size, block_size))


def draw_frame(snake, food_pos):
    display.fill(black)
    draw_food(food_pos)
    draw_snake(snake)
    draw_text(str(len(snake) - 1), white, (display_params[0] // 2, 0), (False, True))
    pygame.display.update()


def calc_food_position(bias=(10, 10)):
    x = round(random.randrange(bias[0], display_params[0] -
                               block_size - bias[0]) / block_size) * block_size
    y = round(random.randrange(bias[1], display_params[1] -
                               block_size - bias[1]) / block_size) * block_size
    return x, y


def check_collisions(snake):
    if (snake[0][0] >= display_params[0] or
            snake[0][0] < 0 or
            snake[0][1] >= display_params[1] or
            snake[0][1] < 0):
        return True

    for i in range(1, len(snake)):
        if snake[0] == snake[i]:
            return True

    return False


def start():
    display.fill(black)
    draw_text('Hello! Enjoy the game', white, (display_params[0] // 2, display_params[1] // 2))
    pygame.display.update()
    pygame.time.delay(1500)


def update():
    snake_direction = (0, -block_size)
    snake = [[display_params[0] // 2, display_params[1] // 2]]
    food_pos = calc_food_position()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT and snake_direction != (block_size, 0):
                    snake_direction = (-block_size, 0)
                elif event.key == pygame.K_RIGHT and snake_direction != (-block_size, 0):
                    snake_direction = (block_size, 0)
                elif event.key == pygame.K_UP and snake_direction != (0, block_size):
                    snake_direction = (0, -block_size)
                elif event.key == pygame.K_DOWN and snake_direction != (0, -block_size):
                    snake_direction = (0, block_size)

        if check_collisions(snake):
            return len(snake) - 1

        for i in range(len(snake) - 1, 0, -1):
            snake[i][0] = snake[i - 1][0]
            snake[i][1] = snake[i - 1][1]

        snake[0][0] += snake_direction[0]
        snake[0][1] += snake_direction[1]

        draw_frame(snake, food_pos)

        if tuple(snake[0]) == food_pos:
            food_pos = calc_food_position()
            snake.append([snake[-1][0] - snake_direction[0],
                          snake[-1][1] - snake_direction[1]])

        clock.tick(snake_speed)


def finish(player_score):
    display.fill(black)
    draw_text(f'You lost! Your score: {player_score}', red, (display_params[0] // 2, display_params[1] // 2))
    pygame.display.update()
    pygame.time.delay(1500)
    pygame.quit()


start()
score = update()
finish(score)
