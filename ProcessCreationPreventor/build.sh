sudo rm -rf ./bin/Debug/Debug/ProcessCreationPreventor.kext || exit 1

xcodebuild -project ProcessCreationPreventor.xcodeproj -scheme ProcessCreationPreventor -configuration Debug build || exit 1

sudo chown -R root:wheel ./bin/Debug/Debug/ProcessCreationPreventor.kext || exit 1

sudo kextload -v 6 ./bin/Debug/Debug/ProcessCreationPreventor.kext
echo "Success! please enable the kext and restart your computer (can't be done from the cli)"
