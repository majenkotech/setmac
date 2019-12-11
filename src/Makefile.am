sbin_PROGRAMS=setmac
setmac_SOURCES=setmac.c nvram.c

systemdsysdir = $(libdir)/systemd/system
systemdsys_DATA = setmac.service

SERVICE_SUBS = \
	s,[@]sbindir[@],$(sbindir),g

setmac.service: setmac.service.in
	sed -e '$(SERVICE_SUBS)' < $< > $@
