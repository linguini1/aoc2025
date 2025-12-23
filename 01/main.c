#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIAL_MAX (99)
#define DIAL_MOD (DIAL_MAX + 1)
#define DIAL_INIT_POS (50)

enum rot_e
{
    R_LEFT,
    R_RIGHT,
};

struct move_s
{
    enum rot_e rot;
    uint16_t times;
};

/* NOTE: Assumes null terminated input in the form <L/R><n> */

static void to_move(const char *movestr, struct move_s *move)
{
    move->rot = movestr[0] == 'L' ? R_LEFT : R_RIGHT;
    move->times = strtoul(&movestr[1], NULL, 10);
}

static uint16_t move_dial(uint16_t dialpos, struct move_s *move,
                          uint32_t *zeroes_pw)
{
    uint16_t eq_moves;
    switch (move->rot)
        {
        case R_RIGHT:
            *zeroes_pw += (dialpos + move->times) / DIAL_MOD;
            return (dialpos + move->times) % DIAL_MOD;

        case R_LEFT:
            eq_moves = move->times % DIAL_MOD;
            *zeroes_pw += move->times / DIAL_MOD;
            if (eq_moves > dialpos)
                {
                    if (dialpos != 0) (*zeroes_pw)++;
                    return DIAL_MOD - (eq_moves - dialpos);
                }
            else
                {
                    if (dialpos - eq_moves == 0) (*zeroes_pw)++;
                    return dialpos - eq_moves;
                }
        }
    assert(0 && "Unreachable");
}

int main(void)
{
    char movement_str[8];
    struct move_s move;
    uint16_t dialpos = DIAL_INIT_POS;
    uint32_t zeroes = 0;
    uint32_t zeroes_pw = 0;

    FILE *file = fopen("input.txt", "r");
    if (file == NULL)
        {
            fprintf(stderr, "Couldn't open input file: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

    while (!feof(file))
        {
            if (fgets(movement_str, sizeof(movement_str), file) == NULL) break;
            to_move(movement_str, &move);
            dialpos = move_dial(dialpos, &move, &zeroes_pw);

            if (dialpos == 0) zeroes++;
        }

    printf("%u\n", zeroes);
    printf("%u\n", zeroes_pw);

    return EXIT_SUCCESS;
}
