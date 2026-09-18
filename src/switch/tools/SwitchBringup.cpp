// Native Nintendo Switch Homebrew bring-up. This is intentionally independent
// from the Wii and PS2 ports: it validates libnx services before game backends
// are introduced.
#include <switch.h>

#include <cstdio>
#include <limits.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    consoleInit(nullptr);
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    const Result mountResult = fsdevMountSdmc();
    char cwd[PATH_MAX] = "(unavailable)";
    if (getcwd(cwd, sizeof(cwd)) == nullptr)
        std::snprintf(cwd, sizeof(cwd), "(unavailable)");

    std::printf("OptiCraft Heritage - Switch bring-up\n");
    std::printf("===================================\n\n");
    std::printf("libnx initialized successfully.\n");
    std::printf("SD mount: %s (0x%08X)\n", R_SUCCEEDED(mountResult) ? "OK" : "FAILED", mountResult);
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
