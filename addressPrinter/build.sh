sudo rm -rf ./bin/Debug/Debug/AddressPrinter.kext || exit 1

xcodebuild -project AddressPrinter.xcodeproj -scheme AddressPrinter -configuration Debug build || exit 1

sudo chown -R root:wheel ./bin/Debug/Debug/AddressPrinter.kext || exit 1

sudo kextload -v 6 ./bin/Debug/Debug/AddressPrinter.kext
echo "Success! please enable the kext and restart your computer (can't be done from the cli)"
