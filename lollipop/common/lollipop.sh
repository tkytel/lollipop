#! /bin/sh

if [ $# -lt 1 ]
then
	echo 'usage: lollipop subcommand [options...] [args...]' >/dev/stderr
	exit 1
fi

lollipop="${0##*/}"
subcommand="$1"
shift
exec "$lollipop-$subcommand" "$@"
