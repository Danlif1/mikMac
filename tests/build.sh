sudo rm -rf ./bin/Debug/Debug/tests.kext || exit 1

xcodebuild -project tests.xcodeproj -scheme tests -configuration Debug build || exit 1

sudo chown -R root:wheel ./bin/Debug/Debug/tests.kext || exit 1

sudo kextload -v 6 ./bin/Debug/Debug/tests.kext
echo "Success! please enable the kext and restart your computer (can't be done from the cli)"
