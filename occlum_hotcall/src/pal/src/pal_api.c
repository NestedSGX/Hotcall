#include <occlum_pal_api.h>
#include "Enclave_u.h"
#include "pal_enclave.h"
#include "pal_error.h"
#include "pal_load_file.h"
#include "pal_interrupt_thread.h"
#include "pal_log.h"
#include "pal_sig_handler.h"
#include "pal_syscall.h"
#include "pal_thread_counter.h"
#include "pal_check_fsgsbase.h"
#ifdef SGX_MODE_HYPER
#include "pal_ms_buffer.h"
#endif
#include "errno2str.h"
#include <linux/limits.h>

#include <pthread.h>
#include "hotcalls/common.h"
#include "hotcalls/hot_calls.h"

HotCall hotOcall = HOTCALL_INITIALIZER;
HotCall *p_hotOcall = &hotOcall;

void setsockopt_hotcall( void* d );
void getsockopt_hotcall( void* d );
void shutdown_hotcall( void* d );
void accept4_hotcall( void* d );
void read_hotcall( void* d );
void fcntl_arg1_hotcall( void* d );
void occlum_sendmsg_hotcall( void* d );
void occlum_recvmsg_hotcall( void* d );
void occlum_ocall_posix_memalign_hotcall( void* d );
void occlum_ocall_free_hotcall( void* d );
void occlum_ocall_ioctl_hotcall( void* d );
void close_hotcall( void* d );
void occlum_ocall_clock_gettime_hotcall( void* d );
void epoll_wait_hotcall( void* d );
void occlum_poll_with_eventfd_hotcall( void* d );
void occlum_eventfd_write_batch_hotcall( void* d );
void sgxprotectedfs_fread_node_hotcall( void* d );

void MyCustomOcall( void* data )
{
    printf("do nothing ocall\n");
}


void* OcallResponderThread( void* p )
{
    printf("OcallResponderThread started.\n");
    void (*callbacks[17])(void*);
    callbacks[0] = setsockopt_hotcall; // ok
    callbacks[1] = getsockopt_hotcall; // ok
    callbacks[2] = shutdown_hotcall;  // will crash the system, big improvement
    callbacks[3] = accept4_hotcall;  // ok
    callbacks[4] = read_hotcall;
    callbacks[5] = fcntl_arg1_hotcall; // ok
    callbacks[6] = occlum_sendmsg_hotcall; // ok
    callbacks[7] = occlum_recvmsg_hotcall; // ok
    callbacks[8] = occlum_ocall_posix_memalign_hotcall; // ok
    callbacks[9] = occlum_ocall_free_hotcall; // ok
    callbacks[10] = occlum_ocall_ioctl_hotcall; // ok
    callbacks[11] = close_hotcall; // ok
    callbacks[12] = occlum_ocall_clock_gettime_hotcall;  // ok
    callbacks[13] = epoll_wait_hotcall;   // crash, no improvement
    callbacks[14] = occlum_poll_with_eventfd_hotcall;  // crash, no improvement
    callbacks[15] = occlum_eventfd_write_batch_hotcall;
    callbacks[16] = sgxprotectedfs_fread_node_hotcall;  

    HotCallTable callTable;
    callTable.numEntries = 17;
    callTable.callbacks  = callbacks;

    /*void (*callbacks[1])(void*);
    callbacks[0] = MyCustomOcall;

    HotCallTable callTable;
    callTable.numEntries = 1;
    callTable.callbacks  = callbacks;*/

    //HotCall_waitForCall( &hotOcall, &callTable );
    HotCall_waitForCall( p_hotOcall, &callTable );

    printf("OcallResponderThread exited.\n");
    return NULL;
}

int occlum_pal_get_version(void) {
    return OCCLUM_PAL_VERSION;
}

int pal_run_init_process() {
    const char *init_path = "/bin/init";
    const char *init_argv[2] = {
        "init",
        NULL,
    };
    struct occlum_stdio_fds init_io_fds = {
        .stdin_fd = STDIN_FILENO,
        .stdout_fd = STDOUT_FILENO,
        .stderr_fd = STDERR_FILENO,
    };
    int libos_tid = 0;
    extern const char **environ;
    struct occlum_pal_create_process_args init_process_args = {
        .path = init_path,
        .argv = init_argv,
        .env = environ,
        .stdio = &init_io_fds,
        .pid = &libos_tid,
    };
    if (occlum_pal_create_process(&init_process_args) < 0) {
        return -1;
    }

    int exit_status = 0;
    struct occlum_pal_exec_args init_exec_args = {
        .pid = libos_tid,
        .exit_value = &exit_status,
    };
    if (occlum_pal_exec(&init_exec_args) < 0) {
        return -1;
    }

    // Convert the exit status to a value in a shell-like encoding
    if (WIFEXITED(exit_status)) { // terminated normally
        exit_status = WEXITSTATUS(exit_status) & 0x7F; // [0, 127]
    } else { // killed by signal
        exit_status = 128 + WTERMSIG(exit_status); // [128 + 1, 128 + 64]
    }
    if (exit_status != 0) {
        errno = EINVAL;
        PAL_ERROR("The init process exit with code: %d", exit_status);
        return -1;
    }

    return 0;
}

int occlum_pal_init(const struct occlum_pal_attr *attr) {
    if (attr == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (attr->instance_dir == NULL) {
        errno = EINVAL;
        return -1;
    }

    char resolved_path[PATH_MAX] = {0};
    if (realpath(attr->instance_dir, resolved_path) == NULL) {
        PAL_ERROR("realpath returns %s", errno2str(errno));
        return -1;
    }

// Check only for SGX hardware mode
#ifdef SGX_MODE_HW
    if (check_fsgsbase_enablement() != 0) {
        PAL_ERROR("FSGSBASE enablement check failed.");
        return -1;
    }
#endif

    sgx_enclave_id_t eid = pal_get_enclave_id();
    if (eid != SGX_INVALID_ENCLAVE_ID) {
        PAL_ERROR("Enclave has been initialized.");
        errno = EEXIST;
        return -1;
    }
    //printf("Gloabal p_hotOcall address: %p (pal_api.c)\n", (void *)p_hotOcall);
    pthread_create( &(p_hotOcall->responderThread), NULL, OcallResponderThread, NULL );

    if (pal_register_sig_handlers() < 0) {
        return -1;
    }

    if (pal_init_enclave(resolved_path) < 0) {
        return -1;
    }
    eid = pal_get_enclave_id();

    int ecall_ret = 0;

    load_file_t hostname_ptr = {0, NULL};
    load_file_t hosts_ptr = {0, NULL};
    load_file_t resolv_conf_ptr = {0, NULL};

    pal_load_file(eid, "/etc/hostname", &hostname_ptr);
    pal_load_file(eid, "/etc/hosts", &hosts_ptr);
    pal_load_file(eid, "/etc/resolv.conf", &resolv_conf_ptr);

    struct host_file_buffer_t file_buffer = {
        .hostname_buf = hostname_ptr.buffer,
        .hostname_buf_size = hostname_ptr.size,
        .hosts_buf = hosts_ptr.buffer,
        .hosts_buf_size = hosts_ptr.size,
        .resolv_conf_buf = resolv_conf_ptr.buffer,
        .resolv_conf_buf_size = resolv_conf_ptr.size,
    };

    sgx_status_t ecall_status = occlum_hotcall_init(eid, (uint64_t)p_hotOcall);

    if (ecall_status != SGX_SUCCESS) {
        PAL_ERROR("Failed to pass hotOcall to the enclave, hotOcall addr = %p.\n", p_hotOcall);
        goto on_destroy_enclave;
    }

    ecall_status = occlum_ecall_init(eid, &ecall_ret, attr->log_level,
                                resolved_path, &file_buffer);

    free_host_file_buffer(eid, &file_buffer);

    if (ecall_status != SGX_SUCCESS) {
        const char *sgx_err = pal_get_sgx_error_msg(ecall_status);
        PAL_ERROR("Failed to do ECall with error code 0x%x: %s", ecall_status, sgx_err);
        goto on_destroy_enclave;
    }
    if (ecall_ret < 0) {
        errno = -ecall_ret;
        PAL_ERROR("occlum_ecall_init returns %s", errno2str(errno));
        goto on_destroy_enclave;
    }

    if (pal_interrupt_thread_start() < 0) {
        PAL_ERROR("Failed to start the interrupt thread: %s", errno2str(errno));
        goto on_destroy_enclave;
    }

    if (pal_run_init_process() < 0) {
        PAL_ERROR("Failed to run the init process: %s", errno2str(errno));
        goto stop_interrupt_thread;
    }

    return 0;

stop_interrupt_thread:
    if (pal_interrupt_thread_stop() < 0) {
        PAL_WARN("Cannot stop the interrupt thread: %s", errno2str(errno));
    }
on_destroy_enclave:
    if (pal_destroy_enclave() < 0) {
        PAL_WARN("Cannot destroy the enclave");
    }
    return -1;
}

int occlum_pal_create_process(struct occlum_pal_create_process_args *args) {
    int ecall_ret = 0; // libos_tid

    if (args->path == NULL || args->argv == NULL || args->pid == NULL) {
        errno = EINVAL;
        return -1;
    }

    sgx_enclave_id_t eid = pal_get_enclave_id();
    if (eid == SGX_INVALID_ENCLAVE_ID) {
        PAL_ERROR("Enclave is not initialized yet.");
        errno = ENOENT;
        return -1;
    }

#ifndef SGX_MODE_HYPER
    sgx_status_t ecall_status = occlum_ecall_new_process(eid, &ecall_ret, args->path,
                                args->argv, args->env, args->stdio);
#else
    const char **ms_buffer_argv = ms_buffer_convert_string_array(eid, args->argv);
    const char **ms_buffer_env = ms_buffer_convert_string_array(eid, args->env);
    if ((!args->argv != !ms_buffer_argv) || (!args->env != !ms_buffer_env)) {
        PAL_ERROR("Marshal buffer size is not enough");
        return -1;
    }
    sgx_status_t ecall_status = occlum_ecall_new_process(
                                    eid,
                                    &ecall_ret,
                                    args->path,
                                    ms_buffer_argv,
                                    ms_buffer_env,
                                    args->stdio);
    ms_buffer_string_array_free(eid, ms_buffer_argv);
    ms_buffer_string_array_free(eid, ms_buffer_env);
#endif
    if (ecall_status != SGX_SUCCESS) {
        const char *sgx_err = pal_get_sgx_error_msg(ecall_status);
        PAL_ERROR("Failed to do ECall with error code 0x%x: %s", ecall_status, sgx_err);
        return -1;
    }
    if (ecall_ret < 0) {
        errno = -ecall_ret;
        PAL_ERROR("occlum_ecall_new_process returns %s", errno2str(errno));
        return -1;
    }

    *args->pid = ecall_ret;
    return 0;
}

int occlum_pal_exec(struct occlum_pal_exec_args *args) {
    int host_tid = GETTID();
    int ecall_ret = 0;

    if (args->exit_value == NULL) {
        errno = EINVAL;
        return -1;
    }

    sgx_enclave_id_t eid = pal_get_enclave_id();
    if (eid == SGX_INVALID_ENCLAVE_ID) {
        PAL_ERROR("Enclave is not initialized yet.");
        errno = ENOENT;
        return -1;
    }

    pal_thread_counter_inc();
    sgx_status_t ecall_status = occlum_ecall_exec_thread(eid, &ecall_ret, args->pid,
                                host_tid);
    pal_thread_counter_dec();
    if (ecall_status != SGX_SUCCESS) {
        const char *sgx_err = pal_get_sgx_error_msg(ecall_status);
        PAL_ERROR("Failed to do ECall: %s", sgx_err);
        return -1;
    }
    if (ecall_ret < 0) {
        errno = -ecall_ret;
        PAL_ERROR("occlum_ecall_exec_thread returns %s", errno2str(errno));
        return -1;
    }

    *args->exit_value = ecall_ret;

    return 0;
}

int occlum_pal_kill(int pid, int sig) {
    sgx_enclave_id_t eid = pal_get_enclave_id();
    if (eid == SGX_INVALID_ENCLAVE_ID) {
        errno = ENOENT;
        PAL_ERROR("Enclave is not initialized yet.");
        return -1;
    }

    int ecall_ret = 0;
    sgx_status_t ecall_status = occlum_ecall_kill(eid, &ecall_ret, pid, sig);
    if (ecall_status != SGX_SUCCESS) {
        const char *sgx_err = pal_get_sgx_error_msg(ecall_status);
        PAL_ERROR("Failed to do ECall with error code 0x%x: %s", ecall_status, sgx_err);
        return -1;
    }
    if (ecall_ret < 0) {
        errno = -ecall_ret;
        PAL_ERROR("Failed to occlum_ecall_kill: %s", errno2str(errno));
        return -1;
    }

    return 0;
}

int occlum_pal_destroy(void) {
    sgx_enclave_id_t eid = pal_get_enclave_id();
    if (eid == SGX_INVALID_ENCLAVE_ID) {
        PAL_ERROR("Enclave is not initialized yet.");
        errno = ENOENT;
        return -1;
    }

    int ret = 0;
    if (pal_interrupt_thread_stop() < 0) {
        ret = -1;
        PAL_WARN("Cannot stop the interrupt thread: %s", errno2str(errno));
    }

    if (pal_destroy_enclave() < 0) {
        ret = -1;
        PAL_WARN("Cannot destroy the enclave");
    }
    StopResponder( p_hotOcall );
    return ret;
}

int pal_get_version(void) __attribute__((weak, alias ("occlum_pal_get_version")));

int pal_init(const struct occlum_pal_attr *attr)\
__attribute__ ((weak, alias ("occlum_pal_init")));

int pal_create_process(struct occlum_pal_create_process_args *args)\
__attribute__ ((weak, alias ("occlum_pal_create_process")));

int pal_exec(struct occlum_pal_exec_args *args)\
__attribute__ ((weak, alias ("occlum_pal_exec")));

int pal_kill(int pid, int sig) __attribute__ ((weak, alias ("occlum_pal_kill")));

int pal_destroy(void) __attribute__ ((weak, alias ("occlum_pal_destroy")));
