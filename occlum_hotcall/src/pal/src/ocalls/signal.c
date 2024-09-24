#include "ocalls.h"

int occlum_ocall_tkill(int tid, int signum) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int tgid = getpid();
    int ret = TGKILL(tgid, tid, signum);
    return ret;
}
