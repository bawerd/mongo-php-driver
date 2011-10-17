//cursor.h
/**
 *  Copyright 2009-2011 10gen, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MONGO_CURSOR_H
#define MONGO_CURSOR_H 1

// Cursor flags
#define TAILABLE 2
#define SLAVE_OKAY 4
#define OPLOG_REPLAY 8
#define NO_CURSOR_TO 16
#define AWAIT_DATA 32
#define EXHAUST 64

void php_mongo_cursor_free(void *object TSRMLS_DC);

/**
 * Queries the database. Returns SUCCESS or FAILURE.
 */
int mongo_cursor__do_query(zval *this_ptr, zval *return_value TSRMLS_DC);

/**
 * If the query should be send to the db or not.  The rules are:
 * - db commands should only be sent onces (no retries)
 * - normal queries should be sent up to 5 times
 * This uses exponential backoff with a random seed to avoid flooding a
 * struggling db with retries.
 */
int mongo_cursor__should_retry(mongo_cursor *cursor);

PHP_METHOD(MongoCursor, __construct);
PHP_METHOD(MongoCursor, getNext);
PHP_METHOD(MongoCursor, hasNext);
PHP_METHOD(MongoCursor, limit);
PHP_METHOD(MongoCursor, batchSize);
PHP_METHOD(MongoCursor, skip);
PHP_METHOD(MongoCursor, fields);
PHP_METHOD(MongoCursor, slaveOkay);
PHP_METHOD(MongoCursor, tailable);
PHP_METHOD(MongoCursor, immortal);
PHP_METHOD(MongoCursor, timeout);
PHP_METHOD(MongoCursor, dead);
PHP_METHOD(MongoCursor, snapshot);
PHP_METHOD(MongoCursor, sort);
PHP_METHOD(MongoCursor, hint);
PHP_METHOD(MongoCursor, addOption);
PHP_METHOD(MongoCursor, explain);
PHP_METHOD(MongoCursor, doQuery);
PHP_METHOD(MongoCursor, current);
PHP_METHOD(MongoCursor, key);
PHP_METHOD(MongoCursor, next);
PHP_METHOD(MongoCursor, rewind);
PHP_METHOD(MongoCursor, valid);
PHP_METHOD(MongoCursor, reset);
PHP_METHOD(MongoCursor, count);
PHP_METHOD(MongoCursor, info);

#define preiteration_setup   mongo_cursor *cursor;                      \
  PHP_MONGO_GET_CURSOR(getThis());                                      \
                                                                        \
  if (cursor->started_iterating) {                                      \
    zend_throw_exception(mongo_ce_CursorException,                      \
                         "cannot modify cursor after beginning iteration.", 0 \
                         TSRMLS_CC);                                    \
    return;                                                             \
  }

#define default_to_true(bit)                                            \
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &z) == FAILURE) { \
    return;                                                             \
  }                                                                     \
                                                                        \
  if (z) {                                                              \
    cursor->opts |= 1 << bit;                                           \
  } else {                                                              \
    cursor->opts &= !(1 << bit);                                        \
  }

PHP_METHOD(MongoCursorException, getHost);

void mongo_init_CursorExceptions(TSRMLS_D);

/**
 * Throw a MongoCursorException with the given code and message.
 * Uses the server to fill in information about the connection that cause the
 * exception. Does nothing if an exception has already been thrown.
 */
zval* mongo_cursor_throw(mongo_server *server, int code TSRMLS_DC, char *format, ...);

/**
 * The cursor_list
 *
 * In PHP, garbage collection works via reference counting.  MongoCursor
 * contains a reference to its "parent" Mongo instance, so it increments the
 * Mongo's reference count in the constructor.
 *
 * Depending on app server/code, MongoCursor could be destroyed before or after
 * Mongo.  If Mongo is destroyed first, we want to kill all open cursors using
 * that connection before destroying the connection.  So, mongo_cursor_free_le,
 * when given a MONGO_LINK, will kill all cursors associated with that link.
 * When given a MONGO_CURSOR, it will destroy exactly that cursor (and no
 * others).  This also removes it from the cursor_list.
 */

/**
 * This kills a cursor or all cursors for a given link, depending on the type
 * given. Also removes killed cursor(s) from the cursor_list.
 */
void mongo_cursor_free_le(void* val, int type TSRMLS_DC);

/**
 * Adds a cursor to the cursor_list.
 *
 * A cursor can only be added once to the cursor list.  If cursor is already on
 * the list, this does nothing.  This creates the cursor_list if it does not
 * exist.
 */
int php_mongo_create_le(mongo_cursor *cursor, char *name TSRMLS_DC);

/**
 * Actually removes a cursor_node node from the linked list.
 */
void php_mongo_free_cursor_node(cursor_node*, zend_rsrc_list_entry*);

/**
 * Persistent list destructor.
 */
void php_mongo_cursor_list_pfree(zend_rsrc_list_entry* TSRMLS_DC);

#endif
