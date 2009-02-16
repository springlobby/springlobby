#!/usr/bin/awk -f

# Author        : Tobi Vollebregt
# Creation date : 6 january 2009

#
# This script analyzes the source code of springunitsynclib.cpp (well, you
# need to give it 'src/springunitsynclib.cpp' as argument) for a simple case
# of deadlock: a lock being taken twice by the same thread.
#
# It does this by:
# 1) constructing the callgraph of the SpringUnitSyncLib class,
# 2) collecting the set of methods that take the unitsync lock,
# 3) propagating this 'takes lock' attribute up through the callgraph,
# 4) checking for any pairs (caller,callee) of which caller takes the lock
#    directly and callee takes the lock either directly or indirectly.
#


BEGIN {
	SUBSEP = ","
}


# C++ style comments are ignored as long as they are on their own on the line.
/^[ \t]*\/\/.*$/ { next; }

# Keep the current function name in the variable <function_name>
/SpringUnitSyncLib::[_A-Za-z][_A-Za-z0-9]*[ \t]*\(/ {
	gsub( /^.*SpringUnitSyncLib::/, "" );
	gsub( /\(.*$/, "" );
	function_name = $0;
}

# Keep track of whether we currently are in a function.
/^{/ { in_function = 1; }
/^}/ { in_function = 0; }

# Create a set of functions which take the unitsync lock.
/InitLib|LOCK_UNITSYNC/ {
	if ( in_function )
		functions_with_lock[function_name] = 1;
}

# Update <callgraph> for the current function with everything that looks
# like an identifier.  This also includes keywords and the like, but given
# that keywords will never be in <functions_with_lock>, it does not matter
# for the end result.
/[_A-Za-z][_A-Za-z0-9]*/ {
	if ( in_function ) {
		gsub( /"[^"]*"/, "" ); # Remove strings.
		gsub( /'[^']*'/, "" ); # Remove character constants.
		split( $0, syms, /[^_A-Za-z0-9]+/ )
		for ( i in syms ) {
			if ( syms[i] ~ /^[_A-Za-z][_A-Za-z0-9]*$/ )
				callgraph[function_name, syms[i]] = NR;
		}
		delete syms;
	}
}


END {
	# Propagate the 'takes lock' attribute up through the callgraph.
	do {
		done = 1;
		for ( key in callgraph ) {
			split( key, fun, "," );
			if ( fun[2] in functions_with_lock && !(fun[1] in functions_with_lock) ) {
				functions_with_lock[fun[1]] = 1 + functions_with_lock[fun[2]];
				done = 0;
			}
		}
	}
	while ( !done );

	# Output warnings for all calls that may cause a deadlock.
	errcount = 0;
	for ( key in callgraph ) {
		split( key, fun, "," );
		if ( fun[1] != fun[2] && functions_with_lock[fun[1]] == 1 && functions_with_lock[fun[2]] >= 1 ) {
			printf "%s:%d: Error: %s (%d) calls %s (%d) and both take unitsync lock\n", \
				FILENAME, callgraph[key], fun[1], functions_with_lock[fun[1]], fun[2], functions_with_lock[fun[2]];
			++errcount;
		}
	}
	if ( errcount > 0 )
		print "This will results in (a) deadlock(s) at runtime.";
	exit errcount;
}
