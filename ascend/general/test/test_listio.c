/*
 *  Unit test functions for ASCEND: general/listio.c
 *
 *  Copyright (C) 2005 Jerry St.Clair
 *
 *  This file is part of the Ascend Environment.
 *
 *  The Ascend Environment is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The Ascend Environment is distributed in hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <ascend/general/platform.h>
#ifdef __WIN32__
#include <io.h>
#endif
#include <ascend/general/ascMalloc.h>
#include <ascend/general/list.h>
#include <ascend/general/listio.h>

#include <test/common.h>
#include <test/assertimpl.h>
#include "test/redirectStdStreams.h"

#define USE_REDIRECT 0

/*
 *  listio.[ch] implements writing of a list's details to file.
 *  Because there is no specification on the file contents, this
 *  test routine does not do more than check boundary conditions
 *  and make sure something is written to a specified file stream.
 */
static void test_listio(void){

  struct gl_list_t *p_list1;
  unsigned long *pint_array[20];
  unsigned long i;
  FILE *file_normal;
#if USE_REDIRECT /* removing stream redirection (too damn confusing) */
  FILE *file_stderr;
#endif
  int i_initialized_lists = FALSE;
  unsigned long prior_meminuse;

  prior_meminuse = ascmeminuse();       /* save meminuse() at start of test function */

#ifdef NDEBUG
  CU_FAIL("test_listio() compiled with NDEBUG - some features not tested.");
#endif

  /* set up pooling & recycling */
  if (FALSE == gl_pool_initialized()) {
    gl_init();
    gl_init_pool();
    i_initialized_lists = TRUE;
  }

  for (i=0 ; i<20 ; ++i) {              /* create some test data */
      pint_array[i] = ASC_NEW(unsigned long);
      *pint_array[i] = i;
  }

  p_list1 = gl_create(20);              /* create and fill a list with data */
  for (i=0 ; i<20 ; ++i) {
      gl_append_ptr(p_list1, pint_array[i]);
  }

  /* test gl_write_list() */

#ifndef ASC_NO_ASSERTIONS
  asc_assert_catch(TRUE);          /* prepare to test assertions */

  asc_assert_reset();
  if (0 == setjmp(g_asc_test_env))
    gl_write_list(NULL, NULL);          /* error if NULL list* */
  CU_TEST(TRUE == asc_assert_failed());

  asc_assert_catch(FALSE);         /* done testing assertions */
#endif    /* !ASC_NO_ASSERTIONS */

  if (NULL != (file_normal = fopen("listiotempfile.tmp", "w+"))) {

    gl_write_list(file_normal, p_list1);/* write to normal open file */
    rewind(file_normal);
    CU_TEST(EOF != fgetc(file_normal)); /* test that file is not empty */
    fclose(file_normal);
  }
  else {
    CU_FAIL("Error opening output file 1 in test_listio.c");
  }

#if USE_REDIRECT /* removing stream redirection (too damn confusing) */

  if (NULL != (file_stderr = redirect_stderr("listiotempstderr.tmp"))) {

    gl_write_list(NULL, p_list1);       /* write to stderr */
    rewind(file_stderr);

    if (NULL != (file_normal = fopen("listiotempfile.tmp", "r"))) {
      int ch1 = fgetc(file_stderr);
      int ch2 = fgetc(file_normal);
      while ((EOF != ch1) && (EOF != ch2)) {  /* test that files are the same */
        if (ch1 != ch2) {
          CU_FAIL("Files differ in test_listio.");
          break;
        }
        ch1 = fgetc(file_stderr);
        ch2 = fgetc(file_normal);
      }
      if (ch1 == ch2) {
        CU_PASS("Files are the same in test_listio.");
      }
      fclose(file_normal);
    }
    reset_stderr();
    remove("listiotempstderr.tmp");
  }
  else {
    CU_FAIL("Error opening output file 2 in test_listio.c");
  }
  remove("listiotempfile.tmp");
#endif

  gl_destroy(p_list1);                  /* clean up the list, preserving data */

  /* clean up and exit */
  for (i=0 ; i<20 ; ++i)
    ascfree(pint_array[i]);

  if (TRUE == i_initialized_lists) {
    gl_destroy_pool();
  }

  CU_TEST(prior_meminuse == ascmeminuse());   /* make sure we cleaned up after ourselves */
}

/*
  This test plays with gl_write_list_str:
*/
static void test_str(void){
  struct gl_list_t *list1;

  // PREPARE 
  int i_initialized_lists = FALSE;
  unsigned long prior_meminuse;
  prior_meminuse = ascmeminuse();       /* save meminuse() at start of test function */

#ifdef WIN32
  char tmpl[PATH_MAX];
  snprintf(tmpl,PATH_MAX,"%s\\.asctempXXXXXX",getenv("HOME"));
  fprintf(stderr,"tmpl = %s\n",tmpl);
  int fd = mkstemp(tmpl);
  if(-1==fd){
    perror("mkstemp");
    CU_FAIL("failed mkstemp");
    return;
  }
  FILE *tmp = fdopen(fd,"w+");
  if(tmp == NULL){
    perror("fdopen");
#else
  FILE *tmp = tmpfile();
  if(tmp == NULL){
    perror("tmpfile");
#endif
    CU_FAIL("failed to open temporary file");
    return;
  }

  /* set up pooling & recycling */
  if (FALSE == gl_pool_initialized()) {
    gl_init();
    gl_init_pool();
    i_initialized_lists = TRUE;
  }

  // TEST 1, 2, 3
  
  char buffer[4096];
  
  fputs("hello world",tmp);
  rewind(tmp);
  char *rb = fgets(buffer,4096,tmp);
  if (!rb) {
	  CU_ASSERT(NULL=="failed to read constant from stack memory");
  }
  CU_ASSERT(0==strcmp(buffer,"hello world"));
  rewind(tmp);
  
  list1 = gl_create(0);
  gl_append_ptr(list1, "one");
  gl_append_ptr(list1, "two");
  gl_append_ptr(list1, "three");

  //fprintf(stderr,"list =");
  //gl_write_list_str(stderr, list1, &gl_write_list_item_str);
  //fprintf(stderr,"\n");

  gl_write_list_str(tmp, list1, &gl_write_list_item_str);
  rewind(tmp);
  rb = fgets(buffer,4096,tmp);
  if (!rb) {
	  CU_ASSERT(NULL=="failed to read gl_write_list_str result from stack memory");
  }
  CU_ASSERT(0==strcmp(buffer,"(one,two,three)"));

  // TEST SORTED LIST
  
  gl_destroy(list1);
  list1 = gl_create(0);
  
  gl_insert_sorted(list1,"person",(CmpFunc)strcmp);
  gl_insert_sorted(list1,"woman",(CmpFunc)strcmp);
  gl_insert_sorted(list1,"man",(CmpFunc)strcmp);
  gl_insert_sorted(list1,"camera",(CmpFunc)strcmp);
  gl_insert_sorted(list1,"TV",(CmpFunc)strcmp);

  //fprintf(stderr,"list =");
  //gl_write_list_str(stderr, list1, &gl_write_list_item_str);
  //fprintf(stderr,"\n");
  
  rewind(tmp);
  gl_write_list_str(tmp, list1, &gl_write_list_item_str);
  rewind(tmp);
  rb = fgets(buffer,4096,tmp);
  if (!rb) {
	  CU_ASSERT(NULL=="failed to read long gl_write_list_str result from stack memory");
  }
  CU_ASSERT(0==strcmp(buffer,"(TV,camera,man,person,woman)"));

  gl_destroy(list1);
  
  // TEST WITH NULLS
  
  list1 = gl_create(0);
  gl_append_ptr(list1, "one");
  gl_append_ptr(list1, "two");
  gl_append_ptr(list1, NULL);
  gl_append_ptr(list1, "three");
  gl_append_ptr(list1, NULL);

  //fprintf(stderr,"list =");
  //gl_write_list_str(stderr, list1, &gl_write_list_item_str);
  //fprintf(stderr,"\n");

  rewind(tmp);
  gl_write_list_str(tmp, list1, &gl_write_list_item_str);
  rewind(tmp);
  rb = fgets(buffer,4096,tmp);
  if (!rb) {
	  CU_ASSERT(NULL=="failed to read longer gl_write_list_str result from stack memory");
  }
  CU_ASSERT(0==strcmp(buffer,"(one,two,(NULL),three,(NULL))"));

  gl_destroy(list1);
    
  // CLEAN UP

  if(tmp)fclose(tmp);

  if (TRUE == i_initialized_lists) {
    gl_destroy_pool();
  }

  CU_TEST(prior_meminuse == ascmeminuse());   /* make sure we cleaned up after ourselves */
}


/*===========================================================================*/
/* Registration information */

#define TESTS(T) \
  T(listio) \
  T(str)

REGISTER_TESTS_SIMPLE(general_listio, TESTS)

// vim:syntax=python:ts=2:sw=2:et
