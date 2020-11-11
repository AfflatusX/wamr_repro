/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <stdlib.h>
#include <string.h>
#include "bh_platform.h"
#include "bh_assert.h"
#include "bh_log.h"
#include "runtime_lib.h"
#include "coap_ext.h"
#include "wasm_export.h"
#include "app_manager_export.h"
#include "./compiled_wasm_app.h"

#include "bi-inc/shared_utils.h"
#include "module_wasm_app.h"
#include <time.h>

#define URL_MAX_LEN 128


bool is_engine_ready = false;

static bool is_wasm_engine_ready()
{
	return is_engine_ready;
}

int g_mid = 0;

static int gen_random_id()
{
	static bool init = false;
	int r;

	if (!init) {
		srand(time(NULL));
		init = true;
	}

	r = rand();
	g_mid = r;

	return r;
}

static request_t *create_request(const char *app_name, const uint8 *file_buf,
				 uint32 size)
{
	request_t request[1] = { 0 };
	char url[URL_MAX_LEN] = { 0 };

	snprintf(url, sizeof(url) - 1, "/applet?name=%s", app_name);

	request_t *req =
		init_request(request, url, COAP_PUT, FMT_APP_RAW_BINARY,
			     (uint8 *)file_buf, size);
	request->mid = gen_random_id();
	return req;
}

void install_app()
{
	printk("[WASM][INFO] waiting for engine to be ready.\n");
	while (!is_wasm_engine_ready()) {
		k_msleep(100);
	}
	printk("[WASM][INFO] engine is now ready.\n");

	const uint8 *wasm_file_buf = (uint8 *)wasm_build_app_aot;
	uint32 wasm_file_size = wasm_build_app_aot_len;

	request_t *request =
		create_request("milan_wasm", wasm_file_buf, wasm_file_size);

	am_dispatch_request(request);

	printk("[WASM][INFO] Dispatched install milan_wasm app request.\n");
}


static k_tid_t wasm_tid;

#define CONFIG_MAIN_THREAD_STACK_SIZE 1024 * 4
#define FACET_START_PAYLOAD_FIX_PART_LEN 1


/* For app manager interfaces. Not implemented for uART */

static bool host_init()
{
	return true;
}

int host_send(void *ctx, const char *buf, int size)
{
	return 0;
}

void host_destroy()
{
}

host_interface interface = { .init = host_init,
			     .send = host_send,
			     .destroy = host_destroy };

/******* Init WASM enviornment and app manager ********/

void init_wasm_engine()
{
	RuntimeInitArgs init_args;
	memset(&init_args, 0, sizeof(RuntimeInitArgs));

	// set mem alloc method
	init_args.mem_alloc_type = Alloc_With_System_Allocator;
	init_args.n_native_symbols = 0;

	if (!wasm_runtime_full_init(&init_args)) {
		printk("[WASM][ERR ] Init runtime environment failed.\n");
		return;
	}

	init_wasm_timer();

	is_engine_ready = true;

	app_manager_startup(&interface);

	printk("[WASM][INFO] Destroying WASM runtime.\n");
	wasm_runtime_destroy();
}

/******************** Spin up thread ******************/

#define MAIN_THREAD_STACK_SIZE (CONFIG_MAIN_THREAD_STACK_SIZE)
#define MAIN_THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(iwasm_main_thread_stack, MAIN_THREAD_STACK_SIZE);
static struct k_thread iwasm_main_thread;

bool wasm_engine_thread_start()
{
	wasm_tid =
		k_thread_create(&iwasm_main_thread, iwasm_main_thread_stack,
				MAIN_THREAD_STACK_SIZE, init_wasm_engine, NULL,
				NULL, NULL, MAIN_THREAD_PRIORITY, 0, K_NO_WAIT);
	if (!wasm_tid) {
		printk("[WASM][ERR ] Failed to start wasm engine thread.\n");
		return false;
	} else {
		printk("[WASM][INFO] WASM engine thread created.\n");
		k_thread_name_set(wasm_tid, "wasm_engine");
		return true;
	}
}

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	wasm_engine_thread_start();
	install_app();

}


