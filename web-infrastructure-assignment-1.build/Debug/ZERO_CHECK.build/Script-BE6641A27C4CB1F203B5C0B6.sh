#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1
  make -f /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1
  make -f /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1
  make -f /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1
  make -f /Users/roshansevalia/Documents/coen162/web-infrastructure-assignment-1/CMakeScripts/ReRunCMake.make
fi

