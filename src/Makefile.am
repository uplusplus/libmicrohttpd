if HAVE_CURL
curltests = testcurl
if HAVE_ZZUF
if HAVE_SOCAT
zzuftests = testzzuf
endif
endif
endif
if ENABLE_SPDY
if HAVE_OPENSSL
microspdy = microspdy 
if HAVE_CURL
microspdy += spdy2http
endif
if HAVE_SPDYLAY
microspdy += testspdy
endif
endif
endif

SUBDIRS = include microhttpd $(microspdy) examples $(curltests) $(zzuftests) . 

EXTRA_DIST = \
 datadir/cert-and-key.pem \
 datadir/cert-and-key-for-wireshark.pem \
 datadir/spdy-draft.txt
