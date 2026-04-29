#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mgba/core/config.h>
#include <mgba/core/core.h>
#include <mgba/core/serialize.h>
#include <mgba-util/common.h>
#include <mgba-util/vfs.h>

static void print_usage(const char *program)
{
    fprintf(stderr, "usage: %s ROM STATE PNG\n", program);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        print_usage(argv[0]);
        return 2;
    }

    const char *romPath = argv[1];
    const char *statePath = argv[2];
    const char *pngPath = argv[3];
    struct mCore *core = mCoreFind(romPath);
    if (core == NULL)
    {
        fprintf(stderr, "failed to create mGBA core for %s\n", romPath);
        return 3;
    }

    if (!core->init(core))
    {
        fprintf(stderr, "failed to initialize mGBA core\n");
        core->deinit(core);
        return 4;
    }

    const unsigned stride = 256;
    const unsigned height = 256;
    uint32_t *pixels = calloc(stride * height, sizeof(*pixels));
    if (pixels == NULL)
    {
        fprintf(stderr, "failed to allocate video buffer\n");
        core->deinit(core);
        return 5;
    }
    core->setVideoBuffer(core, (void *)pixels, stride);

    if (!mCoreLoadFile(core, romPath))
    {
        fprintf(stderr, "failed to load ROM %s\n", romPath);
        free(pixels);
        core->deinit(core);
        return 6;
    }

    mCoreConfigInit(&core->config, "state-screenshot");
    mCoreConfigLoad(&core->config);
    struct mCoreOptions opts = {0};
    mCoreConfigMap(&core->config, &opts);
    opts.audioSync = false;
    opts.videoSync = false;
    mCoreConfigLoadDefaults(&core->config, &opts);
    mCoreLoadConfig(core);

    core->reset(core);

    struct VFile *stateFile = VFileOpen(statePath, O_RDONLY);
    if (stateFile == NULL)
    {
        fprintf(stderr, "failed to open state %s\n", statePath);
        mCoreConfigFreeOpts(&opts);
        mCoreConfigDeinit(&core->config);
        free(pixels);
        core->deinit(core);
        return 7;
    }

    if (!mCoreLoadStateNamed(core, stateFile, 0))
    {
        fprintf(stderr, "failed to load state %s\n", statePath);
        stateFile->close(stateFile);
        mCoreConfigFreeOpts(&opts);
        mCoreConfigDeinit(&core->config);
        free(pixels);
        core->deinit(core);
        return 8;
    }
    stateFile->close(stateFile);

    core->runFrame(core);

    struct VFile *pngFile = VFileOpen(pngPath, O_WRONLY | O_CREAT | O_TRUNC);
    if (pngFile == NULL)
    {
        fprintf(stderr, "failed to open screenshot %s\n", pngPath);
        mCoreConfigFreeOpts(&opts);
        mCoreConfigDeinit(&core->config);
        free(pixels);
        core->deinit(core);
        return 9;
    }

    bool ok = mCoreTakeScreenshotVF(core, pngFile);
    pngFile->close(pngFile);
    mCoreConfigFreeOpts(&opts);
    mCoreConfigDeinit(&core->config);
    free(pixels);
    core->deinit(core);

    if (!ok)
    {
        fprintf(stderr, "failed to write screenshot %s\n", pngPath);
        return 10;
    }

    return 0;
}
