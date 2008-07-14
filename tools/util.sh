# Utility functions for use in shell scripts.

## Generate a redirect string for use in reporting errors and the
#  like.  If no suitable stderr file can be found, returns (prints) an
#  empty string (this function MUST NOT fail).
get_stderr_redirect()
{
    for f in '/dev/stderr' '/dev/fd/1' '/proc/self/fd/1'; do
	if test -c "$f" ; then
	    echo "> $f"
	    return
	fi
    done
}

## Report a diagnostic message to the user.
#
# If DETAIL is given, it will be placed below the initial error
# message.  DETAIL will be automatically reformatted using coreutils
# programs.
#
# Usage: report_oops OOPS_TYPE MESSAGE [DETAIL]
report_oops()
{
    local STR="echo -e \"*** $1: $2\""

    # The sed command below removes linebreaks within a paragraph, but
    # not between two paragraphs.  The `M' (multiline) option to sed's
    # `s' command is a GNU extension.
    if test x"$3" != x ; then
	STR="$STR\"\n`echo '$3' | fmt`\""
    fi

    eval "$STR"`get_stderr_redirect`
}

## Report an error.
#
# Usage: report_error MESSAGE [DETAIL]
alias report_error='report_oops ERROR'

## Report a warning.
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
