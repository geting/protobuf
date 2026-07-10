#!/bin/sh

# Run this script to generate the configure script and other files that will
# be included in the distribution.  These files are not checked in because they
# are automatically generated.

set -e

verify_sha256() {
  archive=$1
  expected=$2
  if command -v sha256sum >/dev/null 2>&1; then
    echo "$expected  $archive" | sha256sum -c -
  elif command -v shasum >/dev/null 2>&1; then
    actual=`shasum -a 256 "$archive" | awk '{print $1}'`
    test "$actual" = "$expected"
  else
    echo "No SHA-256 verification tool found." >&2
    exit 1
  fi
}

if [ ! -z "$@" ]; then
  for argument in "$@"; do
    case $argument in
	  # make curl silent
      "-s")
        curlopts="-s"
        ;;
    esac
  done
fi


# Check that we're being run from the right directory.
if test ! -f src/google/protobuf/stubs/common.h; then
  cat >&2 << __EOF__
Could not find source code.  Make sure you are running this script from the
root of the distribution tree.
__EOF__
  exit 1
fi

# Check that gmock is present.  Usually it is already there since the
# directory is set up as an SVN external.
if test ! -e gmock; then
  echo "Google Mock not present.  Fetching gmock-1.7.0 from the web..."
  curl $curlopts -fL -O https://github.com/google/googlemock/archive/release-1.7.0.zip
  verify_sha256 release-1.7.0.zip \
    407992e9ef17a08339cd383c33dbaff923969cfa01f8e4ceaeea679400016d85
  unzip -q release-1.7.0.zip
  rm release-1.7.0.zip
  mv googlemock-release-1.7.0 gmock

  curl $curlopts -fL -O https://github.com/google/googletest/archive/release-1.7.0.zip
  verify_sha256 release-1.7.0.zip \
    b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0
  unzip -q release-1.7.0.zip
  rm release-1.7.0.zip
  mv googletest-release-1.7.0 gmock/gtest
fi

set -ex

# TODO(kenton):  Remove the ",no-obsolete" part and fix the resulting warnings.
autoreconf -f -i -Wall,no-obsolete

rm -rf autom4te.cache config.h.in~
exit 0
