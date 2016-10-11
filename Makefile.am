bin_PROGRAMS = ticks-oanda

# We ship the git tree around because it includes version info, and is
# pretty small.
EXTRA_DIST = .git

ticks_oanda_CPPFLAGS = $(AMQ_CFLAGS)

ticks_oanda_LDADD = -lcurl $(AMQ_LIBS) -ljson-c
ticks_oanda_SOURCES = main.cc
