struct game_offscreen_buffer
{
    void *memory;
    int width;
    int height;
    int pitch;
};

static void GameUpdateAndRender(game_offscreen_buffer &buffer, int blue_offset, int green_offset);
