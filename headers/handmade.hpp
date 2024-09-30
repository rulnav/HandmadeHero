#include <cstdint>
#include <cstdlib>
#include <math.h>

struct s_gameOffscreenBuffer
{
    void *memory;
    int width;
    int height;
    int pitch;
};

struct s_gameSoundOutputBuffer
{
    int samples_per_second;
    int sample_count;
    int16_t *samples;
};

static void GameUpdateAndRender(s_gameOffscreenBuffer &buffer, int blue_offset, int green_offset, s_gameSoundOutputBuffer &sound_buffer);

static void GameOutputSound(s_gameSoundOutputBuffer &sound_buffer);
