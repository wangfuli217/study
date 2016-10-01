# Copyright (c) 2013 ASMlover. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list ofconditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materialsprovided with the
#    distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

OUT	= devil.exe
CC	= cl -c -nologo
LINK	= link -nologo
MT	= mt -nologo
RM	= del
CFLAGS	= -O2 -W3 -MD -GS -Zi -Fd"vc.pdb" -DNDEBUG\
	-D_CRT_NONSTDC_NO_WARNINGS
LDFLAGS	= -INCREMENTAL -DEBUG -PDB:$(OUT).pdb -manifest\
	-manifestfile:$(OUT).manifest -manifestuac:no
OBJS	= devil_win_mutex.obj\
	devil_win_spinlock.obj\
	devil_allocator.obj\
	devil_queue.obj\
	devil_list.obj\
	devil_table.obj\
	devil_array.obj\
	devil_win_cond.obj\
	devil_win_thread.obj\
	\
	devil_test_main.obj\
	devil_test_allocator.obj\
	devil_test_queue.obj\
	devil_test_list.obj\
	devil_test_table.obj\
	devil_test_array.obj\
	devil_test_mutex.obj\
	devil_test_spinlock.obj\
	devil_test_cond.obj\
	devil_test_thread.obj

all: $(OUT)
rebuild: clean all
clean:
	$(RM) $(OUT) $(OBJS) *.pdb *.manifest *.ilk

$(OUT): $(OBJS)
	$(LINK) -out:$(OUT) $(OBJS) $(LDFLAGS)
	$(MT) -manifest $(OUT).manifest -outputresource:$(OUT);1
{.\src}.c{}.obj:
	$(CC) $(CFLAGS) $<
{.\src\win}.c{}.obj:
	$(CC) $(CFLAGS) $<
{.\test}.c{}.obj:
	$(CC) $(CFLAGS) $<
