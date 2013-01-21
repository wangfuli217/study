/*
 * Copyright (c) 2013 ASMlover. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list ofconditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materialsprovided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "btree.h"


static void 
visit_double(element_t e)
{
  fprintf(stdout, "btree element is : %lf\n", *(double*)e);
}

static void 
visit_destroy(element_t e)
{
  free(e);
}

static void 
btree_test(btree_t bt, int rand_n, double rand_d) 
{
  int i;
  double* d;

  fprintf(stdout, "btree testing ...\n");
  srand(time(0));
  for (i = 0; i < 5; ++i) {
    d = (double*)malloc(sizeof(*d));
    *d = rand() % rand_n * rand_d;
    fprintf(stdout, "btree insert {index=>%d, value=>%lf}\n", i, *d);
    btree_insert(bt, d);
  }
  fprintf(stdout, "btree size is : %d\n", btree_size(bt));
  fprintf(stdout, "pre-order traverse ===>\n");
  btree_preorder(bt, visit_double);
  fprintf(stdout, "in-order traverse ===>\n");
  btree_inorder(bt, visit_double);
  fprintf(stdout, "post-order traverse ===>\n");
  btree_postorder(bt, visit_double);

  fprintf(stdout, "nodes is : %d\n", btree_nodes(bt));
  fprintf(stdout, "leaves is : %d\n", btree_leaves(bt));
  fprintf(stdout, "height is : %d\n", btree_heights(bt));

  btree_preorder(bt, visit_destroy);
}


int 
main(int argc, char* argv[])
{
  btree_t bt = btree_create();

  btree_test(bt, 45646, 0.02341);

  btree_delete(&bt);
  return 0;
}
