#define _DEFAULT_SOURCE

/**
 * Zomwar keyboard controller — Linux (evdev)
 * Compile: gcc key.c -o key
 * Run:     ./key [serial_port [kbd_device]]
 *          ./key /dev/ttyUSB0
 *          ./key /dev/ttyUSB0 /dev/input/event4
 *
 * Uses /dev/input/eventX for true multi-key support.
 * Needs read access to the input device:
 *   sudo usermod -aG input $USER   (then re-login)
 *
 * To list available keyboard devices:
 *   cat /sys/class/input/event0/device/name  (try event0, event1, ...)
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#define DEFAULT_SERIAL  "/dev/ttyUSB0"
#define BAUD_RATE       B115200

/* ---- serial ---- */
static int sfd = -1;
static int kfd = -1;

static int serial_open(const char *port) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) { perror(port); return -1; }
    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetispeed(&tty, BAUD_RATE);
    cfsetospeed(&tty, BAUD_RATE);
    tty.c_cflag  = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);
    tty.c_iflag  = IGNPAR;
    tty.c_oflag  = 0;
    tty.c_lflag  = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

static void serial_send(const char *cmd) {
    char buf[32];
    int  len = snprintf(buf, sizeof(buf), "%s\r\n", cmd);
    write(sfd, buf, len);
    printf("[send] %s\n", cmd);
    fflush(stdout);
}

/* ---- find keyboard device ---- */
#define BITS_PER_LONG   (sizeof(long) * 8)
#define NBITS(x)        (((x) + BITS_PER_LONG - 1) / BITS_PER_LONG)
#define TEST_BIT(b, a)  (((a)[(b)/BITS_PER_LONG] >> ((b) % BITS_PER_LONG)) & 1)

static int is_keyboard(int fd) {
    unsigned long evbits[NBITS(EV_MAX)]   = {0};
    unsigned long keybits[NBITS(KEY_MAX)] = {0};
    if (ioctl(fd, EVIOCGBIT(0,      sizeof(evbits)),  evbits)  < 0) return 0;
    if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits) < 0) return 0;
    return TEST_BIT(EV_KEY,    evbits)  &&
           TEST_BIT(KEY_UP,    keybits) &&
           TEST_BIT(KEY_DOWN,  keybits) &&
           TEST_BIT(KEY_SPACE, keybits);
}

static int find_keyboard(char *out, size_t len) {
    char path[64];
    for (int i = 0; i < 32; i++) {
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;
        int ok = is_keyboard(fd);
        close(fd);
        if (ok) { snprintf(out, len, "%s", path); return 0; }
    }
    return -1;
}

/* ---- cleanup ---- */
static void cleanup(void) {
    static int done = 0;
    if (done++) return;
    if (kfd >= 0) { ioctl(kfd, EVIOCGRAB, 0); close(kfd); }
    if (sfd >= 0) close(sfd);
}

static void sig_handler(int sig) {
    (void)sig;
    cleanup();
    printf("\n[quit]\n");
    _exit(0);
}

/* ---- main ---- */
int main(int argc, char *argv[]) {
    const char *serial_port = (argc > 1) ? argv[1] : DEFAULT_SERIAL;
    char kbd_path[64];

    if (argc > 2) {
        snprintf(kbd_path, sizeof(kbd_path), "%s", argv[2]);
    } else {
        printf("[searching] Looking for keyboard in /dev/input/event*...\n");
        if (find_keyboard(kbd_path, sizeof(kbd_path)) < 0) {
            fprintf(stderr,
                "No keyboard found.\n"
                "Usage:  %s [serial_port] [kbd_device]\n"
                "Fix permissions: sudo usermod -aG input $USER  (re-login)\n",
                argv[0]);
            return 1;
        }
    }

    /* show device name */
    {
        int fd = open(kbd_path, O_RDONLY | O_NONBLOCK);
        char name[256] = "unknown";
        if (fd >= 0) { ioctl(fd, EVIOCGNAME(sizeof(name)), name); close(fd); }
        printf("[kbd] %s  (%s)\n", kbd_path, name);
    }

    sfd = serial_open(serial_port);
    if (sfd < 0) return 1;
    printf("[ok] Connected to %s @ 115200\n\n", serial_port);

    kfd = open(kbd_path, O_RDONLY | O_NONBLOCK);
    if (kfd < 0) {
        perror(kbd_path);
        fprintf(stderr, "Fix: sudo usermod -aG input $USER  (then re-login)\n");
        cleanup();
        return 1;
    }

    if (ioctl(kfd, EVIOCGRAB, 1) < 0)
        fprintf(stderr, "[warn] EVIOCGRAB failed — keypresses will also go to terminal\n");

    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);

    printf("  Hold Arrow Up   -> gunner moves up\n");
    printf("  Hold Arrow Down -> gunner moves down\n");
    printf("  Space           -> fire\n");
    printf("  Esc             -> reset game\n");
    printf("  Q               -> quit\n\n");

    struct input_event ev;

    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(kfd, &fds);

        int ret = select(kfd + 1, &fds, NULL, NULL, NULL);
        if (ret < 0) { if (errno == EINTR) continue; break; }

        ssize_t n = read(kfd, &ev, sizeof(ev));
        if (n < (ssize_t)sizeof(ev)) continue;
        if (ev.type  != EV_KEY) continue;
        if (ev.value == 2)      continue;  /* skip auto-repeat */

        int pressed = (ev.value == 1);

        switch (ev.code) {
        case KEY_UP:    serial_send(pressed ? "key U" : "key u"); break;
        case KEY_DOWN:  serial_send(pressed ? "key D" : "key d"); break;
        case KEY_SPACE: if (pressed)  serial_send("key f");        break;
        case KEY_ESC:   if (pressed)  serial_send("key r");        break;
        case KEY_Q:     if (pressed) goto quit;                   break;
        }
    }

quit:
    cleanup();
    printf("[quit]\n");
    return 0;
}
