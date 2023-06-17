#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "Logger.hpp"
#include "Result.hpp"

class SETTINGS
{
public:
    static const int SystemCallCount = 512;

private:
    std::string JudgeUser = "Judger";
    std::string BaseFolder;
    std::string SettingBaseFolder;
    int JudgeUserID;
    // std::string JudgeUserGroup = "judge";
    int JudgeUserGroupID;
    struct
    {
        int AvailableCount;
        std::string Name;
    } SystemCalls[SystemCallCount] = {
        {-1, "read"},                   // 0
        {-1, "write"},                  // 1
        {0, "open"},                    // 2
        {-1, "close"},                  // 3
        {-1, "stat"},                   // 4
        {-1, "fstat"},                  // 5
        {0, "lstat"},                   // 6
        {0, "poll"},                    // 7
        {-1, "lseek"},                  // 8
        {-1, "mmap"},                   // 9
        {-1, "mprotect"},               // 10
        {-1, "munmap"},                 // 11
        {-1, "brk"},                    // 12
        {0, "rt_sigaction"},            // 13
        {0, "rt_sigprocmask"},          // 14
        {0, "rt_sigreturn"},            // 15
        {0, "ioctl"},                   // 16
        {-1, "pread64"},                // 17
        {0, "pwrite64"},                // 18
        {0, "readv"},                   // 19
        {0, "writev"},                  // 20
        {1, "access"},                  // 21
        {0, "pipe"},                    // 22
        {0, "select"},                  // 23
        {0, "sched_yield"},             // 24
        {0, "mremap"},                  // 25
        {0, "msync"},                   // 26
        {0, "mincore"},                 // 27
        {0, "madvise"},                 // 28
        {0, "shmget"},                  // 29
        {0, "shmat"},                   // 30
        {0, "shmctl"},                  // 31
        {0, "dup"},                     // 32
        {0, "dup2"},                    // 33
        {0, "pause"},                   // 34
        {0, "nanosleep"},               // 35
        {0, "getitimer"},               // 36
        {0, "alarm"},                   // 37
        {0, "setitimer"},               // 38
        {0, "getpid"},                  // 39
        {0, "sendfile"},                // 40
        {0, "socket"},                  // 41
        {0, "connect"},                 // 42
        {0, "accept"},                  // 43
        {0, "sendto"},                  // 44
        {0, "recvfrom"},                // 45
        {0, "sendmsg"},                 // 46
        {0, "recvmsg"},                 // 47
        {0, "shutdown"},                // 48
        {0, "bind"},                    // 49
        {0, "listen"},                  // 50
        {0, "getsockname"},             // 51
        {0, "getpeername"},             // 52
        {0, "socketpair"},              // 53
        {0, "setsockopt"},              // 54
        {0, "getsockopt"},              // 55
        {0, "clone"},                   // 56
        {0, "fork"},                    // 57
        {0, "vfork"},                   // 58
        {1, "execve"},                  // 59
        {0, "exit"},                    // 60
        {0, "wait4"},                   // 61
        {0, "kill"},                    // 62
        {1, "uname"},                   // 63
        {0, "semget"},                  // 64
        {0, "semop"},                   // 65
        {0, "semctl"},                  // 66
        {0, "shmdt"},                   // 67
        {0, "msgget"},                  // 68
        {0, "msgsnd"},                  // 69
        {0, "msgrcv"},                  // 70
        {0, "msgctl"},                  // 71
        {0, "fcntl"},                   // 72
        {0, "flock"},                   // 73
        {0, "fsync"},                   // 74
        {0, "fdatasync"},               // 75
        {0, "truncate"},                // 76
        {0, "ftruncate"},               // 77
        {0, "getdents"},                // 78
        {0, "getcwd"},                  // 79
        {0, "chdir"},                   // 80
        {0, "fchdir"},                  // 81
        {0, "rename"},                  // 82
        {0, "mkdir"},                   // 83
        {0, "rmdir"},                   // 84
        {0, "creat"},                   // 85
        {0, "link"},                    // 86
        {0, "unlink"},                  // 87
        {0, "symlink"},                 // 88
        {1, "readlink"},                // 89
        {0, "chmod"},                   // 90
        {0, "fchmod"},                  // 91
        {0, "chown"},                   // 92
        {0, "fchown"},                  // 93
        {0, "lchown"},                  // 94
        {0, "umask"},                   // 95
        {0, "gettimeofday"},            // 96
        {0, "getrlimit"},               // 97
        {0, "getrusage"},               // 98
        {0, "sysinfo"},                 // 99
        {0, "times"},                   // 100
        {0, "ptrace"},                  // 101
        {0, "getuid"},                  // 102
        {0, "syslog"},                  // 103
        {0, "getgid"},                  // 104
        {0, "setuid"},                  // 105
        {0, "setgid"},                  // 106
        {0, "geteuid"},                 // 107
        {0, "getegid"},                 // 108
        {0, "setpgid"},                 // 109
        {0, "getppid"},                 // 110
        {0, "getpgrp"},                 // 111
        {0, "setsid"},                  // 112
        {0, "setreuid"},                // 113
        {0, "setregid"},                // 114
        {0, "getgroups"},               // 115
        {0, "setgroups"},               // 116
        {0, "setresuid"},               // 117
        {0, "getresuid"},               // 118
        {0, "setresgid"},               // 119
        {0, "getresgid"},               // 120
        {0, "getpgid"},                 // 121
        {0, "setfsuid"},                // 122
        {0, "setfsgid"},                // 123
        {0, "getsid"},                  // 124
        {0, "capget"},                  // 125
        {0, "capset"},                  // 126
        {0, "rt_sigpending"},           // 127
        {0, "rt_sigtimedwait"},         // 128
        {0, "rt_sigqueueinfo"},         // 129
        {0, "rt_sigsuspend"},           // 130
        {0, "sigaltstack"},             // 131
        {0, "utime"},                   // 132
        {0, "mknod"},                   // 133
        {0, "uselib"},                  // 134
        {0, "personality"},             // 135
        {0, "ustat"},                   // 136
        {0, "statfs"},                  // 137
        {0, "fstatfs"},                 // 138
        {0, "sysfs"},                   // 139
        {0, "getpriority"},             // 140
        {0, "setpriority"},             // 141
        {0, "sched_setparam"},          // 142
        {0, "sched_getparam"},          // 143
        {0, "sched_setscheduler"},      // 144
        {0, "sched_getscheduler"},      // 145
        {0, "sched_get_priority_max"},  // 146
        {0, "sched_get_priority_min"},  // 147
        {0, "sched_rr_get_interval"},   // 148
        {0, "mlock"},                   // 149
        {0, "munlock"},                 // 150
        {0, "mlockall"},                // 151
        {0, "munlockall"},              // 152
        {0, "vhangup"},                 // 153
        {0, "modify_ldt"},              // 154
        {0, "pivot_root"},              // 155
        {0, "_sysctl"},                 // 156
        {0, "prctl"},                   // 157
        {-1, "arch_prctl"},             // 158
        {0, "adjtimex"},                // 159
        {0, "setrlimit"},               // 160
        {0, "chroot"},                  // 161
        {0, "sync"},                    // 162
        {0, "acct"},                    // 163
        {0, "settimeofday"},            // 164
        {0, "mount"},                   // 165
        {0, "umount2"},                 // 166
        {0, "swapon"},                  // 167
        {0, "swapoff"},                 // 168
        {0, "reboot"},                  // 169
        {0, "sethostname"},             // 170
        {0, "setdomainname"},           // 171
        {0, "iopl"},                    // 172
        {0, "ioperm"},                  // 173
        {0, "create_module"},           // 174
        {0, "init_module"},             // 175
        {0, "delete_module"},           // 176
        {0, "get_kernel_syms"},         // 177
        {0, "query_module"},            // 178
        {0, "quotactl"},                // 179
        {0, "nfsservctl"},              // 180
        {0, "getpmsg"},                 // 181
        {0, "putpmsg"},                 // 182
        {0, "afs_syscall"},             // 183
        {0, "tuxcall"},                 // 184
        {0, "security"},                // 185
        {0, "gettid"},                  // 186
        {0, "readahead"},               // 187
        {0, "setxattr"},                // 188
        {0, "lsetxattr"},               // 189
        {0, "fsetxattr"},               // 190
        {0, "getxattr"},                // 191
        {0, "lgetxattr"},               // 192
        {0, "fgetxattr"},               // 193
        {0, "listxattr"},               // 194
        {0, "llistxattr"},              // 195
        {0, "flistxattr"},              // 196
        {0, "removexattr"},             // 197
        {0, "lremovexattr"},            // 198
        {0, "fremovexattr"},            // 199
        {0, "tkill"},                   // 200
        {0, "time"},                    // 201
        {0, "futex"},                   // 202
        {0, "sched_setaffinity"},       // 203
        {0, "sched_getaffinity"},       // 204
        {0, "set_thread_area"},         // 205
        {0, "io_setup"},                // 206
        {0, "io_destroy"},              // 207
        {0, "io_getevents"},            // 208
        {0, "io_submit"},               // 209
        {0, "io_cancel"},               // 210
        {0, "get_thread_area"},         // 211
        {0, "lookup_dcookie"},          // 212
        {0, "epoll_create"},            // 213
        {0, "epoll_ctl_old"},           // 214
        {0, "epoll_wait_old"},          // 215
        {0, "remap_file_pages"},        // 216
        {0, "getdents64"},              // 217
        {0, "set_tid_address"},         // 218
        {0, "restart_syscall"},         // 219
        {0, "semtimedop"},              // 220
        {0, "fadvise64"},               // 221
        {0, "timer_create"},            // 222
        {0, "timer_settime"},           // 223
        {0, "timer_gettime"},           // 224
        {0, "timer_getoverrun"},        // 225
        {0, "timer_delete"},            // 226
        {0, "clock_settime"},           // 227
        {-1, "clock_gettime"},          // 228
        {0, "clock_getres"},            // 229
        {0, "clock_nanosleep"},         // 230
        {-1, "exit_group"},             // 231
        {0, "epoll_wait"},              // 232
        {0, "epoll_ctl"},               // 233
        {0, "tgkill"},                  // 234
        {0, "utimes"},                  // 235
        {0, "vserver"},                 // 236
        {0, "mbind"},                   // 237
        {0, "set_mempolicy"},           // 238
        {0, "get_mempolicy"},           // 239
        {0, "mq_open"},                 // 240
        {0, "mq_unlink"},               // 241
        {0, "mq_timedsend"},            // 242
        {0, "mq_timedreceive"},         // 243
        {0, "mq_notify"},               // 244
        {0, "mq_getsetattr"},           // 245
        {0, "kexec_load"},              // 246
        {0, "waitid"},                  // 247
        {0, "add_key"},                 // 248
        {0, "request_key"},             // 249
        {0, "keyctl"},                  // 250
        {0, "ioprio_set"},              // 251
        {0, "ioprio_get"},              // 252
        {0, "inotify_init"},            // 253
        {0, "inotify_add_watch"},       // 254
        {0, "inotify_rm_watch"},        // 255
        {0, "migrate_pages"},           // 256
        {-1, "openat"},                 // 257
        {0, "mkdirat"},                 // 258
        {0, "mknodat"},                 // 259
        {0, "fchownat"},                // 260
        {0, "futimesat"},               // 261
        {0, "newfstatat"},              // 262
        {0, "unlinkat"},                // 263
        {0, "renameat"},                // 264
        {0, "linkat"},                  // 265
        {0, "symlinkat"},               // 266
        {0, "readlinkat"},              // 267
        {0, "fchmodat"},                // 268
        {0, "faccessat"},               // 269
        {0, "pselect6"},                // 270
        {0, "ppoll"},                   // 271
        {0, "unshare"},                 // 272
        {0, "set_robust_list"},         // 273
        {0, "get_robust_list"},         // 274
        {0, "splice"},                  // 275
        {0, "tee"},                     // 276
        {0, "sync_file_range"},         // 277
        {0, "vmsplice"},                // 278
        {0, "move_pages"},              // 279
        {0, "utimensat"},               // 280
        {0, "epoll_pwait"},             // 281
        {0, "signalfd"},                // 282
        {0, "timerfd_create"},          // 283
        {0, "eventfd"},                 // 284
        {0, "fallocate"},               // 285
        {0, "timerfd_settime"},         // 286
        {0, "timerfd_gettime"},         // 287
        {0, "accept4"},                 // 288
        {0, "signalfd4"},               // 289
        {0, "eventfd2"},                // 290
        {0, "epoll_create1"},           // 291
        {-1, "dup3"},                   // 292
        {0, "pipe2"},                   // 293
        {0, "inotify_init1"},           // 294
        {0, "preadv"},                  // 295
        {0, "pwritev"},                 // 296
        {0, "rt_tgsigqueueinfo"},       // 297
        {0, "perf_event_open"},         // 298
        {0, "recvmmsg"},                // 299
        {0, "fanotify_init"},           // 300
        {0, "fanotify_mark"},           // 301
        {0, "prlimit64"},               // 302
        {0, "name_to_handle_at"},       // 303
        {0, "open_by_handle_at"},       // 304
        {0, "clock_adjtime"},           // 305
        {0, "syncfs"},                  // 306
        {0, "sendmmsg"},                // 307
        {0, "setns"},                   // 308
        {0, "getcpu"},                  // 309
        {0, "process_vm_readv"},        // 310
        {0, "process_vm_writev"},       // 311
        {0, "kcmp"},                    // 312
        {0, "finit_module"},            // 313
        {0, "sched_setattr"},           // 314
        {0, "sched_getattr"},           // 315
        {0, "renameat2"},               // 316
        {0, "seccomp"},                 // 317
        {0, "getrandom"},               // 318
        {0, "memfd_create"},            // 319
        {0, "kexec_file_load"},         // 320
        {0, "bpf"},                     // 321
        {0, "execveat"},                // 322
        {0, "userfaultfd"},             // 323
        {0, "membarrier"},              // 324
        {0, "mlock2"},                  // 325
        {0, "copy_file_range"},         // 326
        {0, "preadv2"},                 // 327
        {0, "pwritev2"},                // 328
        {0, "pkey_mprotect"},           // 329
        {0, "pkey_alloc"},              // 330
        {0, "pkey_free"},               // 331
        {0, "statx"},                   // 332
        {0, "io_pgetevents"},           // 333
        {0, "rseq"},                    // 334
        {0, "pidfd_send_signal"},       // 424
        {0, "io_uring_setup"},          // 425
        {0, "io_uring_enter"},          // 426
        {0, "io_uring_register"},       // 427
        {0, "open_tree"},               // 428
        {0, "move_mount"},              // 429
        {0, "fsopen"},                  // 430
        {0, "fsconfig"},                // 431
        {0, "fsmount"},                 // 432
        {0, "fspick"},                  // 433
        {0, "pidfd_open"},              // 434
        {0, "clone3"},                  // 435
        {0, "close_range"},             // 436
        {0, "openat2"},                 // 437
        {0, "pidfd_getfd"},             // 438
        {0, "faccessat2"},              // 439
        {0, "process_madvise"},         // 440
        {0, "epoll_pwait2"},            // 441
        {0, "mount_setattr"},           // 442
        {0, "quotactl_fd"},             // 443
        {0, "landlock_create_ruleset"}, // 444
        {0, "landlock_add_rule"},       // 445
        {0, "landlock_restrict_self"},  // 446
        {0, "memfd_secret"},            // 447
        {0, "process_mrelease"}         // 448
    };
    std::string Compiler = "/bin/g++";
    std::string RunDir;

    int SocketPort = 80;
    int CompileTimeLimit = 60 * 1000;
    int CompileOutputLimit = 128 * 1024 * 1024;

    std::string Email;
    std::string EmailPassword;

    class DATABASE_SETTINGS
    {
    private:
        std::string Host = "localhost";
        int Port = 3306;
        std::string Username = "langningc2009";
        std::string Password = "1!2@3#qQwWeE";
        std::string DatabaseName = "OJ";

        friend class SETTINGS;

    public:
        std::string GetHost();
        int GetPort();
        std::string GetUsername();
        std::string GetPassword();
        std::string GetDatabaseName();

        RESULT Set(std::string Host, int Port, std::string Username, std::string Password, std::string DatabaseName);
    };

    void CheckJudgeUser();
    void GetJudgeUserIDByUserName();
    // void GetJudgeUserGroupNameByGroupID();
    void CheckCompiler();

    friend class WEB_DATA_PROCEED;

public:
    RESULT Save();
    RESULT Load(std::string JudgeUser);

    std::string GetBaseFolder();
    std::string GetJudgeUser();
    std::string GetSettingsBaseFolder();
    int GetJudgeUserID();
    // std::string GetJudgeUserGroup();
    int GetJudgeUserGroupID();
    bool IsBannedSystemCall(int SystemCall, int CallCount);
    std::string GetSystemCallName(int SystemCall);
    std::string GetCompiler();
    std::string GetRunDir();
    int GetSocketPort();
    int GetCompileTimeLimit();
    int GetCompileOutputLimit();
    std::string GetEmail();
    std::string GetEmailPassword();

    void SetJudgeUser(std::string JudgeUser);
    void SetSystemCallAvailableCount(int SystemCall, int AvailableCount);
    void SetCompiler(std::string Compiler);
    void SetSocketPort(int SocketPort);
    void SetCompileTimeLimit(int CompileTimeLimit);
    void SetCompileOutputLimit(int CompileOutputLimit);
    void SetEmail(std::string Email);
    void SetEmailPassword(std::string EmailPassword);

    void CheckSettings();

    DATABASE_SETTINGS DatabaseSettings;
};

extern SETTINGS Settings;

#endif

/*
sudo groupadd judge
sudo useradd Judger -g judge
sudo passwd Judger -d
sudo mkdir /home/Judger
sudo chown -R Judger /home/Judger
sudo chgrp -R judge /home/Judger
echo -e "Judger\tALL=(ALL:ALL) ALL" | sudo tee -a /etc/sudoers > /dev/null
sudo chsh Judger -s /usr/bin/fish
sudo chmod -R a+rw /home/langningc2009/Coding

sudo chmod a+s /bin/chgrp
sudo chmod a+s /bin/chown
*/
