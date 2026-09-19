// Native Nintendo Switch Homebrew bring-up. This is intentionally independent
// from the Wii and PS2 ports: it validates libnx services before game backends
// are introduced.
#include <switch.h>

#include <cstdint>
#include <cstdio>
#include <limits.h>
#include <unistd.h>

namespace
{
std::uint32_t resultModule(Result result)
{
    return static_cast<std::uint32_t>(result) & 0x1ff;
}

std::uint32_t resultDescription(Result result)
{
    return static_cast<std::uint32_t>(result) >> 9;
}

void printResult(const char* label, Result result)
{
    std::printf("%s: %s (0x%08X; module %u, description %u)\n",
                label,
                R_SUCCEEDED(result) ? "OK" : "FAILED",
                static_cast<unsigned int>(result),
                resultModule(result),
                resultDescription(result));
}
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    consoleInit(nullptr);
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    // Check Horizon's SD-card filesystem service separately from the POSIX
    // devoptab mount. This distinguishes a console/SD service failure from a
    // failure in fsdev's path adapter.
    FsFileSystem sdCardFileSystem;
    const Result nativeFsResult = fsOpenSdCardFileSystem(&sdCardFileSystem);
    if (R_SUCCEEDED(nativeFsResult))
        fsFsClose(&sdCardFileSystem);

    const Result mountResult = fsdevMountSdmc();
    char cwd[PATH_MAX] = "(unavailable)";
    if (getcwd(cwd, sizeof(cwd)) == nullptr)
        std::snprintf(cwd, sizeof(cwd), "(unavailable)");

    std::printf("OptiCraft Heritage - Switch bring-up\n");
    std::printf("===================================\n\n");
    std::printf("libnx initialized successfully.\n");
    printResult("Native SD filesystem", nativeFsResult);
    printResult("SD devoptab mount", mountResult);
    std::printf("Working directory: %s\n\n", cwd);
    std::printf("Press PLUS to exit.\n");

    while (appletMainLoop())
    {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus)
            break;
        consoleUpdate(nullptr);
    }

    if (R_SUCCEEDED(mountResult))
        fsdevUnmountDevice("sdmc");
    consoleExit(nullptr);
    return 0;
}
