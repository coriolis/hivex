/* mmap replacement for tsk
 *
 * Copyright (C) 2011 by Coriolis Technologies (vmxray at coriolis.co.in)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

/*
 * We do a sneaky mmap emulation here. Return a buffer and mprotect it so that
 * we get a segfault when hivex accesses any part of the buffer.
 * In the signal handler, we do a tsk_read of a page or two, and
 * unprotect that part of the buffer.
 */

#include "hivex.h"
#include "hivex-internal.h"
#include "mmap-tsk.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define PAGESZ 4096L
#define READAHEAD (PAGESZ * 2)

static int sighandle_installed = 0;
static int g_fd = -1;
static void *g_map = NULL;
static int g_len = -1;
static hive_h *g_h = NULL;

static void
handler(int sig, siginfo_t *si, void *unused)
{
    void *addr = si->si_addr;
    int len;
    off_t off;

    len = READAHEAD;
    if (g_h->msglvl >= 2)
        fprintf(stderr, "Got SIGSEGV at address: 0x%lx %p %p\n", (long) addr, addr + len, g_map + g_len);
    addr = (void *)((unsigned long)addr & ~(PAGESZ - 1));
    if (!(g_map && addr >= g_map && addr < g_map + g_len)) {
        exit(-1);
    }

    if (addr + len > g_map + g_len) {
        len = g_map + g_len - addr;
    }
    off = addr - g_map;
    if (mprotect(addr, len, PROT_READ | PROT_WRITE) < 0) {
        perror("handler: mprotect");
        fprintf(stderr, "mprotect %p %d\n", addr, len);
        exit(-2);
    }
    if (g_h->read(g_fd, addr, len, off) < 0) {
        exit(-3);
    }
}

static void install_sighandler(void)
{
    struct sigaction sa;
    
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        exit(-4);
    }
}

void *
hivex__rpl_mmap (hive_h *h,
                 void *p_addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    void *p_map = NULL;
    
    // Check parameters for unsupported values
    if (p_addr != NULL)
        return MAP_FAILED;
    if (prot != PROT_READ)
        return MAP_FAILED;
    if (flags != MAP_SHARED)
        return MAP_FAILED;

    if (posix_memalign(&p_map, PAGESZ, len) != 0) {
        return MAP_FAILED;
    }
    if (mprotect(p_map, len, PROT_NONE) < 0) {
        free(p_map);
        return MAP_FAILED;
    }
    if (!sighandle_installed) {
        install_sighandler();
        sighandle_installed = 1;
    }    

    g_fd = fd;
    g_map = p_map;
    g_len = len;
    g_h = h;
    fprintf(stderr, "mapping %p len %ld, fd %d\n", p_map, (int)len, fd);
    return p_map;
}

int
hivex__rpl_munmap (hive_h *h, void *p_addr, size_t len)
{
    if (p_addr == NULL)
        return -1;

    g_fd = -1;
    g_map = NULL;
    g_h = NULL;
    free(p_addr);
    return 0;
}
