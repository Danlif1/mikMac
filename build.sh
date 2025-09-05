sudo rm -rf ./bin/Debug/Debug/MyKext.kext || exit 1

xcodebuild -project MyKext.xcodeproj -scheme MyKext -configuration Debug build || exit 1

sudo chown -R root:wheel ./bin/Debug/Debug/MyKext.kext || exit 1

sudo kextload -v 6 ./bin/Debug/Debug/MyKext.kext
echo "Success! please enable the kext and restart your computer (can't be done from the cli)"
