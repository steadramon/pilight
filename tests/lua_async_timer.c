/*
	Copyright (C) 2013 - 2016 CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "../libs/pilight/core/log.h"
#include "../libs/pilight/core/CuTest.h"
#include "../libs/pilight/core/pilight.h"
#include "../libs/pilight/core/common.h"
#include "../libs/pilight/lua_c/lua.h"
#include "../libs/pilight/lua_c/lualibrary.h"

static int run = 0;
static int test = 0;
static CuTest *gtc = NULL;
static uv_timer_t *timer_req = NULL;

static int plua_print(lua_State* L) {
	if(test == 1) {
		switch(run) {
			case 0:
			case 1:
			case 2: 
			case 3: {
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "table", lua_tostring(L, -1));
				run++;
			} break;
			case 4: {
				CuAssertIntEquals(gtc, LUA_TNUMBER, lua_type(L, -1));
				CuAssertIntEquals(gtc, 5, lua_tonumber(L, -1));
				run++;
			} break;
			case 5:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "main", lua_tostring(L, -1));
				run++;
			break;
			case 6: {
				CuAssertIntEquals(gtc, LUA_TNUMBER, lua_type(L, -1));
				CuAssertIntEquals(gtc, 4, lua_tonumber(L, -1));
				run++;
			} break;
			case 7:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "main1", lua_tostring(L, -1));
				run++;
			break;
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15: {
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "boolean", lua_tostring(L, -1));
				run++;
			} break;
			case 16:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "timer", lua_tostring(L, -1));
				run++;
			break;
			case 17:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "boolean", lua_tostring(L, -1));
				run++;
			break;
			case 18:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "timer1", lua_tostring(L, -1));
				run++;
				uv_stop(uv_default_loop());
			break;
		}
	} else if(test == 2) {
		switch(run) {
			case 0:
			case 1: {
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "table", lua_tostring(L, -1));
				run++;
			} break;
			case 2:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "main", lua_tostring(L, -1));
				run++;
			break;
			case 3:
				CuAssertIntEquals(gtc, LUA_TLIGHTUSERDATA, lua_type(L, -1));
				run++;
			break;
			case 4:
				CuAssertIntEquals(gtc, LUA_TSTRING, lua_type(L, -1));
				CuAssertStrEquals(gtc, "timer", lua_tostring(L, -1));
				run++;
			break;
		}
	} else {
		run++;
	}

	return 0;
}

static int call(struct lua_State *L, char *file, char *func) {
	lua_getfield(L, -1, func);
	if(lua_type(L, -1) != LUA_TFUNCTION) {
		return -1;
	}

	if(plua_pcall(L, file, 0, 0) == -1) {
		assert(plua_check_stack(L, 0) == 0);
		return -1;
	}
	lua_pop(L, 1);

	return 1;
}

static void close_cb(uv_handle_t *handle) {
	if(handle != NULL) {
		FREE(handle);
	}
}

static void walk_cb(uv_handle_t *handle, void *arg) {
	if(!uv_is_closing(handle)) {
		uv_close(handle, close_cb);
	}
}

static void stop(uv_timer_t *req) {
	plua_gc();
	eventpool_gc();
	uv_stop(uv_default_loop());
}

static void test_lua_async_timer_missing_parameters(CuTest *tc) {
	struct lua_state_t *state = NULL;

	printf("[ %-48s ]\n", __FUNCTION__);
	fflush(stdout);

	run = 0;
	test = 0;

	gtc = tc;
	memtrack();
	
	plua_init();
	plua_override_global("print", plua_print);
	plua_pause_coverage(1);

	state = plua_get_free_state();
	CuAssertPtrNotNull(tc, state);
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.callback();"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.setCallback(\"foo\");"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.setRepeat(-1);"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.setRepeat('a');"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.setTimeout(-1);"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.setTimeout('a');"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.start();"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.start(\"timer\");"));
	CuAssertIntEquals(tc, 1, luaL_dostring(state->L, "local timer = pilight.async.timer(); timer.stop(\"timer\");"));

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_walk(uv_default_loop(), walk_cb, NULL);
	uv_run(uv_default_loop(), UV_RUN_ONCE);

	while(uv_loop_close(uv_default_loop()) == UV_EBUSY) {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	while(lua_gettop(state->L) > 0) {
		lua_remove(state->L, -1);
	}
	plua_clear_state(state);

	plua_pause_coverage(0);
	plua_gc();
	CuAssertIntEquals(tc, 0, run);
	CuAssertIntEquals(tc, 0, xfree());
}

static void test_lua_async_timer(CuTest *tc) {
	const uv_thread_t pth_cur_id = uv_thread_self();
	memcpy((void *)&pth_main_id, &pth_cur_id, sizeof(uv_thread_t));

	struct lua_state_t *state = NULL;
	struct lua_State *L = NULL;
	char path[1024], *p = path, name[255];
	char *file = NULL;

	memset(name, 0, 255);

	printf("[ %-48s ]\n", __FUNCTION__);
	fflush(stdout);

	run = 0;
	test = 1;

	gtc = tc;
	memtrack();

	eventpool_init(EVENTPOOL_THREADED);

	plua_init();
	plua_override_global("print", plua_print);
	plua_pause_coverage(1);

	file = STRDUP(__FILE__);
	CuAssertPtrNotNull(tc, file);	

	str_replace("lua_async_timer.c", "", &file);

	memset(p, 0, 1024);
	snprintf(p, 1024, "%stimer.lua", file);
	FREE(file);
	file = NULL;

	plua_module_load(path, UNITTEST);

	CuAssertIntEquals(tc, 0, plua_module_exists("timer", UNITTEST));

	state = plua_get_free_state();
	CuAssertPtrNotNull(tc, state);
	CuAssertPtrNotNull(tc, (L = state->L));

	p = name;

	sprintf(name, "unittest.%s", "timer");
	lua_getglobal(L, name);
	CuAssertIntEquals(tc, LUA_TTABLE, lua_type(L, -1));

	struct plua_module_t *tmp = plua_get_modules();
	while(tmp) {
		if(strcmp("timer", tmp->name) == 0) {
			file = tmp->file;
			state->module = tmp;
			break;
		}
		tmp = tmp->next;
	}
	CuAssertPtrNotNull(tc, file);
	CuAssertIntEquals(tc, 1, call(L, file, "run"));

	plua_clear_state(state);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_walk(uv_default_loop(), walk_cb, NULL);
	uv_run(uv_default_loop(), UV_RUN_ONCE);

	while(uv_loop_close(uv_default_loop()) == UV_EBUSY) {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	plua_pause_coverage(0);
	plua_gc();
	CuAssertIntEquals(tc, 19, run);
	CuAssertIntEquals(tc, 0, xfree());
}

static void thread_free(uv_work_t *req, int status) {
	FREE(req);
}

static void thread(uv_work_t *req) {
	struct lua_state_t *state = NULL;
	struct lua_State *L = NULL;
	char name[255];
	char *file = NULL;

	memset(name, 0, 255);

	state = plua_get_free_state();
	CuAssertPtrNotNull(gtc, state);
	CuAssertPtrNotNull(gtc, (L = state->L));

	sprintf(name, "unittest.%s", "timer");
	lua_getglobal(L, name);
	CuAssertIntEquals(gtc, LUA_TTABLE, lua_type(L, -1));

	struct plua_module_t *tmp = plua_get_modules();
	while(tmp) {
		if(strcmp("timer", tmp->name) == 0) {
			file = tmp->file;
			state->module = tmp;
			break;
		}
		tmp = tmp->next;
	}
	CuAssertPtrNotNull(gtc, file);
	CuAssertIntEquals(gtc, 1, call(L, file, "run"));

	plua_clear_state(state);

}

static void test_lua_async_timer_threaded(CuTest *tc) {
	const uv_thread_t pth_cur_id = uv_thread_self();
	memcpy((void *)&pth_main_id, &pth_cur_id, sizeof(uv_thread_t));

	char path[1024], *p = path;
	char *file = NULL;

	printf("[ %-48s ]\n", __FUNCTION__);
	fflush(stdout);

	run = 0;
	test = 1;

	gtc = tc;
	memtrack();

	eventpool_init(EVENTPOOL_THREADED);

	plua_init();
	plua_override_global("print", plua_print);
	plua_pause_coverage(1);

	file = STRDUP(__FILE__);
	CuAssertPtrNotNull(gtc, file);

	str_replace("lua_async_timer.c", "", &file);

	memset(p, 0, 1024);
	snprintf(p, 1024, "%stimer.lua", file);
	FREE(file);
	file = NULL;

	plua_module_load(path, UNITTEST);

	CuAssertIntEquals(gtc, 0, plua_module_exists("timer", UNITTEST));

	uv_work_t *work_req = MALLOC(sizeof(uv_work_t));
	if(work_req == NULL) {
		OUT_OF_MEMORY /*LCOV_EXCL_LINE*/
	}

	uv_queue_work(uv_default_loop(), work_req, "foo", thread, thread_free);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_walk(uv_default_loop(), walk_cb, NULL);
	uv_run(uv_default_loop(), UV_RUN_ONCE);

	while(uv_loop_close(uv_default_loop()) == UV_EBUSY) {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	plua_pause_coverage(0);
	plua_gc();
	CuAssertIntEquals(tc, 19, run);
	CuAssertIntEquals(tc, 0, xfree());
}

static void test_lua_async_timer_bulk(CuTest *tc) {
	const uv_thread_t pth_cur_id = uv_thread_self();
	memcpy((void *)&pth_main_id, &pth_cur_id, sizeof(uv_thread_t));

	char path[1024], *p = path;
	char *file = NULL;

	printf("[ %-48s ]\n", __FUNCTION__);
	fflush(stdout);

	run = 0;
	test = 3;

	gtc = tc;
	memtrack();

	eventpool_init(EVENTPOOL_THREADED);

	plua_init();
	plua_override_global("print", plua_print);
	plua_pause_coverage(1);

	file = STRDUP(__FILE__);
	CuAssertPtrNotNull(gtc, file);

	str_replace("lua_async_timer.c", "", &file);

	memset(p, 0, 1024);
	snprintf(p, 1024, "%stimer.lua", file);
	FREE(file);
	file = NULL;

	plua_module_load(path, UNITTEST);

	CuAssertIntEquals(gtc, 0, plua_module_exists("timer", UNITTEST));

	if((timer_req = MALLOC(sizeof(uv_timer_t))) == NULL) {
		OUT_OF_MEMORY /*LCOV_EXCL_LINE*/
	}

	uv_timer_init(uv_default_loop(), timer_req);

	int i = 0;
	for(i=0;i<1000;i++) {
		uv_work_t *work_req = MALLOC(sizeof(uv_work_t));
		if(work_req == NULL) {
			OUT_OF_MEMORY /*LCOV_EXCL_LINE*/
		}

		uv_queue_work(uv_default_loop(), work_req, "foo", thread, thread_free);
	}
	uv_timer_start(timer_req, (void (*)(uv_timer_t *))stop, 2000, 0);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_walk(uv_default_loop(), walk_cb, NULL);
	uv_run(uv_default_loop(), UV_RUN_ONCE);

	while(uv_loop_close(uv_default_loop()) == UV_EBUSY) {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	plua_pause_coverage(0);
	eventpool_gc();
	// CuAssertIntEquals(tc, 19, run);
	CuAssertIntEquals(tc, 0, xfree());
}

static void test_lua_async_timer_prematurely_stopped(CuTest *tc) {
	struct lua_state_t *state = NULL;
	struct lua_State *L = NULL;
	char path[1024], *p = path, name[255];
	char *file = NULL;

	memset(name, 0, 255);

	printf("[ %-48s ]\n", __FUNCTION__);
	fflush(stdout);

	run = 0;
	test = 1;

	gtc = tc;
	memtrack();
	
	plua_init();
	plua_override_global("print", plua_print);
	plua_pause_coverage(1);

	file = STRDUP(__FILE__);
	CuAssertPtrNotNull(tc, file);	

	str_replace("lua_async_timer.c", "", &file);

	memset(p, 0, 1024);
	snprintf(p, 1024, "%stimer.lua", file);
	FREE(file);
	file = NULL;

	plua_module_load(path, UNITTEST);

	CuAssertIntEquals(tc, 0, plua_module_exists("timer", UNITTEST));

	if((timer_req = MALLOC(sizeof(uv_timer_t))) == NULL) {
		OUT_OF_MEMORY /*LCOV_EXCL_LINE*/
	}

	uv_timer_init(uv_default_loop(), timer_req);
	uv_timer_start(timer_req, (void (*)(uv_timer_t *))stop, 1000, 0);	
	
	state = plua_get_free_state();
	CuAssertPtrNotNull(tc, state);
	CuAssertPtrNotNull(tc, (L = state->L));

	p = name;

	sprintf(name, "unittest.%s", "timer");
	lua_getglobal(L, name);
	CuAssertIntEquals(tc, LUA_TTABLE, lua_type(L, -1));

	struct plua_module_t *tmp = plua_get_modules();
	while(tmp) {
		if(strcmp("timer", tmp->name) == 0) {
			file = tmp->file;
			state->module = tmp;
			break;
		}
		tmp = tmp->next;
	}
	CuAssertPtrNotNull(tc, file);
	CuAssertIntEquals(tc, 1, call(L, file, "run"));

	lua_pop(L, -1);

	plua_clear_state(state);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_walk(uv_default_loop(), walk_cb, NULL);
	uv_run(uv_default_loop(), UV_RUN_ONCE);

	while(uv_loop_close(uv_default_loop()) == UV_EBUSY) {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	plua_pause_coverage(0);
	plua_gc();
	CuAssertIntEquals(tc, 18, run);
	CuAssertIntEquals(tc, 0, xfree());
}

static void test_lua_async_timer_nonexisting_callback(CuTest *tc) {
	struct lua_state_t *state = NULL;
	struct lua_State *L = NULL;
	char path[1024], *p = path, name[255];
	char *file = NULL;

	printf("[ %-48s ]\n", __FUNCTION__);
	fflush(stdout);

	run = 0;
	test = 2;

	gtc = tc;
	memtrack();
	
	plua_init();
	plua_override_global("print", plua_print);
	plua_pause_coverage(1);

	file = STRDUP(__FILE__);
	CuAssertPtrNotNull(tc, file);	

	str_replace("lua_async_timer.c", "", &file);

	memset(p, 0, 1024);
	snprintf(p, 1024, "%stimer1.lua", file);
	FREE(file);
	file = NULL;

	plua_module_load(path, UNITTEST);

	CuAssertIntEquals(tc, 0, plua_module_exists("timer", UNITTEST));

	if((timer_req = MALLOC(sizeof(uv_timer_t))) == NULL) {
		OUT_OF_MEMORY /*LCOV_EXCL_LINE*/
	}

	uv_timer_init(uv_default_loop(), timer_req);
	uv_timer_start(timer_req, (void (*)(uv_timer_t *))stop, 1000, 0);

	state = plua_get_free_state();
	CuAssertPtrNotNull(tc, state);
	CuAssertPtrNotNull(tc, (L = state->L));

	p = name;

	sprintf(name, "unittest.%s", "timer");
	lua_getglobal(L, name);
	CuAssertIntEquals(tc, LUA_TTABLE, lua_type(L, -1));

	struct plua_module_t *tmp = plua_get_modules();
	while(tmp) {
		if(strcmp("timer", tmp->name) == 0) {
			file = tmp->file;
			state->module = tmp;
			break;
		}
		tmp = tmp->next;
	}
	CuAssertPtrNotNull(tc, file);
	CuAssertIntEquals(tc, -1, call(L, file, "run"));

	lua_pop(L, -1);

	plua_clear_state(state);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_walk(uv_default_loop(), walk_cb, NULL);
	uv_run(uv_default_loop(), UV_RUN_ONCE);

	while(uv_loop_close(uv_default_loop()) == UV_EBUSY) {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	plua_pause_coverage(0);
	plua_gc();
	CuAssertIntEquals(tc, 3, run);
	CuAssertIntEquals(tc, 0, xfree());
}

CuSuite *suite_lua_async_timer(void) {
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_lua_async_timer_missing_parameters);
	SUITE_ADD_TEST(suite, test_lua_async_timer);
	SUITE_ADD_TEST(suite, test_lua_async_timer_threaded);
	SUITE_ADD_TEST(suite, test_lua_async_timer_bulk);
	SUITE_ADD_TEST(suite, test_lua_async_timer_prematurely_stopped);
	SUITE_ADD_TEST(suite, test_lua_async_timer_nonexisting_callback);

	return suite;
}
