# Utility functions for use in shell scripts.

## Generate a redirect string for use in reporting errors and the
#  like.  If no suitable stderr file can be found, returns (prints) an
#  empty string.
#
# The redirection operator needs to be included in the output to
# handle the case that we can't figure out where stderr is (so we can
# disable redirection altogether).
get_stderr_redirect()
{
    # All of these are symlinks, actually. On my system:
    # /dev/stderr       -> /dev/fd/2
    # /dev/fd	        -> /proc/self/fd
    # /proc/self/fd/2   -> (terminal device, probably the same as the
    #                       output of the `tty' command.)
    #
    # But I prefer to use the most symbolic one available, because
    # /dev/stderr makes more sense than something with numbers in
    # it. ;)
    for f in '/dev/stderr' '/dev/fd/2' '/proc/self/fd/2'; do
	# Use the first one that looks like a character special
	# device.
	if test -c "$f" ; then
	    echo "> $f"
	    return
	fi
    done
}

## Report a diagnostic message to the user via stderr.
#
# If DETAIL is given, it will be placed below the initial error
# message.  DETAIL will be automatically reformatted using fmt from
# coreutils.
#
# Usage: report_oops TYPE MESSAGE [DETAIL]
#
# Output is in the form below.
#
# *** TYPE: MESSAGE
# DETAIL
report_oops()
{

    local STR="*** $1: $2"

    # If DETAIL is given, append it to the output string after piping
    # it through fmt.
    if test x"$3" != x ; then
	STR="$STR\n`echo '$3' | fmt`"
    fi

    # Output the message.
    #
    # To redirect using the computed redirection string (possibly
    # empty), we need to use eval.
    #
    # N.B. the use of proper shell quoting for eval.
    eval echo -e "\"$STR\"" `get_stderr_redirect`
}

## Report an error using report_oops.
#
# Usage: report_error MESSAGE [DETAIL]
alias report_error='report_oops ERROR'

## Report a warning using report_oops.
#
# Usage: report_warning MESSAGE [DETAIL]
alias report_warning='report_oops WARNING'


## Report an error and kill the script.  Same syntax as report_error.
# Use this to avoid snafus when a critical item fails and you wish to
# abort the script.
die_error()
{
    report_error "$@"
    exit 1
}
