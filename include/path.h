#ifndef LOLLIPOP_PATH_H
#define LOLLIPOP_PATH_H

#define TUNTAPPATH	"/dev/net/tun"

#define INPUTSOCKET	"/var/run/lollipop.%s.socket"

#define SPOOLDIR	"/var/spool/lollipop/"
#define WAITINGDIR	SPOOLDIR "waiting/"
#define REMOVEDDIR	SPOOLDIR "removed/"
#define SENDINGDIR	SPOOLDIR "sending/"
#define SENTDIR		SPOOLDIR "sent/"

#endif	/* !LOLLIPOP_PATH_H */
