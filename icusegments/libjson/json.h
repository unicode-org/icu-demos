/* ==========================================================================
 * json.h - Path Autovivifying JSON C Library
 * --------------------------------------------------------------------------
 * Copyright (c) 2012  William Ahern
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ==========================================================================
 */
#ifndef JSON_H
#define JSON_H

#include <stddef.h> /* size_t */

#include <setjmp.h> /* _setjmp(3) */


/*
 * J S O N  V E R S I O N  I N T E R F A C E S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define JSON_VERSION JSON_V_REL
#define JSON_VENDOR "william@25thandClement.com"

#define JSON_V_REL 0x20121101
#define JSON_V_ABI 0x20120512
#define JSON_V_API 0x20121101

int json_version(void);
const char *json_vendor(void);

int json_v_rel(void);
int json_v_abi(void);
int json_v_api(void);


/*
 * J S O N  C O R E  I N T E R F A C E S
 *
 * A note on parsing and composition of JSON documents: the routines
 * json_parse(), json_compose(), and json_getc() are stateful, which means
 * they can be called multiple times to parse or compose (print) successive
 * chunks of the JSON text document. These routines maintain state with
 * pointers to the current node and its data buffer.
 *
 * The routines json_load*() and json_print*() are not stateful, and neither
 * read nor write any persistent state information. The entire document is
 * parsed or composed in its entirety according to the respective input or
 * output parameters.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define JSON_F_PRETTY  1
#define JSON_F_TRACE   2
#define JSON_F_STRONG  4
#define JSON_F_AUTOVIV 8

struct json;

struct json *json_open(int flags, int *error);
/* Returns a new JSON object on success, with default parameters initialized
 * to `flags'. On failure returns NULL, with the error code in `error'.
 */

void json_close(struct json *J);
/* Destroys and deallocates the JSON object `J', iff `J' is not NULL. */

int json_parse(struct json *, const void *src, size_t len);
/* Parses `len' bytes of the JSON document chunk `src'. Returns 0 on success
 * or an error code on failure.
 */

int json_loadlstring(struct json *, const void *src, size_t len);
/* Parses the JSON document `src' with length `len'. The document should be
 * complete. Returns 0 on success or an error code on failure. */

int json_loadstring(struct json *, const char *src);
/* Parses the JSON document `src', which should be a NUL-terminated string.
 * The document should be complete. Returns 0 on success or an error code on
 * failure.
 */

int json_loadfile(struct json *, FILE *file);
/* Parses the JSON document `file'. The document should be complete. Returns
 * 0 on success or an error code on failure.
 */

int json_loadpath(struct json *, const char *path);
/* Parses the JSON document `path'. The document should be complete. Returns
 * 0 on success or an error code on failure.
 */

size_t json_compose(struct json *, void *dst, size_t lim, int flags, int *error);
/* Composes the next document chunk of the current composition context,
 * writing a maximum of `lim' bytes to `dst'. The chunk is NOT
 * NUL-terminated. `flags' is XORd with the flags passed to json_open().
 * Returns the count of bytes written on success, which may be 0 if the
 * entire document has been composed and returned. On failure returns 0 and
 * a non-0 error code through the optional parameter `error'.
 *
 * The caller should ensure that `error' is initialized to 0 if it wishes to
 * differentiate between success or failure when a count of 0 is returned.
 *
 * To reset the composition context, see json_flush().
 */

int json_getc(struct json *, int flags, int *error);
/* Composes the next character of the JSON document. `flags' is XORd with
 * the flags passed to json_open(). On success returns the next character,
 * or EOF if the end of the document has been reached. On failure return EOF
 * and a non-0 error code through the optional parameter `error'.
 *
 * The caller should ensure that `error' is initialized to 0 if it
 * wishes to diffentiate success or failure when EOF is returned.
 *
 * To reset the composition context, see json_flush().
 */

void json_flush(struct json *);
/* Resets the composition context so that the next call to json_compose() or
 * json_getc() returns the start of the JSON document.
 */

int json_printfile(struct json *, FILE *file, int flags);
/* Composes the JSON document into `file', independent of the internal
 * stateful composition context. On success returns 0, otherwise an error
 * code.
 */

size_t json_printstring(struct json *, void *dst, size_t lim, int flags, int *error);
/* Composes the JSON document into the buffer `dst', independent of the
 * internal stateful composition context. Only a maximum of `lim' bytes is
 * written, and `dst' is ALWAYS NUL-terminated iff `lim' is greater than 0.
 * `flags' is XORd with the flags passed to json_open(). On success returns
 * the logical string length, which if greater than or equal to `lim'
 * implies that the document has been truncated. On failure returns 0 and a
 * non-0 error code through the optional parameter `error'.
 */


/*
 * J S O N  V A L U E  I N T E R F A C E S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define JSON_M_AUTOVIV 0x01
#define JSON_M_CONVERT 0x02

struct json_value *json_v_search(struct json *, struct json_value *, int, const void *, size_t);

struct json_value *json_v_index(struct json *, struct json_value *, int, int);

int json_v_delete(struct json *, struct json_value *);

int json_v_clear(struct json *, struct json_value *);

double json_v_number(struct json *, struct json_value *);

const char *json_v_string(struct json *, struct json_value *);

size_t json_v_length(struct json *, struct json_value *);

size_t json_v_count(struct json *, struct json_value *);

_Bool json_v_boolean(struct json *, struct json_value *);

int json_v_setnumber(struct json *, struct json_value *, double);

int json_v_setlstring(struct json *, struct json_value *, const void *, size_t);

int json_v_setstring(struct json *, struct json_value *, const void *);

int json_v_setboolean(struct json *, struct json_value *, _Bool);

int json_v_setnull(struct json *, struct json_value *);

int json_v_setarray(struct json *, struct json_value *);

int json_v_setobject(struct json *, struct json_value *);


/*
 * J S O N  P A T H  I N T E R F A C E S
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int json_push(struct json *J, const char *, ...);

void json_pop(struct json *);

void json_popall(struct json *);

void json_delete(struct json *J, const char *, ...);

double json_number(struct json *, const char *, ...);

const char *json_string(struct json *J, const char *, ...);

size_t json_length(struct json *J, const char *, ...);

size_t json_count(struct json *J, const char *, ...);

_Bool json_boolean(struct json *J, const char *, ...);

int json_setnumber(struct json *, double, const char *, ...);

int json_setlstring(struct json *, const void *, size_t, const char *, ...);

int json_setstring(struct json *, const void *, const char *, ...);

int json_setboolean(struct json *, _Bool, const char *, ...);

int json_setnull(struct json *, const char *, ...);

int json_setarray(struct json *, const char *, ...);

int json_setobject(struct json *, const char *, ...);


/*
 * J S O N  E R R O R  I N T E R F A C E S
 *
 * JSON error conditions are returned using regular int objects. System
 * errors are loaded from errno as soon as encountered, and the value
 * returned through the JSON API. (DO NOT check errno, which may have been
 * overwritten by subsequent error handling code.) JSON errors are negative
 * and utilize a simple higher-order-byte namespacing protocol (as do all of
 * my projects). This works because ISO C and POSIX guarantee that all
 * system error codes are positive.
 *
 * When manipulating a JSON object with a long series of operations it may
 * be desirable to use an exception model for handling error conditions.
 * When a jmp_buf context is specified with json_setjmp(), error conditions
 * encountered by the tree manipulation routines will cause a _longjmp
 * instead of a return to the caller.
 *
 * DO NOT FORGET to restore the previous setjmp context, because the library
 * has no way of knowing if the presently set jmp_buf object is still valid.
 * 
 * The pattern should look something like:
 *
 * 	int foo(struct json *J) {
 * 		struct jsonxs xs;
 * 		int error;
 *
 * 		if ((error = json_enter(J, &xs)))
 * 			goto error;
 *
 * 		...
 *
 * 		json_leave(J, &xs);
 *
 * 		return 0;
 * 	error:
 * 		json_leave(J, &xs);
 *
 * 		return error;
 * 	}
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define JSON_EBASE -(('J' << 24) | ('S' << 16) | ('O' << 8) | 'N')
#define JSON_ERROR(error) ((error) >= JSON_EBASE && (error) < JSON_ELAST)

enum json_errors {
	JSON_EASSERT = JSON_EBASE,
	JSON_ELEXICAL,
	JSON_ESYNTAX,
	JSON_ETRUNCATED,
	JSON_ENOMORE,
	JSON_ETYPING,
	JSON_ELAST
}; /* enum json_errors */


struct jsonxs {
	jmp_buf trap, *otrap;
}; /* struct jsonxs */

jmp_buf *json_setjmp(struct json *, jmp_buf *);

#define json_enter(J, xs) ({ \
	(xs)->otrap = json_setjmp((J), &(xs)->trap); \
	_setjmp((xs)->trap); \
})

#define json_leave(J, xs) \
	json_setjmp((J), (xs)->otrap)

int json_throw(struct json *, int error);
/* Calls _longjmp(3) with `error' iff a non-NULL jmp_buf context is
 * currently set, otherwise returns `error'.
 */

int json_ifthrow(struct json *, int error);
/* Calls _longjmp(3) with `error' iff a non-NULL jmp_buf context is
 * currently set AND `error' is non-0. Otherwise, returns `error'.
 */

const char *json_strerror(int error);
/* Returns a string description of the error code `error'. System error
 * codes are passed to strerror(3).
 */


#endif /* JSON_H */
