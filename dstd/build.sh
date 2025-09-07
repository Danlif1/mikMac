sudo rm -rf ./bin/Debug/Debug/dstd.kext || exit 1

xcodebuild -project dstd.xcodeproj -scheme dstd -configuration Debug build || exit 1

sudo chown -R root:wheel ./bin/Debug/Debug/dstd.kext || exit 1

sudo kextload -v 6 ./bin/Debug/Debug/dstd.kext
echo "Success! please enable the kext and restart your computer (can't be done from the cli)"
