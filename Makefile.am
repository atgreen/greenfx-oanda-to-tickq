bin_PROGRAMS = oanda-to-tickq

# We ship the git tree around because it includes version info, and is
# pretty small.
EXTRA_DIST = .git COPYING3

oanda_to_tickq_CPPFLAGS = $(AMQ_CFLAGS) -Wall -Wno-deprecated-declarations

oanda_to_tickq_LDADD = -lcurl $(AMQ_LIBS) -ljson-c
oanda_to_tickq_SOURCES = main.cc
