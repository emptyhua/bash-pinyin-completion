#!/bin/bash
TMP=/tmp
RELEASE=$1
VERSION=$(cat ./VERSION)~${RELEASE}1
TODAY=$(date -R)

echo "Release $RELEASE"
echo "Version $VERSION"

BUILD_DIR="${TMP}/bash_pinyin_completion_${VERSION}_${RELEASE}"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
SRC_DIR="$BUILD_DIR/bash-pinyin-completion_${VERSION}"
mkdir -p "$SRC_DIR"

cp -r ./* "$SRC_DIR"

cd "$SRC_DIR"
sed -i -e "s/VERSION/${VERSION}/" -e "s/TIME/${TODAY}/" -e "s/RELEASE/${RELEASE}/" ./debian/changelog
sed -i -e "s/VERSION/${VERSION}/" ./debian/control
cd $SRC_DIR
debuild -S -sa
cd $BUILD_DIR
dput ppa:emptyhua/toolbox *.changes
